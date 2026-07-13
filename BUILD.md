# LLVM i286 — Build, Test, and Project Guide

## Project Goal

Build a complete toolchain that compiles C17 source into 80286 16-bit protected-mode OS/2 1.x
executables via an LLVM-based backend:

```
C Source ──(clang)──► LLVM IR ──(llvm-i286)──► NASM .asm
  ──(nasm)──► .obj ──(wlink)──► .exe ──(lx_loader)──► Runs!
```

**Target architecture**: Intel 80286, 16-bit protected mode  
**Target OS**: OS/2 1.x (NE-format executables, segmented 16-bit memory model)  
**Data model**: All types 32-bit (int, long, pointers) emulated on 16-bit hardware

## Project Goals

- Provide a working code generator that lowers LLVM IR to real 80286 NASM assembly
- Support a useful subset of C (pointers, arrays, structs, control flow, function calls)
- Produce executables that actually run on real or emulated 80286 hardware
- Support a partial musl libc port so user C programs can link against a real standard library
- Test everything end-to-end with an automated test suite

## Architecture Overview

```
llvm-i286/
├── clang_i286         # Pipeline driver (C → IR → asm → obj → exe)
├── build/llvm-i286    # Code generator binary
│   └── src/
│       ├── ir/        # LLVM IR parser (ANTLR4 visitor → internal AST)
│       ├── codegen/   # IR → 80286 NASM lowering
│       └── target/    # Target-specific helpers
├── runtime/           # Runtime library (div32, mul32, os2_syscall, itoa, printnum)
├── tests/
│   ├── os2/           # Source C test files
│   ├── output/        # Generated .ll, .asm, .exe
│   └── run_tests.sh   # Automated test runner
├── musl/              # musl libc sources (arch/i286/, include/, ...)
├── musl_port/         # musl porting helpers (printf, va_start)
├── grammars-v4/       # ANTLR4 grammars (llvm-ir/, c/, ...)
└── 2ine_debugger/     # OS/2 executable runner (lx_loader)
```

### Code Generation Pipeline

1. **Parse**: ANTLR4 grammar (`grammars-v4/llvm-ir/LLVMIR.g4`) → AST
2. **Visit**: `IrVisitor` walks AST → internal `Module`/`Function`/`BasicBlock`/`Instruction`
3. **Lower**: `Selector` walks functions, emits 80286 NASM instructions
4. **Emit**: `Emitter` wraps instructions in proper NASM module format

### Calling Convention

- Arguments pushed right-to-left on stack
- Return value in `AX` (16-bit) or `DX:AX` (32-bit)
- Caller cleans up stack
- SS-derived pointers (allocas) accessed with `SS:` segment override
- DS-derived pointers (parameters, globals) accessed without segment override

## Tool Dependencies

| Tool | Version | Purpose |
|------|---------|---------|
| **CMake** | 3.16+ | Build system |
| **clang** | 15 | C → LLVM IR generation (default; `CLANG=clang-15`) |
| **NASM** | 2.16.01 | Assembler for generated 80286 code |
| **OpenWatcom** v2 | Any | wlink linker (NE-format executables) |
| **ANTLR4** | 4.13.3-SNAPSHOT | IR parser generator (Java + C++ runtime) |
| **2ine_debugger** | Local build | OS/2 executable runner (`lx_loader`) |

**Note**: clang-15 is the supported version. clang-18 may work but is not tested.

## Build Instructions

### Prerequisites

```bash
# System packages (Ubuntu/Debian)
sudo apt install cmake g++ nasm openjdk-17-jdk

# OpenWatcom v2 (install manually)
# Download from https://github.com/open-watcom/open-watcom-v2/releases
# Install to ~/ow/open-watcom-v2/ (default path used by build scripts)

# 2ine_debugger (local build required)
cd 2ine_debugger
mkdir build && cd build
cmake .. && make
```

### Build the Code Generator

```bash
mkdir -p build && cd build
cmake .. && make -j$(nproc)
# → produces build/llvm-i286 (code generator binary)
```

### Build the Runtime Library

```bash
bash runtime/build_runtime.sh
# → produces runtime/runtime.lib (div32, mul32, os2_syscall, itoa, printnum)
```

The runtime library provides:
- **div32/mul32**: 32-bit arithmetic helpers (since 80286 only has 16-bit instructions)
- **os2_syscall**: OS/2 DOSCALL dispatcher for Linux `2ine` compatibility layer
- **itoa/printnum**: Integer-to-string conversion helpers

## Test Instructions

### Full Test Suite

```bash
# Run all tests (from project root)
bash tests/run_tests.sh
```

Each test is:
1. Compiled with `clang_i286`
2. Linked into an OS/2 NE executable
3. Run under `lx_loader`
4. Output compared against expected values

Expected result: **19 pass, 1 skip** (test_printf_simple is a known limitation)

### Single Test

```bash
# Compile a single test
./clang_i286 tests/os2/test_add.c -o /tmp/test_add.exe

# Run it under lx_loader
cd 2ine_debugger/build
LD_LIBRARY_PATH=. ./lx_loader /tmp/test_add.exe

# Or just see the generated assembly
./clang_i286 -S tests/os2/test_add.c -o /tmp/test_add.asm
cat /tmp/test_add.asm
```

### Test Files

- `tests/os2/*.c` — 20 test source files
- `tests/os2/output_helper.h` — Inline `int_to_str`/`print_int` helpers (no runtime deps)
- `tests/run_tests.sh` — Automated runner with expected output comparison

### Output Helper Library

`tests/os2/output_helper.h` provides inline helpers so tests can format and print values
using `__os2_syscall3` without depending on the runtime `printnum` or `itoa`:

```c
#include "output_helper.h"
char buf[16];
print_int(42, buf);
__os2_syscall3(2, 1, (long)buf, strn_len(buf));
```

This is the recommended pattern for new tests.

## Using clang_i286

```bash
# Compile to executable (default)
./clang_i286 input.c -o output.exe

# Emit NASM assembly only
./clang_i286 -S input.c -o output.asm

# Emit object file only
./clang_i286 -c input.c -o output.o

# Emit LLVM IR only
./clang_i286 -emit-llvm -S input.c -o output.ll

# Link multiple files
./clang_i286 main.c helper.c -o output.exe

# Use custom clang
CLANG=clang-18 ./clang_i286 input.c -o output.exe

# Verbose mode
./clang_i286 -v input.c -o output.exe
```

### Environment Variables

| Variable | Default | Purpose |
|----------|---------|---------|
| `CLANG` | `clang-15` | clang binary |
| `LLVM_I286` | `./build/llvm-i286` | Code generator binary |
| `WATCOM` | `~/ow/open-watcom-v2/rel` | OpenWatcom installation root |
| `RUNTIME_LIB` | `./runtime/runtime.lib` | Runtime library path |
| `LX_LOADER` | `./2ine_debugger/build/lx_loader` | OS/2 executable runner |
| `LIB2INE` | `./2ine_debugger/build/lib2ine.so` | 2ine compatibility layer |
| `LIBDOSCALLS` | `./2ine_debugger/build/libdoscalls.so` | DOSCALL library |
| `WATCOM_DIR` | `$WATCOM` | OpenWatcom root for build scripts |
| `NASM` | `nasm` | NASM assembler |
| `WLINK` | `$WATCOM_DIR/binl/wlink` | wlink linker |
| `WLIB` | `$WATCOM_DIR/binl/wlib` | wlib archiver |

## Running Generated Executables

Generated executables are OS/2 1.x NE-format binaries. They cannot run directly on Linux.
Use the `lx_loader` from the `2ine_debugger` project:

```bash
cd 2ine_debugger/build
LD_LIBRARY_PATH=. ./lx_loader /path/to/your.exe
```

`lx_loader` is a 32-bit Linux executable built inside `2ine_debugger/build/`. It provides:
- A 32-bit address space for the NE binary to run in
- Emulated OS/2 DOSCALL API (DosWrite, DosRead, DosExit, etc.)
- Segment selector management for the 80286 segmented model

## musl libc Port

The `musl/` directory contains a partial port of musl libc for the i286 target.
See `MUSL_PORT_README.md` for details on current status and how to use it.

### Current Status

- **Working**: Header files for most C standard library types and functions
- **Working**: Architecture-specific atomic operations, syscall numbers, CRT startup
- **Working**: `__os2_syscall3` dispatch for read/write/exit syscalls
- **Working**: printf_min.c (minimal printf with %d, %s, %%, literal characters)
- **Limitation**: Complex printf format specifiers (%f, %x, etc.) not yet supported
- **Limitation**: File I/O, malloc, string functions beyond basic memcmp/memcpy not yet ported

## Known Limitations

- **No dynamic linking**: All executables are statically linked
- **No floating point**: FP operations not supported (push 0 as placeholder)
- **No 32-bit division in codegen**: Uses runtime `_DivideI32`/`_DivideU32`
- **Limited variadic functions**: Only basic va_start/va_arg support
- **No exception handling**: C++ exceptions not supported
- **No threads**: No pthread support yet
- **Single segment**: All data fits in one 64K DGROUP segment initially

## Development

### Code Generator Source

```
src/codegen/
├── CodeGen.cpp           # Main pipeline orchestrator
├── InstructionSelect.cpp  # IR → NASM lowering (core logic)
├── InstructionSelectInternal.h  # SelectorState, helper types
├── MemoryOps.cpp         # alloca, load, store, GEP lowering
├── CallOps.cpp           # Call instruction lowering (including va_start inline)
├── ArithmeticOps.cpp     # add, sub, mul, div, rem lowering
├── BitwiseOps.cpp        # and, or, xor, shl, lshr, ashr lowering
├── ControlFlowOps.cpp    # br, condbr, switch lowering
├── ConversionOps.cpp     # trunc, zext, sext, ptrtoint, inttoptr
├── Emitter.cpp           # NASM text emission
├── StackFrame.cpp/.h     # Stack frame layout (allocas, params, temps)
└── Addressing.cpp/.h     # Address calculation helpers
```

### Key Design Decisions

- **Two-pass lowering**: Stack frame layout computed first, then instructions lowered
- **SS-derived tracking**: Allocas are SS-derived; loaded pointer values may target DS or SS
- **Inline va_start**: llvm.va.start generates inline code, no external runtime call
- **No inline asm in user code**: All segment selection handled by codegen

### Adding a New Test

1. Create `tests/os2/my_test.c`
2. Use `output_helper.h` for output:
   ```c
   #include "output_helper.h"
   int main() {
       char buf[16];
       print_int(42, buf);
       __os2_syscall3(2, 1, (long)buf, strn_len(buf));
       return 0;
   }
   ```
3. Add expected output to `tests/run_tests.sh`:
   ```bash
   EXPECTED_OUTPUT[my_test]="42"
   ```
4. Run: `bash tests/run_tests.sh 2>&1 | grep my_test`
