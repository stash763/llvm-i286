# LLVM i286 Code Generator - Progress Report

## Completed (Phase 1-3)

### Project Infrastructure
- ✅ CMake build system with ANTLR4 integration
- ✅ ANTLR4 C++ runtime (pre-built binary from previous project)
- ✅ ANTLR4 Java tool built from source (version 4.13.3-SNAPSHOT)
- ✅ LLVM IR grammar parser generated from `grammars-v4/llvm-ir/LLVMIR.g4`
- ✅ Project structure: src/{ir,codegen,target,runtime,formats,frontend}, antlr, tests/

### LLVM IR Parser (Phase 2)
- ✅ ANTLR4 visitor implementation for LLVM IR
- ✅ Module parsing (target triple, data layout, source filename)
- ✅ Function definition and declaration parsing
- ✅ Parameter parsing with type information
- ✅ Basic block parsing with labels
- ✅ Instruction parsing (arithmetic, bitwise, memory, control flow, conversions)
- ✅ Terminator parsing (ret, br, condbr, switch, unreachable)
- ✅ Internal AST representation (Module, Function, BasicBlock, Instruction, Type, Value)
- ✅ Call instruction parsing (callee name, arguments)

### Code Generation (Phase 3)
- ✅ Instruction selection for basic operations:
  - Arithmetic: add, sub, mul
  - Bitwise: and, or, xor, shl, lshr, ashr
  - Memory: alloca, store, load
  - Control flow: ret, br (basic implementation)
  - Function calls: call (with argument pushing, stack cleanup)
- ✅ Register allocation with tracking:
  - Parameters mapped to BX, SI, DI (callee-saved registers)
  - Return value in AX
  - Temp register allocation (AX, CX, DX) for intermediate values
  - Register occupancy tracking to avoid conflicts
- ✅ NASM text emission
  - Function prologue/epilogue (push/pop bp, bx, si, di)
  - 16-bit instruction encoding
  - Proper NASM syntax
  - Memory operand handling (bp-relative addresses with brackets)
- ✅ Module structure (BITS 16, segment declarations)

### Testing
- ✅ Parser tests (5/5 passing)
  - Simple return, constants, function parameters, multiple functions, clang-generated IR
- ✅ Arithmetic operation tests (8/8 passing)
  - i16 add, sub, mul, and, or, xor
  - Multiple operations in sequence
  - Return constants
- ✅ Memory operation tests (alloca, store, load)
- ✅ NASM assembly validation (all generated code assembles successfully)
- ✅ Clang output compatibility (test_add.c → IR → NASM → assembles)

## Test Results

```
=== Basic Arithmetic Operation Tests ===
Test 1: i16_add... PASS
Test 2: i16_sub... PASS
Test 3: i16_mul... PASS
Test 4: i16_and... PASS
Test 5: i16_or... PASS
Test 6: i16_xor... PASS
Test 7: multiple_ops... PASS
Test 8: return_constant... PASS

Total: 8
Pass: 8
Fail: 0
```

## Example Generated Code

**Input LLVM IR:**
```llvm
define i16 @add_test(i16 %a, i16 %b) {
entry:
  %result = add i16 %a, %b
  ret i16 %result
}
```

**Generated NASM:**
```nasm
BITS 16

segment _TEXT CLASS=CODE

global add_test
add_test:
  push bp
  mov bp, sp
  push bx
  push si
  push di
entry:
  mov ax, bx
  add ax, si
  pop di
  pop si
  pop bx
  pop bp
  ret
```

## Current Limitations

1. **32-bit Operations**: i32 operations treated as 16-bit (need multi-word emulation)
2. **Register Allocation**: Simplified scheme, limited to AX/CX/DX for temps, no spilling yet
3. **Control Flow**: condbr, switch not fully implemented (condbr needs icmp)
4. **Segment:Offset**: No segmentation support yet
5. **Type Conversions**: trunc, zext, sext not implemented
6. **GEP**: getelementptr not implemented (needed for struct/array access)
7. **lx_loader**: Cannot compile on 64-bit system (uses 32-bit ucontext API)

## Next Steps (Phase 4+)

### High Priority
1. **Control Flow** (condbr, icmp) - needed for if/else, loops
2. **32/64-bit Emulation** - required for NetBSD porting goal
3. **Type Conversions** (trunc, zext, sext) - needed for clang output
4. **GEP** (getelementptr) - needed for struct/array access
5. **Segment:Offset Addressing** - required for 286 protected mode

### Medium Priority
6. **NE Format Output** - link with wlink for OS/2 executables
7. **Test Harness** - full pipeline: clang → IR → NASM → wlink → 2ine
8. **Debug Integration** - integrate with td2ine debugger
9. **lx_loader fix** - port to 64-bit or use 32-bit compilation

### Low Priority
10. **NetBSD Porting** - 32-bit linear to segmented address translation

## Tool Chain

```
C/C++ Source → clang → LLVM IR → llvm-i286 → NASM Assembly → nasm → Object File → wlink → NE Executable → 2ine (lx_loader)
```

**Current pipeline status**: C → clang → LLVM IR → llvm-i286 → NASM → nasm ✅ (assembles successfully)
**wlink**: Can create NE executables ✅
**2ine execution**: Blocked on lx_loader 32-bit compilation issue

## Recent Changes (2026-06-29)

### Register Tracking
- Added `physToVreg` map to track which vreg is in which physical register
- Added `assignReg()` helper to allocate registers for results
- Added `getFreeTempReg()` to find available registers (AX, CX, DX)
- Load instruction now assigns different registers for multiple loads (AX, then CX)

### Memory Operations
- Store instruction works correctly with bp-relative addressing
- Load instruction properly loads from memory into assigned registers
- Add instruction handles memory operands by loading into registers first

### Call Instruction
- Implemented call instruction with argument pushing (right-to-left)
- Stack cleanup after call (add sp, N)
- Result tracking (return value in AX)
- Handles both constant and register arguments

### Generated Code Quality
- `test_add.c` now generates correct NASM that assembles and links
- Load operations use different registers (AX for first, CX for second)
- Add operation correctly adds AX + CX
- Call operation pushes args and calls function properly

## Build Commands

```bash
# Build ANTLR4 tool (one-time)
cd antlr4 && mvn install -DskipTests

# Build project
mkdir build && cd build
cmake ..
make

# Run tests
./tests/test_parser.sh
./tests/test_arithmetic.sh

# Generate code
./build/llvm-i286 input.ll -o output.asm

# Assemble
nasm -f obj -o output.o output.asm
```
