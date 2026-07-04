# Refactoring Plan: InstructionSelect.cpp

## Current State Analysis

### File: `src/codegen/InstructionSelect.cpp` (2899 lines)

The file contains a single monolithic `lowerInstruction()` method with a ~2453-line
switch statement, plus a 265-line `Impl` struct, and `lowerFunction()` (138 lines).

### Structure Breakdown

| Section | Lines | LOC | Description |
|---|---|---|---|
| `Impl` struct | 15-265 | ~250 | Register allocation state + helper methods |
| Constructor/destructor | 267-273 | 7 | trivial |
| `lowerInstruction()` switch | 275-2728 | ~2453 | All opcode handlers |
| `lowerBasicBlock()` | 2734-2756 | 23 | trivial |
| `lowerFunction()` | 2759-2896 | 138 | Prologue, parameter setup |

### Opcode Handler Inventory (within `lowerInstruction()`)

| Category | Opcodes | Lines | Complexity |
|---|---|---|---|
| Arithmetic (32-bit + 16-bit) | Add, Sub, Mul, SDiv, UDiv, SRem, URem | ~740 | Very High |
| Bitwise | And, Or, Xor, Shl, LShr, AShr | ~175 | Low (repetitive) |
| Memory | Alloca, Load, Store, GetElementPtr | ~435 | High |
| Control Flow | BrTerm, CondBrTerm, RetTerm, Switch, Phi, Unreachable | ~250 | Medium |
| Comparison | ICmp, Select | ~200 | Medium |
| Call | Call | ~226 | Very High |
| Conversion | Trunc, ZExt, SExt, BitCast, PtrToInt, IntToPtr, Freeze | ~140 | Low |
| Aggregate | ExtractValue, InsertValue | ~55 | Low |
| FPU | FAdd, FSub, FMul, FDiv, FRem, FCmp, FPTrunc, FPExt, FPToUI, FPToSI, UIToFP, SIToFP | ~160 | Medium |

### Duplicated Code Patterns (DRY violations)

1. **bp-offset+2 high-word addressing** (~15 occurrences, ~10 lines each):
   The pattern of parsing a `bp-NN` string, adding 2, and rebuilding the address string.

2. **Operand classification** (~10 occurrences, ~5 lines each):
   Checking if an operand is a constant, vreg, or memory location.

3. **32-bit load/store sequences** (~8 occurrences, ~20 lines each):
   Loading/storing low word + high word from/to stack with offset calculation.

4. **Conditional jump label pattern** (~4 occurrences):
   The "push lowered, create label, push, create end label" sequence for ICmp, CondBr, Select, Switch.

5. **Operand-to-register loading** (~12 occurrences):
   The "check if memory, load to AX; check if constant, mov immediate; else use register" sequence.

---

## Refactoring Plan

### Phase 1: Create Internal Header (`InstructionSelectInternal.h`)

**Goal:** Expose the `Impl` struct and shared utilities to multiple translation units
without changing the public API.

**New file:** `src/codegen/InstructionSelectInternal.h`

Contents:
- Move the `Impl` struct definition here as a standalone struct named
  `SelectorState` (not nested inside `InstructionSelector`)
- Include all state members: `vregToPhys`, `vregToStackOffset`, `physToVreg`,
  `is32bit`, `allocaVregs`, `funcParamBitWidths`, `labelCounter`, `currentFunc`,
  `currentStackOffset`, `tempRegs`
- Include all helper methods: `is32BitReg`, `mark32Bit`, `getPhysReg`,
  `getFreeTempReg`, `assignReg`, `updateResultReg`, `freeReg`, `nextLabel`,
  `allocateStack`, `load32Bit`, `store32Bit`, `alloc32BitStack`, `isConstant`

**New shared utility functions** (extracted from duplicated patterns):

```cpp
// Operand classification
enum class OperandKind { Constant, Register, Memory, Global, Unknown };
OperandKind classifyOperand(const std::string& name, const SelectorState& state);

// bp-relative offset string builder
// Given "bp-10" and offset=2, returns "bp-8"
std::string makeBpOffset(const std::string& bpReg, int additionalOffset);

// Check if a register string is a memory location (contains "bp")
bool isMemoryOperand(const std::string& reg);

// Check if a name is a global variable (starts with '.')
bool isGlobalVar(const std::string& name);
std::string toNasmGlobal(const std::string& name);  // ".foo" -> "_foo"

// 32-bit store from AX:BX (or AX:DX) to a stack location
void emit32BitStoreToStack(std::vector<Instruction286>& output,
                            const std::string& stackReg,
                            const std::string& lowReg = "ax",
                            const std::string& highReg = "bx");

// 32-bit load from stack into AX:BX (or AX:DX)
void emit32BitLoadFromStack(std::vector<Instruction286>& output,
                            const std::string& stackReg,
                            const std::string& lowReg = "ax",
                            const std::string& highReg = "bx");

// Allocate 4 bytes on stack for a 32-bit result, return bp-relative string
// Also emits "sub sp, 4" and updates vregToStackOffset
std::string ensure32BitStackSlot(SelectorState& state,
                                  std::vector<Instruction286>& output,
                                  const std::string& vregName);

// Load operand value into AX (handles constant, register, memory cases)
void loadOperandToReg(std::vector<Instruction286>& output,
                      const std::string& destReg,
                      const std::string& opName,
                      const SelectorState& state);

// Check if operand is a vreg
bool isVreg(const std::string& name, const SelectorState& state);

// Check if operand is a constant integer
bool isConstantInt(const std::string& name);
```

**Changes to `InstructionSelect.cpp`:**
- `#include "InstructionSelectInternal.h"`
- `struct InstructionSelector::Impl : SelectorState {};` (thin wrapper, inherits all)
- Constructor/destructor unchanged

**No changes to `InstructionSelect.h`** - the public API stays identical.

**Verification:** Build must succeed. Generated assembly must be byte-identical.

---

### Phase 2: Extract Opcode Handlers into Separate .cpp Files

Each handler becomes a free function with signature:
```cpp
std::vector<LoweredInstruction> lower<OpcodeName>(
    SelectorState& state,
    const ir::Instruction& irInst,
    const std::string& resultReg
);
```

The `resultReg` parameter is the value computed at the top of the current
`lowerInstruction()` (line 280-289). Passing it avoids recomputation.

#### 2a. `ArithmeticOps.cpp` (~740 lines extracted)
Handlers: `lowerAdd`, `lowerSub`, `lowerMul`, `lowerDivRem` (shared by SDiv/UDiv/SRem/URem)

This is the most complex file. The 32-bit paths use the shared 32-bit load/store
utilities from Phase 1 to eliminate the duplicated bp-offset code.

Key refactoring within this file:
- Extract the 32-bit arithmetic helper (shared load-operands-into-AX:BX / CX:DX pattern)
- The 32-bit Add, Sub, Mul, and Div all follow the same pattern: load operands,
  perform operation, store result to stack. This can be partially shared.
- Mul and Div call runtime helpers (`_MultiplyI32`, `_DivideI32`/`_DivideU32`)

#### 2b. `BitwiseOps.cpp` (~175 lines extracted)
Handlers: `lowerAnd`, `lowerOr`, `lowerXor`, `lowerShl`, `lowerLshr`, `lowerAShr`

These six handlers are nearly identical (move op1 to resultReg, apply binary op with op2).
A shared template/helper can reduce them further:
```cpp
// Shared pattern for binary ops: resultReg = op1 <op> op2
std::vector<LoweredInstruction> lowerBinaryOp(
    SelectorState& state, const ir::Instruction& irInst,
    const std::string& resultReg, const std::string& mnemonic);
```
And/Or/Xor call `lowerBinaryOp(..., "and"/"or"/"xor")`,
Shl/LShr/AShr call `lowerBinaryOp(..., "shl"/"shr"/"sar")` with minor differences
(shift count in CL vs immediate).

#### 2c. `MemoryOps.cpp` (~435 lines extracted)
Handlers: `lowerAlloca`, `lowerLoad`, `lowerStore`, `lowerGetElementPtr`

- `lowerLoad` and `lowerStore` have 16-bit and 32-bit paths, use shared 32-bit utilities.
- `lowerAlloca` is simple (30 lines).
- `lowerGetElementPtr` handles pointer arithmetic with element size calculation.

#### 2d. `ControlFlowOps.cpp` (~450 lines extracted)
Handlers: `lowerRetTerm`, `lowerBrTerm`, `lowerCondBrTerm`, `lowerSwitch`,
`lowerPhi`, `lowerUnreachable`, `lowerSelect`, `lowerICmp`

- `lowerRetTerm` includes function epilogue (stack cleanup, register restoration)
- `lowerICmp` uses the conditional-jump-label pattern (shared utility candidate)
- `lowerSelect` uses a similar label pattern

#### 2e. `ConversionOps.cpp` (~200 lines extracted)
Handlers: `lowerTrunc`, `lowerZExt`, `lowerSExt`, `lowerBitCast` (shared by
BitCast/PtrToInt/IntToPtr), `lowerFreeze`, `lowerExtractValue`,
`lowerInsertValue`

- Trunc, ZExt, SExt, BitCast, Freeze are nearly identical (move value, update result).
  Candidate for a shared helper.

#### 2f. `FpuOps.cpp` (~160 lines extracted)
Handlers: `lowerFAdd`, `lowerFSub`, `lowerFMul`, `lowerFDiv` (shared),
`lowerFRem`, `lowerFCmp`, `lowerFPTrunc`, `lowerFPExt` (shared),
`lowerFPToUI`, `lowerFPToSI` (shared), `lowerUIToFP`, `lowerSIToFP` (shared)

- FAdd/FSub/FMul/FDiv differ only in mnemonic (`fadd`/`fsub`/`fmul`/`fdiv`)
- FPTrunc/FPExt differ only in operand size (`dword`/`qword`)
- FPToUI/FPToSI are identical (`fistp`)
- UIToFP/SIToFP differ only in mnemonic (`fild` is same for both)

#### 2g. `CallOps.cpp` (~226 lines extracted)
Handler: `lowerCall`

The Call handler is complex (226 lines) due to:
- 16-bit vs 32-bit argument handling
- Global variable argument handling
- Constant argument handling
- Stack cleanup after call
- 32-bit argument splitting into high/low words

This file can use the shared 32-bit utilities and operand classification helpers.

---

### Phase 3: Refactor `lowerInstruction()` to Dispatcher

**File:** `src/codegen/InstructionSelect.cpp` (will shrink from ~2899 to ~150 lines)

The new `lowerInstruction()` becomes a thin dispatcher:

```cpp
std::vector<LoweredInstruction> InstructionSelector::lowerInstruction(
    const ir::Instruction& irInst) {

    // Compute resultReg (shared across all handlers)
    std::string resultReg;
    if (!irInst.resultName.empty()) {
        resultReg = impl->getPhysReg(irInst.resultName);
    }

    switch (irInst.opcode) {
        // Arithmetic
        case ir::Opcode::Add:      return lowerAdd(*impl, irInst, resultReg);
        case ir::Opcode::Sub:      return lowerSub(*impl, irInst, resultReg);
        case ir::Opcode::Mul:     return lowerMul(*impl, irInst, resultReg);
        case ir::Opcode::SDiv:
        case ir::Opcode::UDiv:
        case ir::Opcode::SRem:
        case ir::Opcode::URem:    return lowerDivRem(*impl, irInst, resultReg);

        // Bitwise
        case ir::Opcode::And:     return lowerAnd(*impl, irInst, resultReg);
        case ir::Opcode::Or:      return lowerOr(*impl, irInst, resultReg);
        case ir::Opcode::Xor:     return lowerXor(*impl, irInst, resultReg);
        case ir::Opcode::Shl:    return lowerShl(*impl, irInst, resultReg);
        case ir::Opcode::LShr:   return lowerLShr(*impl, irInst, resultReg);
        case ir::Opcode::AShr:   return lowerAShr(*impl, irInst, resultReg);

        // Memory
        case ir::Opcode::Alloca:  return lowerAlloca(*impl, irInst, resultReg);
        case ir::Opcode::Load:   return lowerLoad(*impl, irInst, resultReg);
        case ir::Opcode::Store:  return lowerStore(*impl, irInst, resultReg);
        case ir::Opcode::GetElementPtr: return lowerGetElementPtr(*impl, irInst, resultReg);

        // Control flow
        case ir::Opcode::RetTerm:   return lowerRetTerm(*impl, irInst, resultReg);
        case ir::Opcode::BrTerm:    return lowerBrTerm(*impl, irInst, resultReg);
        case ir::Opcode::CondBrTerm: return lowerCondBrTerm(*impl, irInst, resultReg);
        case ir::Opcode::Switch:   return lowerSwitch(*impl, irInst, resultReg);
        case ir::Opcode::Phi:      return lowerPhi(*impl, irInst, resultReg);
        case ir::Opcode::Unreachable: return lowerUnreachable(*impl, irInst, resultReg);
        case ir::Opcode::Select:  return lowerSelect(*impl, irInst, resultReg);
        case ir::Opcode::ICmp:     return lowerICmp(*impl, irInst, resultReg);

        // Call
        case ir::Opcode::Call:    return lowerCall(*impl, irInst, resultReg);

        // Conversions
        case ir::Opcode::Trunc:   return lowerTrunc(*impl, irInst, resultReg);
        case ir::Opcode::ZExt:    return lowerZExt(*impl, irInst, resultReg);
        case ir::Opcode::SExt:    return lowerSExt(*impl, irInst, resultReg);
        case ir::Opcode::BitCast:
        case ir::Opcode::PtrToInt:
        case ir::Opcode::IntToPtr: return lowerBitCast(*impl, irInst, resultReg);
        case ir::Opcode::Freeze:   return lowerFreeze(*impl, irInst, resultReg);
        case ir::Opcode::ExtractValue: return lowerExtractValue(*impl, irInst, resultReg);
        case ir::Opcode::InsertValue: return lowerInsertValue(*impl, irInst, resultReg);

        // FPU
        case ir::Opcode::FAdd:
        case ir::Opcode::FSub:
        case ir::Opcode::FMul:
        case ir::Opcode::FDiv:    return lowerFpuArith(*impl, irInst, resultReg);
        case ir::Opcode::FRem:    return lowerFRem(*impl, irInst, resultReg);
        case ir::Opcode::FCmp:    return lowerFCmp(*impl, irInst, resultReg);
        case ir::Opcode::FPTrunc:
        case ir::Opcode::FPExt:   return lowerFpuConvert(*impl, irInst, resultReg);
        case ir::Opcode::FPToUI:
        case ir::Opcode::FPToSI:  return lowerFPToInt(*impl, irInst, resultReg);
        case ir::Opcode::UIToFP:
        case ir::Opcode::SIToFP:  return lowerIntToFP(*impl, irInst, resultReg);

        default:
            return {LoweredInstruction{}};  // NOP with TODO comment
    }
}
```

`lowerBasicBlock()` and `lowerFunction()` stay in `InstructionSelect.cpp`.

---

### Phase 4: Update CMakeLists.txt

Add the new source files to the `codegen` library:

```cmake
add_library(codegen
    src/codegen/CodeGen.cpp
    src/codegen/InstructionSelect.cpp          # dispatcher + lowerFunction
    src/codegen/InstructionSelectUtils.cpp     # shared utilities
    src/codegen/ArithmeticOps.cpp
    src/codegen/BitwiseOps.cpp
    src/codegen/MemoryOps.cpp
    src/codegen/ControlFlowOps.cpp
    src/codegen/ConversionOps.cpp
    src/codegen/FpuOps.cpp
    src/codegen/CallOps.cpp
    src/codegen/RegisterAlloc.cpp
    src/codegen/Emitter.cpp
    src/codegen/Addressing.cpp
)
```

---

### Phase 5: Build and Verify

After each phase:
1. Build: `cd build && make -j4`
2. Generate assembly for all test files and diff against baseline:
   ```bash
   for f in tests/integration/*.ll tests/os2/*.c; do
       # generate .asm, diff against baseline
   done
   ```
3. Run integration tests: `ctest` or `tests/run_tests.sh`
4. Verify generated assembly is **byte-identical** to pre-refactoring output

---

## File Summary After Refactoring

| File | Lines (est.) | Contents |
|---|---|---|
| `InstructionSelect.h` | 56 (unchanged) | Public API |
| `InstructionSelectInternal.h` | ~120 | `SelectorState` struct + utility declarations |
| `InstructionSelectUtils.cpp` | ~150 | Shared utility implementations |
| `InstructionSelect.cpp` | ~150 | Constructor, dispatcher, `lowerFunction()`, `lowerBasicBlock()` |
| `ArithmeticOps.cpp` | ~500 | Add, Sub, Mul, Div/Rem |
| `BitwiseOps.cpp` | ~80 | And, Or, Xor, Shl, LShr, AShr |
| `MemoryOps.cpp` | ~350 | Alloca, Load, Store, GEP |
| `ControlFlowOps.cpp` | ~350 | Ret, Br, CondBr, Switch, Phi, Unreachable, Select, ICmp |
| `ConversionOps.cpp` | ~150 | Trunc, ZExt, SExt, BitCast, Freeze, ExtractValue, InsertValue |
| `FpuOps.cpp` | ~120 | FAdd, FSub, FMul, FDiv, FRem, FCmp, FPTrunc, FPExt, FPToUI, FPToSI, UIToFP, SIToFP |
| `CallOps.cpp` | ~200 | Call |

**Total: ~2200 lines** (down from 2899, with duplication eliminated)

Largest file drops from **2899 lines to ~500 lines** (ArithmeticOps.cpp).

---

## Risk Assessment and Mitigation

### Risk 1: Behavior change during extraction
**Mitigation:** Extract handlers one file at a time. After each extraction, generate
assembly for all test cases and `diff` against baseline. Any difference indicates a
behavior change that must be fixed before proceeding.

### Risk 2: Shared state in `SelectorState` / `Impl`
**Mitigation:** The `SelectorState` struct is passed by reference to each handler.
The state is per-function (reset in `lowerFunction()`), so there are no cross-function
leaks. The pImpl pattern is preserved.

### Risk 3: `resultReg` computation differences
**Mitigation:** `resultReg` is computed once in the dispatcher and passed to all
handlers. The computation is:
```cpp
std::string resultReg;
if (!irInst.resultName.empty()) {
    resultReg = impl->getPhysReg(irInst.resultName);
}
```
This is identical to the current code (lines 280-289). The `if (resultReg == "ax")`
branch is a no-op comment, so it can be dropped.

### Risk 4: Implicit fall-through in switch cases
**Mitigation:** Several opcode cases share handlers (e.g., SDiv/UDiv/SRem/URem,
BitCast/PtrToInt/IntToPtr). The dispatcher explicitly maps each opcode to its handler
function. The handler checks `irInst.opcode` internally where needed (e.g., the Div
handler checks `irInst.opcode == ir::Opcode::SDiv` to choose `idiv` vs `div`).

### Risk 5: `loweredVec` / `lowered` local variables
**Mitigation:** Each handler returns `std::vector<LoweredInstruction>`. The
dispatcher returns this directly. The pattern of pushing `lowered` into `loweredVec`
and returning `loweredVec` is replaced by each handler building and returning its own
vector. The ICmp, CondBr, Select, and Switch handlers that create label
`LoweredInstruction` entries will push those into their return vector.

---

## Execution Order

1. **Phase 1a:** Create `InstructionSelectInternal.h` with `SelectorState` struct
2. **Phase 1b:** Create `InstructionSelectUtils.cpp` with shared utilities
3. **Phase 1c:** Update `InstructionSelect.cpp` to use `SelectorState` via inheritance
4. **Phase 1d:** Build + verify assembly output unchanged
5. **Phase 2a-g:** Extract each handler group (one at a time, build + verify after each)
6. **Phase 3:** Rewrite `lowerInstruction()` as dispatcher
7. **Phase 4:** Update CMakeLists.txt (add new .cpp files)
8. **Phase 5:** Full build + test run

Each phase is independently verifiable: generate assembly from test .ll files and
diff against the baseline. The refactoring is complete when all tests pass and assembly
output is byte-identical.

---

## Notes

- The `lowerFunction()` method (lines 2759-2896) also accesses `impl->` members
  extensively for parameter setup. It stays in `InstructionSelect.cpp` since it needs
  access to `SelectorState` and is tightly coupled to the function-level state reset.
- The `Impl` struct's `load32Bit` and `store32Bit` methods are currently unused by
  most handlers (they were apparently intended for the 32-bit arithmetic ops but the
  handlers inline their own load/store code). These can be cleaned up or replaced
  with the new shared utilities.
- The `alloc32BitStack` helper in `Impl` is used only by the Div handler. It returns
  a stack string but also has a side effect of emitting a `sub sp, 4` instruction into
  a local `Instruction286` that is never actually added to the output (this looks like
  a bug - the `subSp` instruction is created but not pushed). This should be investigated
  during the refactoring.