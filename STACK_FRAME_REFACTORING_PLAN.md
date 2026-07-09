# Multi-Pass Stack Frame Management Refactoring Plan

## Problem Statement

The current single-pass code generator has **decentralized stack management** with three independent counters (`currentStackOffset`, `tempSpaceInBlock`, `totalAllocaSpace`) that each instruction handler must manually keep in sync. This has produced:

- **Stack space leaks** (`lowerPtrToInt`/`lowerIntToPtr` decrement `currentStackOffset` without tracking cleanup)
- **Lost values across blocks** (`tempSpaceInBlock` freed at block end corrupts values needed later)
- **Fragile epilogue** (`lowerRetTerm` reverse-engineers `allocaEndOffset` to compute cleanup)
- **30+ duplicated bp-offset+2 parsing patterns** (inconsistent parsing, one helper `makeBpOffset` exists but is rarely used)
- **`alloc32BitStack` bug** (creates `sub sp, 4` but never emits it)
- **32-bit arg detection** falls back to return type instead of argument types
- **No global frame planning** (each handler emits its own `sub sp`/`add sp` inline)

## Proposed Architecture: Two-Pass Lowering with Central StackFrame

```
┌─────────────────────────────────────────────────────────────┐
│ Pass 1: Function Analysis                                   │
│   Walk IR basic blocks & instructions                        │
│   Collect: alloca sizes, 32-bit vregs, cross-block liveness,│
│            max temp space per block, parameter info         │
│   Output: StackFrame (pre-populated)                         │
├─────────────────────────────────────────────────────────────┤
│ Pass 2: Frame Layout Computation                              │
│   StackFrame.computeLayout() assigns final offsets           │
│   Computes single frameSize = localsSize + tempSize          │
│   Assigns each vreg a fixed slot (alloca, spill, or temp)    │
├─────────────────────────────────────────────────────────────┤
│ Pass 3: Instruction Lowering (modified existing handlers)    │
│   Each handler queries StackFrame for slot offsets           │
│   No inline sub sp / add sp — frame pre-allocated            │
│   Centralized 32-bit load/store helpers                       │
│   Centralized operand materialization                       │
├─────────────────────────────────────────────────────────────┤
│ Pass 4: Emission                                               │
│   Prologue: push bp; mov bp,sp; push bx/si/di; sub sp,N     │
│   Epilogue: add sp,N; pop di/si/bx/bp; ret                   │
│   Single sub/add sp instead of scattered inline ones         │
└─────────────────────────────────────────────────────────────┘
```

## Stack Frame Layout (Target)

```
Higher addresses (caller's stack):
  [bp+4+...]   arguments (right-to-left, caller-pushed)
  [bp+2]       return address
  [bp]        ─── saved BP ───
  [bp-2]      saved BX
  [bp-4]      saved SI
  [bp-6]      saved DI
  [bp-6-L]    local slots (allocas + cross-block spill slots)
  [bp-6-L-T]  temp scratch (pre-allocated max, reused per block)

  Total sub sp = L + T  (single allocation in prologue)
```

Where:
- **L** = sum of all local slot sizes (allocas + cross-block spill slots)
- **T** = max temp space needed by any single basic block

Key differences from current:
- **32-bit parameters** stay at their caller-pushed location (`[bp+offset]`). No copy to local stack. The vreg mapping points directly to the positive bp offset.
- **No per-block `add sp`** — temp space is part of the pre-allocated frame, reused across blocks.
- **No inline `sub sp`** in instruction handlers — all slots pre-assigned.

## New Files (4 files)

### 1. `src/codegen/StackFrame.h`

```cpp
enum class SlotKind {
    Param,
    Alloca,
    Spill,
    Temp
};

struct SlotInfo {
    std::string vregName;
    int bpOffset;
    int byteSize;
    bool is32bit;
    SlotKind kind;
    std::string paramReg;
};

class StackFrame {
public:
    // --- Pass 1: Analysis ---
    void addParam(const std::string& name, int bitWidth, const std::string& reg);
    void addAllocaSlot(const std::string& vregName, int byteSize);
    void addSpillSlot(const std::string& vregName, int byteSize, bool is32bit);
    void setMaxTempSpace(int bytes);

    // --- Pass 2: Layout ---
    void computeLayout();
    int getFrameSize() const;

    // --- Pass 3: Accessors ---
    bool hasSlot(const std::string& vregName) const;
    std::string getBpOffset(const std::string& vregName) const;
    std::string getHighBpOffset(const std::string& vregName) const;
    int getSlotSize(const std::string& vregName) const;
    bool is32bit(const std::string& vregName) const;
    bool isAlloca(const std::string& vregName) const;
    bool isParam(const std::string& vregName) const;
    std::string getParamReg(const std::string& vregName) const;

    // Temp space (reset per block, no sub sp needed)
    std::string allocTemp(int byteSize, bool is32bit = false);
    void resetTemp();

    // Register tracking
    std::string getPhysReg(const std::string& vregName) const;
    void setPhysReg(const std::string& vregName, const std::string& reg);
    std::string getFreeTempReg() const;
    void freeReg(const std::string& reg);

    // Operand classification
    enum class OperandKind { Constant, Register, Memory, Global, Alloca, Unknown };
    OperandKind classifyOperand(const std::string& name) const;

    // 32-bit helpers
    void emitLoad32(std::vector<Instruction286>& output, const std::string& vregName,
                     const std::string& lowReg, const std::string& highReg) const;
    void emitStore32(std::vector<Instruction286>& output, const std::string& vregName,
                      const std::string& lowReg, const std::string& highReg) const;

private:
    std::vector<SlotInfo> slots;
    std::map<std::string, int> vregToSlotIndex;
    std::map<std::string, std::string> vregToPhys_;
    std::map<std::string, std::string> physToVreg_;

    int savedRegsSize = 6;
    int nextLocalOffset = -6;
    int localsSize = 0;
    int tempSize = 0;
    int totalFrameSize = 0;
    int tempBase = 0;
    int tempCurrent = 0;
};
```

### 2. `src/codegen/StackFrame.cpp`

Implementation of all StackFrame methods:
- `addParam()`, `addAllocaSlot()`, `addSpillSlot()`, `setMaxTempSpace()` — analysis methods
- `computeLayout()` — assigns offsets for locals then temp area
- `getBpOffset()`, `getHighBpOffset()` — centralized bp-offset computation (replaces 30+ duplicated patterns)
- `allocTemp()`, `resetTemp()` — temp space allocation within pre-allocated area
- `emitLoad32()`, `emitStore32()` — centralized 32-bit load/store (replaces duplicated patterns)
- `classifyOperand()` — centralized operand classification
- Register tracking methods

### 3. `src/codegen/FunctionAnalysis.h`

```cpp
class FunctionAnalysis {
public:
    static void analyze(const ir::Function& func, StackFrame& frame,
                        const std::map<std::string, std::vector<int>>& funcParamBitWidths);

private:
    struct BlockInfo {
        std::set<std::string> defined;
        std::set<std::string> used;
        int tempSpaceNeeded = 0;
    };

    static void analyzeParams(const ir::Function& func, StackFrame& frame,
                              const std::map<std::string, std::vector<int>>& funcParamBitWidths);
    static void analyzeBlocks(const ir::Function& func, StackFrame& frame,
                              std::vector<BlockInfo>& blockInfos);
    static void computeLiveness(const std::vector<BlockInfo>& blockInfos,
                                StackFrame& frame);
    static void computeMaxTempSpace(const std::vector<BlockInfo>& blockInfos,
                                     StackFrame& frame);
};
```

### 4. `src/codegen/FunctionAnalysis.cpp`

Implementation of FunctionAnalysis:
- `analyzeParams()` — walks function parameters, adds to StackFrame
- `analyzeBlocks()` — walks basic blocks, collects defined/used vregs, estimates temp space
- `computeLiveness()` — identifies cross-block vregs that need spill slots
- `computeMaxTempSpace()` — computes max temp space needed across all blocks

**Liveness algorithm** (SSA form simplifies this):
1. Walk each basic block, collect `defined` (result names) and `used` (operand names) vreg sets
2. For each vreg, find its defining block
3. A vreg is "cross-block" if it appears in `used` of any block ≠ its defining block
4. Cross-block vregs get spill slots (4 bytes if 32-bit, 2 bytes if 16-bit)
5. Single-block vregs use temp space (within pre-allocated area)

**Temp space estimation per block**: For each instruction that currently allocates temp space (32-bit arithmetic results, 32-bit conversions, GEP results, etc.), add 4 bytes. Take the max across all blocks.

## Modified Files (15 files)

### 5. `src/codegen/InstructionSelectInternal.h`

**Replace** `SelectorState` with a struct that contains a `StackFrame`:

```cpp
struct SelectorState {
    StackFrame frame;
    std::map<std::string, std::string> aliasMap;
    int labelCounter = 0;
    const ir::Function* currentFunc = nullptr;
    std::map<std::string, std::vector<int>> funcParamBitWidths;

    // Removed (moved to StackFrame):
    // - currentStackOffset, tempSpaceInBlock, allocaEndOffset, totalAllocaSpace
    // - vregToStackOffset, is32bit, allocaVregs
    // - load32Bit, store32Bit, alloc32BitStack, allocateStack
    // - getPhysReg, getFreeTempReg, assignReg, updateResultReg, freeReg

    int nextLabel(const std::string& prefix = ".L");
};
```

### 6. `src/codegen/InstructionSelect.cpp`

**`lowerFunction`** — major rewrite:

```cpp
std::vector<LoweredInstruction> InstructionSelector::lowerFunction(const ir::Function& func) {
    impl->currentFunc = &func;
    impl->frame = StackFrame();  // reset frame

    // Pass 1: Analyze the function
    FunctionAnalysis::analyze(func, impl->frame, impl->funcParamBitWidths);

    // Pass 2: Compute frame layout
    impl->frame.computeLayout();

    // Emit function label
    std::vector<LoweredInstruction> lowered;
    LoweredInstruction funcLabel;
    funcLabel.label = func.name;
    lowered.push_back(funcLabel);

    // Pass 3: Lower each basic block
    for (const auto& bb : func.basicBlocks) {
        impl->frame.resetTemp();  // reset temp allocator per block
        auto bbLowered = lowerBasicBlock(*bb);
        lowered.insert(lowered.end(), bbLowered.begin(), bbLowered.end());
    }

    return lowered;
}
```

**`lowerBasicBlock`** — remove `tempSpaceInBlock` cleanup:

```cpp
std::vector<LoweredInstruction> InstructionSelector::lowerBasicBlock(const ir::BasicBlock& bb) {
    // ... (emit label)

    // Lower each non-terminator instruction
    for (const auto& inst : bb.instructions) {
        auto instLowered = lowerInstruction(*inst);
        lowered.insert(lowered.end(), instLowered.begin(), instLowered.end());
    }

    // REMOVED: tempSpaceInBlock cleanup (now handled by StackFrame pre-allocation)

    // Lower terminator
    if (bb.terminator) {
        auto termLowered = lowerInstruction(*bb.terminator);
        lowered.insert(lowered.end(), termLowered.begin(), termLowered.end());
    }

    return lowered;
}
```

**Remove parameter setup code** (lines 266-399) — parameters are handled in FunctionAnalysis + Emitter.

### 7. `src/codegen/InstructionSelect.h`

Add `getFrameSize()` method:

```cpp
class InstructionSelector {
public:
    // ... existing methods ...
    int getFrameSize() const;  // NEW: returns frame size for prologue
};
```

### 8. `src/codegen/Emitter.h`

Update `emitFunction` signature:

```cpp
std::string emitFunction(const std::vector<LoweredInstruction>& instructions,
                          const std::string& funcName,
                          int frameSize);  // NEW parameter
```

### 9. `src/codegen/Emitter.cpp`

Modified prologue to use `frameSize`:

```cpp
std::string Emitter::emitFunction(const std::vector<LoweredInstruction>& instructions,
                                   const std::string& funcName,
                                   int frameSize) {
    std::string output;

    output += "global " + funcName + "\n";
    output += funcName + ":\n";

    // Emit prologue
    output += "  push bp\n";
    output += "  mov bp, sp\n";
    output += "  push bx\n";
    output += "  push si\n";
    output += "  push di\n";
    if (frameSize > 0) {
        output += "  sub sp, " + std::to_string(frameSize) + "\n";
    }

    // Emit instructions (no prologue param loading - done by StackFrame)
    for (const auto& lowered : instructions) {
        if (lowered.label == funcName) continue;
        output += emitLoweredInstruction(lowered);
    }

    return output;
}
```

### 10. `src/codegen/CodeGen.cpp`

Pass `frameSize` from selector to emitter:

```cpp
for (const auto& func : module.functions) {
    if (!func->isDeclaration) {
        auto lowered = selector.lowerFunction(*func);
        int frameSize = selector.getFrameSize();  // NEW
        allFunctions += emitter.emitFunction(lowered, func->name, frameSize);
        allFunctions += "\n";
    }
}
```

### 11. `src/codegen/ControlFlowOps.cpp`

**`lowerRetTerm`** — use `frame.getFrameSize()` for epilogue:

```cpp
std::vector<LoweredInstruction> lowerRetTerm(SelectorState& state, ...) {
    // Load return value (unchanged logic for value loading)
    // ...

    // Single add sp, frameSize (replaces the old allocaEndOffset computation)
    int frameSize = state.frame.getFrameSize();
    if (frameSize > 0) {
        Instruction286 addSp;
        addSp.mnemonic = "add";
        addSp.operands.push_back("sp");
        addSp.operands.push_back(std::to_string(frameSize));
        lowered.instructions.push_back(addSp);
    }

    // Pop callee-saved registers
    // ...
    emit "ret"
}
```

### 12. `src/codegen/MemoryOps.cpp`

**`lowerAlloca`** — no `sub sp`, just slot assignment:

```cpp
std::vector<LoweredInstruction> lowerAlloca(SelectorState& state, ...) {
    // No sub sp! Slot was pre-assigned during FunctionAnalysis
    // Just record the vreg -> slot mapping (already done in StackFrame)
    // No instructions to emit (alloca is just a stack offset assignment)
    return {};  // empty — alloca produces no code, just a stack offset
}
```

**`lowerLoad`/`lowerStore`** — use `frame.emitLoad32()` / `frame.emitStore32()` instead of inline bp-offset parsing.

### 13. `src/codegen/ArithmeticOps.cpp`

32-bit operations use `frame.allocTemp(4)` (pre-allocated, no `sub sp`) and `frame.emitStore32()` / `frame.emitLoad32()`:

```cpp
// Before (lowerAdd 32-bit):
state.currentStackOffset -= 4;
state.tempSpaceInBlock += 4;
emit sub sp, 4
emit mov [bp-offset], ax
emit mov [bp-offset+2], bx

// After:
std::string slot = state.frame.allocTemp(4, true);  // 4 bytes, 32-bit
state.frame.emitStore32(output, slot, "ax", "bx");  // centralized
```

### 14. `src/codegen/CallOps.cpp`

**32-bit arg detection fix**: Use `funcParamBitWidths[argIdx]` for each argument, not the return type.

**Argument pushing**: Use `frame.getBpOffset()` and `frame.classifyOperand()` instead of inline checks.

**Stack cleanup**: Computed from argument sizes (caller's responsibility).

### 15. `src/codegen/ConversionOps.cpp`

**`lowerPtrToInt`/`lowerIntToPtr`** — use `frame.allocTemp(4)` instead of manual `currentStackOffset -= 4` (fixes stack leak).

**`lowerSExt`/`lowerZExt`/`lowerBitCast`** — use `frame` helpers.

### 16. `src/codegen/BitwiseOps.cpp`

32-bit ops use `frame` helpers.

### 17. `src/codegen/FpuOps.cpp`

Use `frame` helpers.

### 18. `src/codegen/InstructionSelectUtils.cpp`

**Removed**: `ensure32BitStackSlot` (replaced by `StackFrame::allocTemp()`), `emit32BitStoreToStack` / `emit32BitLoadFromStack` (replaced by `StackFrame::emitStore32()` / `StackFrame::emitLoad32()`), `makeBpOffset` (replaced by `StackFrame::getHighBpOffset()`).

**Kept**: `isVreg`, `isConstantInt`, `isGlobalVar`, `toNasmGlobal` (not stack-related).

### 19. `CMakeLists.txt`

Add new source files:

```cmake
add_library(codegen
    src/codegen/CodeGen.cpp
    src/codegen/InstructionSelect.cpp
    src/codegen/InstructionSelectUtils.cpp
    src/codegen/StackFrame.cpp           # NEW
    src/codegen/FunctionAnalysis.cpp    # NEW
    src/codegen/ControlFlowOps.cpp
    src/codegen/ArithmeticOps.cpp
    src/codegen/BitwiseOps.cpp
    src/codegen/MemoryOps.cpp
    src/codegen/RegisterAlloc.cpp
    src/codegen/Emitter.cpp
    src/codegen/Addressing.cpp
    src/codegen/CallOps.cpp
    src/codegen/ConversionOps.cpp
    src/codegen/FpuOps.cpp
)
```

## How Each Bug Is Fixed

| Bug | Current | With StackFrame |
|-----|---------|-----------------|
| `alloc32BitStack` doesn't emit `sub sp` | Creates instruction but never emits it | No inline `sub sp` — slots pre-assigned by `StackFrame` |
| `lowerPtrToInt`/`lowerIntToPtr` leak 4 bytes | `currentStackOffset -= 4` with no cleanup tracking | `frame.allocTemp(4)` — pre-allocated, no leak possible |
| `tempSpaceInBlock` freed at block end | Values lost across blocks | Temp space pre-allocated as part of frame; `resetTemp()` just resets pointer |
| 32-bit arg detection uses return type | Falls back to `irInst.resultType->bitWidth` | Uses `funcParamBitWidths` per-arg, or `frame.is32bit(argName)` |
| Epilogue computation fragile | `totalLocals = -(allocaEndOffset + 6)` | `frame.getFrameSize()` — single value, pre-computed |
| bp-offset+2 parsing duplicated 30+ times | Inline in each handler | `frame.getHighBpOffset(vregName)` — centralized |
| `getPhysReg` returns "ax" for unknown vregs | Silent corruption | `frame.classifyOperand()` returns `Unknown` for unregistered vregs |
| 32-bit param copy to local stack | `sub sp, 4` + 2 `mov` per param | Params stay at caller-pushed `[bp+offset]`, vreg maps to positive offset |
| `lowerGEP` 32-bit result uses temp freed at block end | `tempSpaceInBlock += 4` | Cross-block vregs get spill slots; single-block use pre-allocated temp |

## Parameter Handling (Simplified)

Current (32-bit param): `sub sp, 4; mov ax, [bp+offset]; mov [bp-local], ax; mov dx, [bp+offset+2]; mov [bp-local+2], dx` — 5 instructions per param.

New (32-bit param): vreg maps directly to `[bp+offset]`. Zero instructions. The 32-bit load/store helpers use the positive offset directly.

Current (16-bit param, first 3): `mov bx, [bp+4]` — 1 instruction per param.

New: Same. No change for register-allocated params.

Current (16-bit param, 4+): vreg maps to `[bp+offset]`. Zero instructions.

New: Same. No change.

## 32-bit Value Handling (Centralized)

**Before** (duplicated in 30+ places):
```cpp
// Parse bp offset string, add 2, rebuild string — 8-10 lines each time
int offset = 0;
std::string offsetStr = physReg.substr(2);
if (!offsetStr.empty()) { try { offset = std::stoi(offsetStr); } catch(...) {} }
int newOffset = offset + 2;
std::string offsetStr2 = (newOffset >= 0) ? ("+" + std::to_string(newOffset)) : std::to_string(newOffset);
emit mov [bp+offset], lowReg
emit mov [bp+offset+2], highReg
```

**After** (centralized in StackFrame):
```cpp
frame.emitStore32(output, vregName, "ax", "dx");  // 2 instructions, handles offset internally
frame.emitLoad32(output, vregName, "ax", "dx");   // 2 instructions, handles offset internally
```

## Testing Strategy

1. **Build**: `cd build && cmake .. && make -j4`
2. **Run existing test suite**: `./tests/run_tests.sh` — all 18 tests (16 pass, 2 known failures) should still pass
3. **Compare generated assembly**: Before refactoring, save baseline `.asm` output for each test. After each phase, diff to ensure no regressions
4. **Specific regression tests for known bugs**:
   - `test_isalpha` — stack cleanup bug in complex functions (currently FAIL)
   - `test_mul_print` — stack cleanup bug after and operation (currently FAIL)
   - New test: 32-bit value surviving across basic blocks (temp space bug)
   - New test: `ptrtoint`/`inttoptr` followed by use in another block (stack leak bug)

5. **Expected improvements**: `test_isalpha` and `test_mul_print` should start PASSING if the stack handling bugs are fixed.

## Implementation Order

| Step | Task | Depends On |
|------|------|-----------|
| 1 | Create `StackFrame.h` / `StackFrame.cpp` with the class skeleton | — |
| 2 | Create `FunctionAnalysis.h` / `FunctionAnalysis.cpp` | 1 |
| 3 | Refactor `SelectorState` to contain `StackFrame` | 1 |
| 4 | Modify `lowerFunction` to call analysis + layout passes | 2, 3 |
| 5 | Modify `Emitter::emitFunction` to accept `frameSize` | 3 |
| 6 | Modify `CodeGen::generate` to pass `frameSize` | 4, 5 |
| 7 | Modify `lowerBasicBlock` — remove `tempSpaceInBlock` cleanup | 3 |
| 8 | Modify `lowerRetTerm` — use `frame.getFrameSize()` | 3 |
| 9 | Modify `lowerAlloca` — no `sub sp`, just slot assignment | 3 |
| 10 | Modify `lowerLoad`/`lowerStore` — use `StackFrame` helpers | 3 |
| 11 | Modify `lowerAdd`/`lowerSub`/`lowerMul`/`lowerDivRem` — use `StackFrame` | 3 |
| 12 | Modify `lowerCall` — fix 32-bit arg detection, use `StackFrame` | 3 |
| 13 | Modify `lowerPtrToInt`/`lowerIntToPtr` — fix stack leak | 3 |
| 14 | Modify `lowerSExt`/`lowerZExt`/`lowerBitCast` — use `StackFrame` | 3 |
| 15 | Modify `lowerGEP` — use `StackFrame` for 32-bit results | 3 |
| 16 | Modify `lowerICmp`/`lowerSelect`/`lowerCondBrTerm` — use `StackFrame` | 3 |
| 17 | Modify FPU ops — use `StackFrame` | 3 |
| 18 | Update `CMakeLists.txt` — add new source files | 1, 2 |
| 19 | Build and run test suite | 17 |
| 20 | Add new regression tests for cross-block temp space | 19 |

Steps 7-17 can be done in any order (they're independent once `StackFrame` exists), but it's best to do them in this order: prologue/epilogue first (steps 7-8), then memory ops (9-10), then arithmetic (11), then call (12), then conversions (13-14), then the rest.

## Key Design Decisions

1. **32-bit params stay at caller-pushed location**: No copy to local stack. The vreg maps to a positive bp offset. This eliminates ~5 instructions per 32-bit param and simplifies the prologue.

2. **Temp space is part of the frame, not freed per-block**: `resetTemp()` just resets the offset counter. The physical stack space is pre-allocated in the prologue. This fixes the cross-block value loss bug.

3. **Cross-block vregs get spill slots**: The liveness analysis determines which vregs survive across blocks. These get fixed slots (like allocas). Single-block vregs use temp space (reset per block, but physically pre-allocated).

4. **No inline `sub sp` / `add sp` in instruction handlers**: All stack space is pre-allocated in the prologue. The only `add sp` is in the epilogue (`add sp, frameSize`). The only `sub sp` is in the prologue.

5. **Centralized 32-bit helpers**: `StackFrame::emitLoad32()` and `emitStore32()` replace the 30+ duplicated bp-offset+2 patterns. Single source of truth for 32-bit load/store.

6. **Centralized operand classification**: `StackFrame::classifyOperand()` replaces the ad-hoc "is it a constant? is it a vreg? is it a global? is it memory?" checks scattered through handlers.

## Expected Outcomes

1. **Eliminated stack leaks**: All stack allocations are tracked and recovered in a single, pre-computed frame size
2. **Cross-block value preservation**: Liveness analysis ensures values survive across basic blocks
3. **Reduced code duplication**: 30+ bp-offset patterns replaced with centralized helpers
4. **Simplified prologue/epilogue**: Single `sub sp` / `add sp` instead of scattered inline adjustments
5. **Fixed known test failures**: `test_isalpha` and `test_mul_print` expected to pass
6. **Better maintainability**: Central stack management makes future changes easier to reason about

## Date

2026-07-08
