# Label Management Refactoring Plan

## Problem Statement

The current codebase has a label duplication issue caused by `!llvm.loop` metadata in LLVM IR. When clang compiles loops, it attaches `!llvm.loop` metadata to back-edge branches. The IR parser currently creates duplicate basic blocks when processing these branches, leading to:

1. NASM compilation errors ("label inconsistently redefined")
2. Incorrect program behavior (stray `jmp` instructions creating infinite loops)

The current workaround in `Emitter.cpp` uses a `deduplicateLabels()` pass that keeps the last occurrence of each duplicate label and discards earlier ones. This is a band-aid that masks the underlying problem.

## Root Cause Analysis

**Location**: `src/ir/IrVisitor.cpp`

The IR parser's `visitBasicBlock()` and `parseTerminator()` functions don't check for duplicate block labels. When a branch instruction has `!llvm.loop` metadata, the parser may encounter the target block multiple times (once from the natural flow, once from the back-edge), creating duplicate `BasicBlock` instances with the same label.

**Evidence**:
- IR for `test_strlen` has block `%4` referenced from both block `%1` (entry) and block `%8` (loop back-edge with `!llvm.loop`)
- Generated assembly shows two `my_strlen_bb_4:` labels
- NASM rejects duplicate labels

## Proposed Solution: Three-Phase Refactoring

### Phase 1: Fix the IR Parser (Root Cause)

**File**: `src/ir/IrVisitor.cpp`

**Approach**: Add duplicate block detection in `visitFunction()` or `visitBasicBlock()`.

```cpp
// In visitFunction() or after parsing each basic block:
std::set<std::string> seenBlockLabels;

for (auto* bbCtx : funcCtx->basicBlocks()) {
    auto bb = parseBasicBlock(bbCtx);
    
    // Check for duplicate label
    if (!bb->label.empty() && !seenBlockLabels.insert(bb->label).second) {
        // Duplicate found - skip this block (it's a re-emission from !llvm.loop)
        continue;
    }
    
    function->basicBlocks.push_back(std::move(bb));
}
```

**Pros**:
- Fixes the problem at the source
- No downstream changes needed
- Cleanest solution

**Cons**:
- Requires understanding exactly when/why the parser creates duplicates
- May need to handle edge cases (e.g., blocks referenced before they're defined)

**Risk**: Low - the parser already creates blocks correctly in most cases; we just need to detect and skip duplicates.

---

### Phase 2: Create a Label Resolution Pass (Defensive)

**New File**: `src/codegen/LabelPass.h`, `src/codegen/LabelPass.cpp`

Even after fixing the parser, we should have a robust label management system that:
1. Validates all labels are unique
2. Detects stray loop headers (single `jmp` to self)
3. Merges duplicate blocks if any slip through
4. Validates all jump targets reference valid labels

**Design**:

```cpp
class LabelPass {
public:
    // Run the label resolution pass
    std::vector<LoweredInstruction> resolve(
        const std::vector<LoweredInstruction>& instructions,
        const std::string& funcName);

private:
    // Collect all labels and their positions
    void collectLabels(const std::vector<LoweredInstruction>& instructions);
    
    // Detect and handle duplicate labels
    void handleDuplicates();
    
    // Detect and remove stray loop headers (label: jmp label)
    void removeStrayLoopHeaders();
    
    // Validate all jump targets reference valid labels
    bool validateJumpTargets();
    
    struct LabelInfo {
        std::string label;
        size_t position;
        std::vector<Instruction286> instructions;
    };
    
    std::map<std::string, LabelInfo> labelMap;
    std::vector<LoweredInstruction> result;
};
```

**Integration**:
- Called in `InstructionSelector::lowerFunction()` after all blocks are lowered
- Before passing to `Emitter::emitFunction()`

```cpp
// In InstructionSelector::lowerFunction():
LabelPass labelPass;
auto resolved = labelPass.resolve(lowered, func.name);

// Pass to emitter
output += emitter.emitFunction(resolved, func.name, frameSize);
```

**Pros**:
- Defensive - catches any label issues regardless of source
- Can validate jump targets (catch bugs early)
- Separates concerns: instruction selection doesn't worry about labels

**Cons**:
- Adds complexity
- May hide bugs that should be fixed at the source

**Risk**: Medium - needs careful implementation to avoid breaking valid programs.

---

### Phase 3: Clean Up Emitter

**File**: `src/codegen/Emitter.cpp`

After Phase 1 and 2 are complete, the Emitter no longer needs label deduplication logic:

```cpp
std::string Emitter::emitFunction(const std::vector<LoweredInstruction>& instructions,
                                  const std::string& funcName,
                                  int frameSize) {
    // ... prologue ...
    
    // No label deduplication needed - handled by LabelPass
    for (const auto& lowered : instructions) {
        if (lowered.label == funcName) continue;
        output += emitLoweredInstruction(lowered);
    }
    
    return output;
}
```

**Pros**:
- Emitter is simpler and more focused
- Easier to maintain and test

---

## Implementation Order

1. **Start with Phase 1** (fix the parser) - this is the root cause
2. **Then Phase 3** (clean up Emitter) - remove the workaround
3. **Test thoroughly** - ensure all 18 tests pass
4. **Optionally add Phase 2** (LabelPass) as defensive programming

## Testing Strategy

After each phase:
- Run full test suite (`bash tests/run_tests.sh`)
- Verify all 18 tests pass (currently 16/18 - the other 2 are unrelated string comparison bugs)
- Check generated assembly for:
  - No duplicate labels
  - No stray `jmp` instructions (label: jmp label)
  - All jump targets resolve to valid labels

## Additional Bugs to Fix

The current test failures are **NOT** related to label management:
- `test_memcmp` returns `-104` instead of `0` - likely a pointer arithmetic or comparison bug
- `test_strcmp` returns `65432` instead of `0` - likely a sign-extension or comparison bug

These should be investigated separately after the label refactoring is complete.

## Files to Modify

1. `src/ir/IrVisitor.cpp` - Add duplicate block detection (Phase 1)
2. `src/codegen/Emitter.cpp` - Remove deduplication logic (Phase 3)
3. `src/codegen/LabelPass.h` - New file (Phase 2, optional)
4. `src/codegen/LabelPass.cpp` - New file (Phase 2, optional)
5. `src/codegen/InstructionSelect.cpp` - Integrate LabelPass (Phase 2, optional)

## Success Criteria

- All 18 tests pass
- No duplicate labels in generated assembly
- No stray loop headers
- Emitter is clean and simple
- (Optional) LabelPass validates all jump targets
