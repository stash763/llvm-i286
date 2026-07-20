# Codegen Refactoring Plan

## Problem Statement

The llvm-i286 code generator has produced persistent, difficult-to-fix bugs due to structural design issues that increase the probability of errors. This document outlines a phased refactoring approach to address the root causes.

### Current State

- ~9,000 lines of C++ across 16 codegen files and 3 IR files
- c-testsuite: 148/220 pass (67%)
- test_hello.exe segfaults due to prologue/epilogue frame size mismatch
- ~50 commits of incremental bug fixes, many addressing symptoms rather than root causes

### Root Causes (Detailed)

#### RC1: String-Based Operand Representation

**Problem:** The IR AST stores operands as raw strings (`Operand::name`). Every handler must classify operands by inspecting string prefixes, suffixes, and attempting numeric parsing.

**Affected files:** All handler files (MemoryOps, CallOps, ArithmeticOps, ControlFlowOps, ConversionOps, BitwiseOps)

**Evidence:**
- `name[0] == '@'` (global detection): 8+ locations across 4 files
- `name[0] == '%'` (vreg detection): 6+ locations
- `std::stoi(name)` in try/catch (constant detection): ~30 locations
- `name == "null"` / `name == "false"` / `name == "true"`: 6+ locations
- `name.substr(0, 13) == "getelementptr"` (GEP expression): 3+ locations
- `name.substr(0, 8) == "ptrtoint"` (ptrtoint expression): 2+ locations
- `@.str` → `_str` name conversion: 12+ locations (MemoryOps: 4, CallOps: 3, ControlFlowOps: 1, CodeGen: 4)

**Bug pattern:** Handler A treats `"null"` as constant 0; Handler B doesn't check for `"null"` and tries `std::stoi("null")` which throws, falling through to the wrong code path. Handler C checks for `@` prefix; Handler D checks for `.` prefix; they disagree on what counts as "global."

**Impact:** Generates inconsistencies between handlers. Each new instruction type requires duplicating all classification logic. Fixes applied in one handler don't propagate to others.

#### RC2: String-Based Location Representation

**Problem:** `StackFrame::getPhysReg()` returns a string that could be a register name (`"ax"`, `"bx"`), a bp-relative offset (`"bp+6"`, `"bp-8"`), or the fallback `"ax"`. Every handler must parse these strings to extract offsets and compute high-word locations.

**Evidence:**
- `find("bp") != std::string::npos` (stack location detection): 90 occurrences across handlers
  - MemoryOps.cpp: 31
  - ArithmeticOps.cpp: 22
  - ConversionOps.cpp: 13
  - BitwiseOps.cpp: 10
  - ControlFlowOps.cpp: 8
  - CallOps.cpp: 6
  - StackFrame.cpp: 5
- High-word offset computation (`substr(2)` → `stoi()` → `offset + 2` → reconstruct string): 16 occurrences
  - ArithmeticOps.cpp: 8
  - MemoryOps.cpp: 5
  - ConversionOps.cpp: 2
  - StackFrame.cpp: 1
- `getPhysReg()` silently returns `"ax"` as fallback for unknown vregs (StackFrame.cpp:350) — masks bugs

**Bug pattern:** A handler gets `"bp-8"` from `getPhysReg()`, parses it with `substr(2)` to get `"-8"`, computes `offset + 2 = -6`, constructs `"bp-6"`. But if the string was `"bx"` (register), `substr(2)` gives `""`, `stoi("")` throws, handler falls through to wrong code. The 16 copies of this logic each handle edge cases differently.

#### RC3: No Real Register Allocation

**Problem:** `RegisterAllocator` (RegisterAlloc.cpp, 76 lines) is a stub — `allocate()` returns instructions unchanged with `TODO: Implement linear scan`. Actual "register allocation" is the ad-hoc `vregToPhys_` map in `StackFrame`, which maps each vreg to a single register or stack location with no liveness tracking.

**Evidence:**
- `setPhysReg(vreg, "ax")` called 11 times across handlers — each call overwrites the previous vreg mapped to AX
- `setPhysReg()` clears the old occupant (StackFrame.cpp:353-371), so the previous vreg loses its mapping and silently falls back to `"ax"` on next lookup — two vregs now both think they're in AX
- No interference tracking, no spilling policy, no register reuse
- `getFreeTempReg()` checks `physToVreg_` but is never called by any handler

**Bug pattern:** Instruction 1 produces `%5`, maps it to AX. Instruction 2 produces `%6`, maps it to AX — `%5` is now evicted. Instruction 3 uses `%5`, calls `getPhysReg("%5")`, gets `"ax"` (fallback), but AX now holds `%6`'s value. Silent data corruption.

#### RC4: Frame Size Computation Fragility

**Problem:** `totalFrameSize` is computed once in `computeLayout()` and never updated. During lowering, `allocResultSlot()`, `allocTemp()`, and `allocStack()` can allocate space beyond the pre-estimated amount, creating a mismatch between the prologue (`sub sp, frameSize`) and epilogue (`add sp, frameSize`).

**Evidence:**
- `allocResultSlot()` called 25 times across handlers — each can advance `tempCurrent` beyond the pre-allocated `tempSize` without bounds checking
- `allocStack()` (StackFrame.cpp:512) modifies `nextLocalOffset` and adds slots, but doesn't update `localsSize` or `totalFrameSize`
- `FunctionAnalysis::computeMaxTempSpace()` estimates temp space by counting instructions with results — a heuristic that misses cases like `Load` needing extra 4 bytes for pointer save (line 172), or GEP needing temp registers
- `computeLayout()` is called once (InstructionSelect.cpp:325) before lowering; `getFrameSize()` is read by both prologue (Emitter.cpp:111) and epilogue (ControlFlowOps.cpp:113)

**Bug pattern:** Analysis estimates 8 bytes of temp space. During lowering, a block actually needs 12 bytes. Prologue allocates 8 bytes (`sub sp, 8`), but lowering writes 12 bytes of temp data, overwriting saved BP or return address. Epilogue deallocates only 8 (`add sp, 8`), leaving 4 bytes of garbage on stack, corrupting `pop bp` / `retf` sequence. This is the test_hello segfault.

#### RC5: Massive Code Duplication

**Problem:** The same 32-bit load/store/classification patterns are duplicated across every handler.

**Evidence:**
- High-word offset computation: 16 copies
- Constant detection (`try { stoi(); } catch(...) {}`): ~30 copies
- `@.str` → `_str` global name conversion: 12+ copies
- 32-bit load sequence (load low to AX, load high to DX/BX, with register/stack distinction): ~10 copies
- The "check if global, check if GEP, check if ptrtoint" cascade in CallOps.cpp (lines 482-555) is 73 lines of string parsing partially duplicated in MemoryOps

**Bug pattern:** A fix to high-word offset computation is applied in ArithmeticOps but not MemoryOps. The `@.str` naming fix works in CallOps but not ControlFlowOps. Each handler has its own variations that drift over time.

#### RC6: PHI Elimination as Post-Hoc Patching

**Problem:** PHI nodes are handled with a separate pass in `lowerBasicBlock()` (InstructionSelect.cpp:148-296) — ~150 lines of duplicated value classification logic, outside the instruction handler framework.

**Evidence:**
- PHI store logic (lines 206-278) reimplements constant detection (`null`, `false`, `true`, `undef`, FP constants, complex expressions) — same logic in every handler
- `setPhysReg(phiResult, "ax")` at line 174 overwrites whatever was previously in AX
- Unmatched-label mapping (lines 359-370) is a workaround for parser not tracking entry block labels
- `lowerPhi()` exists but returns empty — real work is in `lowerBasicBlock`

**Bug pattern:** PHI result mapped to AX. First instruction of block also produces result mapped to AX. PHI result is lost. Or: PHI incoming value is a GEP expression, but PHI store logic doesn't handle GEP (only handles `%vreg`, `@global`, constants), so wrong value is stored.

#### RC7: Unused and Incomplete Abstractions

**Problem:** Several abstractions were designed but never adopted:
- `Addressing` class with `MemoryOperand` struct (typed segment, baseReg, indexReg, displacement) — completely unused
- `RegisterAllocator` class — stub with `TODO`
- `MemoryOperand` struct — never constructed by any handler

Handlers construct operands as raw strings: `"[bp+6]"`, `"[es:bx]"`, `"[" + globalPtrName + "]"`.

**Evidence:**
- `grep -rn "MemoryOperand\|Addressing" src/codegen/*.cpp` (excluding definitions) returns zero results
- `RegisterAllocator::allocate()` returns instructions unchanged
- Handlers build memory operand strings by concatenation

**Bug pattern:** A handler constructs `"[bp-6]"` for a store destination, but the string doesn't carry information about segment override, register saving, or offset correctness — it's just text.

---

## Phased Refactoring Plan

### Overview

| Phase | Root Cause | Impact | Effort | Dependencies | Priority |
|-------|-----------|--------|--------|-------------|----------|
| 1 | RC1: Typed Operands | Eliminates ~50% of bugs | High | None | First |
| 2 | RC2: Typed Locations | Eliminates ~20% of bugs | Medium | Phase 1 | Second |
| 3 | RC5: ValueMaterializer | Eliminates duplication | Medium | Phases 1, 2 | Third |
| 4 | RC4: Frame Size Fix | Fixes segfault | Low-Med | None | Parallel (immediate) |
| 5 | RC3: Register Alloc | Eliminates AX overwrites | High | Phases 1, 2 | Fourth |
| 6 | RC6: PHI Integration | Cleanup | Low-Med | Phases 1, 3 | Fifth |
| 7 | RC7: Emitter Fix | Cleanup | Low | None | Parallel (immediate) |

**Immediate wins:** Phase 4 (frame size fix) and Phase 7 (emitter fix) can be done independently and likely fix the test_hello segfault.

**Phase 4 status: IMPLEMENTED.** `extendTempArea()` dynamically updates `totalFrameSize`; epilogue uses `lea sp, [bp-6]` instead of `add sp, frameSize` to avoid early-return block mismatch. Frame size mismatch resolved (confirmed via td2ine trace). test_hello still loops in musl `exit()` — see "exit() infinite loop" risk below. Likely a codegen bug in `a_cas` inline function compilation, to be fixed by Phases 1-3.

**Strategic investment:** Phase 1 (typed operands) is the highest-effort but highest-impact change. Once done, Phases 2-3 follow naturally and dramatically reduce handler file sizes.

---

### Phase 1: Typed Operand Representation

**Goal:** Replace string-based operands with typed `OperandRef` objects that carry their kind, type, and value.

**Root cause addressed:** RC1

#### Design

```cpp
// New: src/ir/OperandRef.h
class OperandRef {
public:
    enum class Kind {
        Constant,      // Integer/FP constant
        Vreg,          // %name - SSA virtual register
        Global,        // @name - global variable/function
        GEPExpr,       // getelementptr(...) constant expression
        PtrToIntExpr,  // ptrtoint(...) constant expression
        Null,          // null pointer constant
        Undef,         // undefined value
        BoolConst,     // true/false
    };

    Kind kind = Kind::Constant;
    std::string name;       // Raw name (vreg name, global name, or expression text)
    int64_t constValue = 0; // For Constant
    TypeRef type;           // Carries bitWidth, pointer-ness, etc.

    // For GEP/PtrToInt expressions (computed once during parsing):
    std::string baseGlobal; // The @name inside the expression
    int64_t gepOffset = 0;  // Computed byte offset

    // Classification helpers
    bool isConstant() const { return kind == Kind::Constant; }
    bool isVreg() const { return kind == Kind::Vreg; }
    bool isGlobal() const { return kind == Kind::Global || kind == Kind::GEPExpr || kind == Kind::PtrToIntExpr; }
    bool isNull() const { return kind == Kind::Null; }
    bool is32bit() const { return type.bitWidth == 32; }

    // Convert global name to NASM format (single source of truth)
    std::string nasmName() const;
};
```

#### Changes

1. **New file:** `src/ir/OperandRef.h` — defines `OperandRef` and `TypeRef`
2. **Modify:** `src/ir/IrAst.h` — `Operand::name` (string) → `Operand::ref` (OperandRef); `Instruction::callArgs` (vector<string>) → `vector<OperandRef>`
3. **Modify:** `src/ir/IrVisitor.cpp` — populate `OperandRef` during parsing instead of raw strings; classify each operand once; call `computeGEPByteOffset` once during parsing
4. **Modify:** All handler files — receive pre-classified operands; remove all `name[0] == '@'` checks, `stoi` calls for classification, GEP string parsing, `@.str` → `_str` conversions
5. **New file:** `src/ir/OperandClassifier.h/.cpp` — single source of truth for operand classification, used by IrVisitor

#### Migration Strategy

To avoid a big-bang rewrite, introduce `OperandRef` alongside existing strings:

1. Add `OperandRef Operand::ref` alongside existing `std::string Operand::name`
2. Populate both in `IrVisitor` — `ref` is the new typed version, `name` is the legacy string
3. Migrate handlers one at a time: each handler switches from `name` to `ref`
4. After all handlers migrated, remove `name` field

#### Test Verification

After each handler migration:
- Rebuild compiler: `cd build && cmake --build . -j$(nproc)`
- Rebuild musl: `cd musl && PATH="..." make -j$(nproc)`
- Run c-testsuite: `cd c-testsuite && PATH="..." ./run_tests.sh`
- Compare pass count before/after — must not decrease

#### Expected Impact

- Eliminates ~180 `name[0]` checks across handlers
- Eliminates ~30 `stoi` calls for classification
- Eliminates all GEP string parsing in handlers (moved to parser)
- Eliminates all `@.str` → `_str` duplication (single `nasmName()` method)
- Eliminates `null`/`false`/`true`/`undef` string checks (handled by `Kind` enum)
- Makes operand classification consistent across all handlers

---

### Phase 2: Typed Location Abstraction

**Goal:** Replace string-based register/stack locations with a typed `Location` that knows its kind and computes high-word offsets without string parsing.

**Root cause addressed:** RC2

#### Design

```cpp
// New: src/codegen/Location.h
class Location {
public:
    enum class Kind {
        Register,      // In a physical register (AX, BX, CX, DX, SI, DI)
        StackSlot,     // At [bp+offset]
        None,          // Not yet allocated (error if queried)
    };

    Kind kind = Kind::None;
    std::string regName;  // For Register: "ax", "bx", etc.
    int bpOffset = 0;     // For StackSlot: -8, +6, etc.
    bool is32bit = false; // Whether this holds a 32-bit value

    // Addressing helpers (compute, don't parse)
    std::string lowWordAddr() const;   // "[bp-8]" or "ax"
    std::string highWordAddr() const;  // "[bp-6]" or "ax" (for 32-bit: "dx" or "[bp-6]")
    bool isStack() const { return kind == Kind::StackSlot; }
    bool isRegister() const { return kind == Kind::Register; }
    bool isNone() const { return kind == Kind::None; }
};
```

#### Changes

1. **New file:** `src/codegen/Location.h` — defines `Location`
2. **Modify:** `src/codegen/StackFrame.h` — `getPhysReg()` returns `Location` instead of `string`; `getBpOffset()` returns `Location`; remove `getHighBpOffset()` (replaced by `Location::highWordAddr()`)
3. **Modify:** `src/codegen/StackFrame.cpp` — update implementations
4. **Modify:** All handler files — replace `find("bp")` checks with `loc.isStack()`; replace high-word computation with `loc.highWordAddr()`
5. **Modify:** `src/codegen/StackFrame.cpp` — `getPhysReg()` returns `Location::None` for unknown vregs instead of silently returning `"ax"` (makes bugs visible)

#### Migration Strategy

1. Add `Location` class alongside existing string returns
2. Add `Location getPhysRegLoc()` alongside `std::string getPhysReg()` in StackFrame
3. Migrate handlers one at a time to use `getPhysRegLoc()`
4. After all migrated, remove `getPhysReg()` string version

#### Expected Impact

- Eliminates all 90 `find("bp")` checks
- Eliminates all 16 high-word offset computation copies
- Makes it impossible to accidentally parse `"bx"` as a stack location
- Makes unknown vregs visible (returns `Location::None` instead of silently `"ax"`)

---

### Phase 3: Centralized Value Materialization

**Goal:** Create a single `ValueMaterializer` that handles loading/storing values. All handlers use this instead of each implementing their own load/store sequences.

**Root cause addressed:** RC5

#### Design

```cpp
// New: src/codegen/ValueMaterializer.h/.cpp
class ValueMaterializer {
public:
    // Load operand value into lowReg:highReg
    // Handles all operand kinds: constant, vreg, global, GEP, ptrtoint
    vector<Instruction286> loadValue(OperandRef op,
                                      string lowReg, string highReg);

    // Store value from lowReg:highReg to a memory location pointed by ptrOp
    vector<Instruction286> storeToMem(OperandRef ptrOp,
                                       string lowReg, string highReg,
                                       bool isSSDerived);

    // Push a 32-bit or 16-bit value onto the stack (for call arguments)
    vector<Instruction286> pushValue(OperandRef op, bool isPointer, bool is32bit);

    // Load address of a global (for lea bx, [name] pattern)
    vector<Instruction286> loadGlobalAddr(OperandRef op, string reg);

private:
    StackFrame& frame;
};
```

#### Changes

1. **New file:** `src/codegen/ValueMaterializer.h/.cpp` — central load/store/push logic
2. **Modify:** All handler files — replace inline 32-bit load sequences with `materializer.loadValue(...)`; replace store sequences with `materializer.storeToMem(...)`; replace call argument pushing with `materializer.pushValue(...)`
3. Global name conversion (`@.str` → `_str`) is done in one place inside `ValueMaterializer`
4. GEP/PtrToInt handling is done in one place inside `ValueMaterializer`

#### Expected Impact

- MemoryOps.cpp: 1490 → ~400 lines
- CallOps.cpp: 1231 → ~300 lines
- ArithmeticOps.cpp: 952 → ~400 lines
- All load/store/global-name/GEP fixes made in exactly one place
- Eliminates the largest source of duplication

---

### Phase 4: Fix Frame Size Computation (Immediate Win)

**Goal:** Make `totalFrameSize` accurate by ensuring all slot allocation happens in the analysis pass, not during lowering.

**Root cause addressed:** RC4

#### Design

**Option A: Two-pass lowering (recommended)**
```
Pass 3a: Lower all instructions, discard output, only track slot allocation
Pass 3b: Lower all instructions for real, with all slots pre-allocated
```

**Option B: Exact temp space counting (simpler)**
- `FunctionAnalysis::computeMaxTempSpace()` walks each block and counts every `allocResultSlot`/`allocTemp` call that would be made, including:
  - Load through register pointer: +4 bytes for pointer save
  - GEP: +4 bytes for pointer temp
  - Call result: +4 or +2 bytes
  - ICmp result: +2 bytes
  - Every instruction with resultName: +4 or +2 bytes
- Add bounds checking in `allocResultSlot()` and `allocTemp()` — assert if `tempCurrent` exceeds `tempBase + tempSize`

**Option C: Dynamic frame size (fallback)**
- `allocResultSlot()` and `allocTemp()` update `totalFrameSize` when they exceed the pre-allocated area
- Prologue reads `getFrameSize()` after lowering (already does this)
- Epilogue reads `getFrameSize()` during lowering (already does this)
- Since both read the same field after all allocations, they'll match

#### Changes (Option C — simplest, implement first)

1. **Modify:** `src/codegen/StackFrame.cpp` — `allocResultSlot()` and `allocTemp()` check if `tempCurrent + byteSize > tempBase + tempSize`; if so, extend `tempSize` and `totalFrameSize`
2. **Modify:** `src/codegen/StackFrame.cpp` — `allocStack()` updates `localsSize` and `totalFrameSize`
3. **Verify:** Prologue (Emitter.cpp:111) reads `getFrameSize()` from CodeGen.cpp (after lowering); epilogue (ControlFlowOps.cpp:113) reads `getFrameSize()` during lowering — both must see the same final value

#### Verification

1. Rebuild compiler
2. Compile test_hello.c
3. Run under td2ine: `cd 2ine_debugger/build && LD_LIBRARY_PATH=$(pwd) ./td2ine --autostep 200 --autostep-mode into ../../tests/output/test_hello.exe 2>&1 | tee /tmp/td2ine_trace.txt`
4. Verify prologue `sub sp, N` matches epilogue `add sp, N` for every function
5. Verify program exits cleanly with code 0

#### Expected Impact

- Fixes test_hello segfault
- Unblocks ~60 c-testsuite tests blocked by printf crash
- No API changes, minimal risk

---

### Phase 5: Real Register Allocation

**Goal:** Implement the stub `RegisterAllocator` with proper liveness tracking, replacing the ad-hoc `vregToPhys_` map.

**Root cause addressed:** RC3

#### Design

```
1. Build def-use chains from the IR (def in block B, use in block B')
2. Compute liveness: a vreg is live from its def to its last use
3. Allocate registers using linear scan:
   - AX: caller-saved, return values, scratch
   - CX: caller-saved, scratch
   - DX: caller-saved, high word of 32-bit values
   - BX: callee-saved, memory base register
   - SI: callee-saved, memory index register
   - DI: callee-saved, memory index register
4. When a vreg's live range ends, free its register
5. When all registers occupied, spill to stack slot
6. Track which register holds which vreg at each program point
```

#### Changes

1. **Modify:** `src/codegen/RegisterAlloc.cpp` — implement linear scan in `allocate()`
2. **Modify:** `src/codegen/StackFrame.h/.cpp` — replace `vregToPhys_` with allocator's allocation map; remove `setPhysReg()` (allocator decides assignments)
3. **Modify:** All handler files — remove 11 `setPhysReg(vreg, "ax")` calls; get register assignments from allocator
4. **Modify:** `src/codegen/InstructionSelect.cpp` — call `RegisterAllocator::allocate()` after lowering

#### Migration Strategy

1. Implement allocator alongside existing `vregToPhys_` map
2. Run both in parallel, compare results, verify they agree
3. Switch handlers to use allocator
4. Remove `vregToPhys_` map

#### Expected Impact

- Eliminates silent data corruption from AX overwrites
- Makes register usage deterministic and trackable
- Enables proper spilling instead of ad-hoc stack slots

---

### Phase 6: Integrate PHI Elimination

**Goal:** Move PHI handling into the instruction selection framework.

**Root cause addressed:** RC6

#### Changes

1. **Modify:** `src/codegen/ControlFlowOps.cpp` — `lowerBrTerm()` and `lowerCondBrTerm()` emit PHI stores before the jump (using `ValueMaterializer`)
2. **Modify:** `src/codegen/InstructionSelect.cpp` — `lowerBasicBlock()` PHI load logic (lines 148-178) stays but uses `ValueMaterializer`; PHI store logic (lines 186-296) is removed (moved to terminators)
3. **Modify:** `src/codegen/ControlFlowOps.cpp` — `lowerPhi()` becomes real handler emitting spill-slot loads
4. Remove the 150 lines of PHI store logic from `lowerBasicBlock()`

#### Expected Impact

- Eliminates 150 lines of duplicated logic
- Makes PHI handling testable in isolation
- PHI value classification uses `ValueMaterializer` (from Phase 3)

---

### Phase 7: Remove Emitter Workarounds (Immediate Win)

**Goal:** Fix the root cause of duplicate labels in the parser rather than deduplicating in the emitter.

**Root cause addressed:** RC7 (partial)

#### Changes

1. **Modify:** `src/ir/IrParser.cpp` — robustly strip `!llvm.loop` and all metadata attachments before ANTLR parsing
2. **Modify:** `src/ir/IrVisitor.cpp` — handle metadata-stripped IR without duplicating blocks
3. **Modify:** `src/codegen/Emitter.cpp` — remove `deduplicateLabels()` function (lines 56-90)
4. **Verify:** No more duplicate labels in output

#### Expected Impact

- Eliminates silent code dropping in emitter
- Removes 35 lines of workaround code

---

## Implementation Order

### Track 1: Immediate Wins (can start now, parallel)

1. **Phase 4: Frame Size Fix** — Option C (dynamic frame size)
   - Estimated effort: 1-2 hours
   - Fixes test_hello segfault
   - Unblocks c-testsuite re-run

2. **Phase 7: Emitter Fix** — Remove deduplication workaround
   - Estimated effort: 1 hour
   - Clean up parser metadata stripping

### Track 2: Strategic Refactoring (sequential, after Track 1)

3. **Phase 1: Typed Operands** — Highest impact
   - Estimated effort: 8-12 hours
   - Introduce `OperandRef`, migrate handlers one at a time
   - After each handler migration, run c-testsuite to verify no regression

4. **Phase 2: Typed Locations** — Depends on Phase 1
   - Estimated effort: 4-6 hours
   - Introduce `Location`, migrate handlers one at a time

5. **Phase 3: ValueMaterializer** — Depends on Phases 1, 2
   - Estimated effort: 6-8 hours
   - Centralize load/store/push logic
   - Dramatic reduction in handler file sizes

6. **Phase 5: Register Allocation** — Depends on Phases 1, 2
   - Estimated effort: 8-12 hours
   - Implement linear scan allocator
   - Eliminate AX overwrite bugs

7. **Phase 6: PHI Integration** — Depends on Phases 1, 3
   - Estimated effort: 2-4 hours
   - Move PHI handling into framework

### Total Estimated Effort

- Track 1 (immediate): 2-3 hours
- Track 2 (strategic): 28-42 hours
- **Total: 30-45 hours**

---

## Verification Plan

### Per-Phase Verification

After each phase:
1. Rebuild compiler: `cd build && cmake --build . -j$(nproc)`
2. Rebuild musl: `cd musl && PATH="/home/stash/code/llvm-i286/build:/home/stash/code/llvm-i286:/home/stash/code/llvm-i286/musl_build:$PATH" make -j$(nproc)`
3. Run c-testsuite: `cd c-testsuite && PATH="..." ./run_tests.sh`
4. Compare pass count — must not decrease
5. Run test_hello under td2ine if segfault-related changes

### Regression Benchmarks

- c-testsuite pass count (baseline: 148/220)
- musl libc.a build success (baseline: passes)
- test_hello.exe exit code (baseline: segfault)
- Compilation time (baseline: measure before refactoring)

### Final Validation

After all phases complete:
1. Run full c-testsuite — target: 200+/220 pass
2. Run test_hello — target: exit code 0
3. Run printf test — target: correct output
4. Verify no regressions in musl build

---

## Risk Mitigation

### Risk: Big-bang refactor breaks everything

**Mitigation:** Phased approach with per-phase verification. Each phase is independently testable. Track 1 (immediate wins) can proceed in parallel with Track 2 planning.

### Risk: OperandRef migration introduces bugs

**Mitigation:** Dual-field migration strategy — populate both `OperandRef ref` and `std::string name` during parsing. Migrate handlers one at a time. After each migration, run c-testsuite. If regression, revert that handler.

### Risk: Register allocator is wrong

**Mitigation:** Run allocator alongside existing `vregToPhys_` map in parallel. Compare results. Switch only when confident.

### Risk: Frame size fix doesn't fix segfault

**Mitigation:** Phase 4 has three options (A, B, C). Start with Option C (simplest). If it doesn't fix the segfault, use td2ine trace to identify the actual mismatch, then try Option A (two-pass lowering).

### Risk: exit() infinite loop in musl (__sys_pause / SYS_pause=999)

**Status:** Observed after Phase 4 fix. test_hello compiles and links but loops in musl's `exit()` function calling `__os2_syscall1(999, 0)` (SYS_pause) in a `for(;;)` loop.

**Root cause analysis:** `exit()` uses `a_cas(exit_lock, 0, tid)` where `tid = __pthread_self()->tid`. `__get_tp()` returns 0 (no TLS on i286), so `__pthread_self()` is NULL. The `a_cas` inline function does a 32-bit compare-and-swap on `exit_lock`. If codegen compiles `a_cas` incorrectly (wrong return value, wrong 32-bit comparison, or wrong conditional store), `prev` becomes non-zero, triggering the `for(;;) __sys_pause()` branch.

**Likelihood this is a codegen bug:** High (70-80%). `a_cas` is a simple inline function (`int old = *p; if (old == t) *p = s; return old;`) that involves 32-bit load, 32-bit comparison, conditional store, and 32-bit return — exactly the patterns RC1 (string-based operands) and RC2 (string-based locations) address.

**Mitigation:** Postpone musl workaround until after Phases 1-3. Re-test test_hello after each phase. If the loop persists after Phase 3, implement a musl platform fix: make `__get_tp()` return a pointer to a static `struct __pthread` instance (allocated in BSS) instead of 0, so `__pthread_self()` dereferences a valid struct with `tid = 1`. This is a minimal musl change in `arch/i286/pthread_arch.h`.

**Verification:** After Phase 3, trace `exit()` / `a_cas` under td2ine to confirm whether `prev` is non-zero (codegen bug) or zero (musl platform issue). If `prev` is 0 and the loop still triggers, the control flow codegen is wrong. If `prev` is non-zero, check whether `a_cas` returns the correct value.

---

## Success Criteria

- [ ] test_hello.exe exits with code 0 (no segfault)
- [ ] c-testsuite pass rate > 85% (188+/220)
- [ ] No `find("bp")` checks in handler files (Phase 2 complete)
- [ ] No `name[0] == '@'` checks in handler files (Phase 1 complete)
- [ ] No `setPhysReg(vreg, "ax")` calls in handler files (Phase 5 complete)
- [ ] No `deduplicateLabels()` in Emitter (Phase 7 complete)
- [ ] MemoryOps.cpp < 500 lines (Phase 3 complete)
- [ ] CallOps.cpp < 400 lines (Phase 3 complete)
- [ ] No code duplication for high-word offset computation (Phase 2 complete)
- [ ] No code duplication for global name conversion (Phase 1 complete)