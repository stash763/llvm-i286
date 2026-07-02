# LLVM i286 Code Generator - End-to-End Pipeline Complete

## Achievement

**Full end-to-end pipeline now working:**

```
C Source → clang → LLVM IR → llvm-i286 → NASM → nasm → Object File → wlink → OS/2 1.x NE Executable → lx_loader → Runs!
```

## Test Results

### Test 1: Simple Return (C → LLVM IR → OS/2 executable)
- **Source:** `tests/os2/test_return.c`
- **LLVM IR:** `tests/output/test_return.ll`
- **NASM:** `tests/output/test_return.asm`
- **Executable:** `test_return.exe`
- **Result:** ✅ Runs successfully, exits with code 0

### Test 2: Hello World (hand-written NASM)
- **NASM:** `tests/output/test_hello_print.asm`
- **Executable:** `test_hello_print.exe`
- **Result:** ✅ Prints "Hello from LLVM i286!" to stdout

### Test 3: Addition (LLVM IR)
- **LLVM IR:** `tests/output/test_add.ll`
- **NASM:** `tests/output/test_add.asm`
- **Executable:** `test_add.exe`
- **Result:** ✅ Runs successfully, exits with code 0

## Build Process

### Prerequisites
- OpenWatcom v2 installed at `~/ow/open-watcom-v2/`
- NASM installed
- lx_loader built in `2ine_debugger/build/`

### Build Script
```bash
./build_os2.sh <input.asm> <output.exe>
```

### Manual Build Steps
```bash
# 1. Assemble
nasm -f obj -o test.o test.asm

# 2. Link (with OpenWatcom environment)
export WATCOM=~/ow/open-watcom-v2/rel
export PATH=$PATH:$WATCOM/binl
wlink system os2 d all path $WATCOM/lib286 library $WATCOM/lib286/os2/os2.lib name test.exe file "$PWD/test.o"

# 3. Run (in 32-bit container or native 32-bit environment)
cd 2ine_debugger/build
LD_LIBRARY_PATH=. ./lx_loader ../../test.exe
```

## Key Changes

### Code Generator
- Modified Emitter to generate OS/2-compatible NASM with:
  - Proper segment structure (DGROUP, _DATA, STACK, _TEXT)
  - OS/2 API externals (DOSEXIT, DOSWRITE, DOSREAD)
  - Entry point at `..start:` with direct code execution
  - Exit via `call far DOSEXIT`

### CodeGen Pipeline
- Modified to emit user instructions directly at `..start:` without function prologue/epilogue
- Strips function wrapper (push bp, mov bp, sp, push/pop bx/si/di, ret)
- Preserves actual user instructions

### Build Infrastructure
- Created `build_os2.sh` script for proper OpenWatcom environment setup
- Script handles nasm assembly and wlink linking with correct library paths

## Current Limitations

1. **No function call support** - Programs with multiple functions (e.g., `main` calling `add`) not yet supported
2. **No stack frame setup** - Local variables using `[bp-N]` addressing may not work correctly
3. **Limited instruction support** - Only basic arithmetic, memory operations, and return supported
4. **No string handling** - Cannot generate code that prints strings (must be hand-written)
5. **32-bit environment required** - lx_loader requires 32-bit environment (built in podman container)

## Next Steps

1. **Implement function calls** - Support `call` instruction for calling functions
2. **Add stack frame support** - Generate proper prologue/epilogue when needed
3. **Implement string constants** - Support for global string data
4. **Add DOSWRITE support** - Generate code to print strings
5. **Expand instruction support** - Add more LLVM IR instructions (icmp, condbr, etc.)
6. **Improve register allocation** - Better handling of parameters and return values

## Files

- `build_os2.sh` - Build script for OS/2 executables
- `src/codegen/CodeGen.cpp` - Modified to emit user code directly
- `src/codegen/Emitter.cpp` - Modified to generate OS/2-compatible NASM
- `tests/output/test_hello_print.asm` - Example NASM that prints text
- `tests/os2/test_return.c` - Simple C test program
- `2ine_debugger/build/lx_loader` - OS/2 executable loader

---

**Date:** 2026-06-29
**Status:** ✅ End-to-end pipeline working
