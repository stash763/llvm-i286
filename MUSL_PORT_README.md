# musl libc Port to llvm-i286 (OS/2 1.x)

## Overview

This directory contains the partial port of musl libc to the llvm-i286 code generator,
targeting OS/2 1.x in 16-bit protected mode on Intel 80286 processors.

## Current Status

### Phase 1 Complete: 143/143 musl functions compile cleanly

**String functions** (74/74 ✅):
- All core memory ops: `memcpy`, `memmove`, `memset`, `memcmp`, `memchr`
- All string operations: `strlen`, `strcmp`, `strcpy`, `strcat`, `strstr`, `strtok`
- All wide character variants: `wcscmp`, `wcsncmp`, `wcscpy`, `wcschr`, etc.
- Error string functions: `strerror`, `strerror_r`, `strsignal`

**Ctype functions** (37/37 ✅):
- All character classification: `isalpha`, `isdigit`, `isupper`, `islower`, etc.
- All character conversion: `tolower`, `toupper`, `toascii`
- All wide character variants: `iswalnum`, `iswalpha`, `iswctype`, etc.

**Stdlib functions** (22/22 ✅):
- All conversion: `atoi`, `atol`, `atof`, `strtod`, `strtol`
- All absolute/division: `abs`, `labs`, `div`, `ldiv`, `imaxabs`, `imaxdiv`
- Search/sort: `bsearch`, `qsort`, `qsort_nr`

**Exit functions** (8/8 ✅):
- `abort`, `atexit`, `at_quick_exit`, `exit`, `_Exit`, `quick_exit`

**Errno functions** (2/2 ✅):
- `__errno_location`, `strerror`

### Phase 2: Full musl libc build in progress

**Toolchain integration:**
- `clang_i286` wrapper configured as musl's CC (drop-in clang replacement)
- Musl's build system configured: `CC=clang_i286 CFLAGS="-m32 -std=gnu99"`
- Pipeline works end-to-end: C source → LLVM IR → NASM → .obj → .exe via `wlink`
- Build progresses through most musl source files successfully

**Remaining musl build issues:**
- `semctl.c` - union semun in va_arg (musl source compatibility, not toolchain issue)
- Other POSIX API calls may need OS/2 syscall stubs
- These are fixed as they're encountered, not blocking overall build progress

**What's different about our approach:**
- `clang_i286` is a **generic C compiler wrapper**, not musl-specific
- Works with musl, c-testsuite, and any user C code
- Produces OS/2 NE format executables via wlink
- Supports full pipeline: preprocessing, compilation, assembly, linking

### Working

- **Header files**: Type definitions, syscall numbers, CRT startup for i286 architecture
  - `musl/arch/i286/bits/alltypes.h` — 32-bit data model types (int=32, long=32, ptr=32)
  - `musl/arch/i286/syscall_arch.h` — OS/2 syscall numbers (with stubs for missing syscalls)
  - `musl/arch/i286/bits/errno.h` — 150+ errno values
  - `musl/arch/i286/atomic_arch.h` — Atomic operations for i286
  - `musl/arch/i286/crt_arch.h` — CRT startup code
  - `musl/include/stdarg.h` — Custom `va_arg` macro (no inline asm, uses codegen SS-derived pointers)
- **Syscall dispatch**: `__os2_syscall{0..6}` for OS/2 API calls
- **printf_min**: Minimal printf with `%d`, `%s`, `%%`, and literal characters
- **va_start inline**: Codegen generates inline `llvm.va.start` (no external runtime call)
- **Runtime library**: `runtime/runtime.lib` with `_MultiplyI32`, `_DivideI32`, `_DivideU32`, `itoa`, `printnum`, `__os2_syscall{0..6}`

### Limitations (Phase 2+)

- **No file I/O**: fopen/fread/fwrite/fclose not ported (next: Phase 2)
- **No malloc/free**: Memory allocation not supported (next: Phase 3)
- **Limited printf**: Only `%d`, `%s`, `%%` supported (no `%f`, `%x`, `%ld`, etc.)
- **No threads**: No pthread support
- **No locale**: No locale support

## Directory Structure

```
musl/
├── arch/i286/              # i286 architecture-specific files
│   ├── bits/               # Header files for types, syscalls, etc.
│   │   ├── alltypes.h      # 32-bit data model type definitions
│   │   ├── syscall.h       # OS/2 syscall numbers (via syscall_arch.h)
│   │   ├── errno.h         # Error number definitions
│   │   └── limits.h        # System limits
│   ├── atomic_arch.h       # Atomic operations for i286
│   ├── crt_arch.h          # CRT startup code
│   ├── kstat.h             # Kernel stat structure
│   ├── pthread_arch.h      # pthread architecture (stub)
│   ├── reloc.h             # Relocation support
│   └── syscall_arch.h      # Syscall interface definition
├── include/                # Public header files (subset of musl)
│   ├── stdarg.h            # Custom va_arg macro (codegen-friendly)
│   └── ...                 # Other standard headers
├── src/                    # Library source files (subset ported)
│   └── ...                 # Minimal set for current tests
└── port/                   # Porting infrastructure
    └── ...                 # Build scripts, patches

musl_port/                  # llvm-i286-specific porting helpers
├── printf_min.c            # Minimal printf implementation
└── ...                     # Other porting helpers
```

## Using the musl Headers

To compile C code that uses musl headers:

```bash
clang-15 -S -emit-llvm -m32 \
  -I./musl/include \
  -I./musl/arch/i286 \
  -ffreestanding \
  input.c -o output.ll
```

The `-ffreestanding` flag tells clang not to assume a hosted environment (no standard
library assumed).

### Example: Minimal printf test

```c
#include <stdarg.h>

extern long __os2_syscall3(long n, long a1, long a2, long a3);

int printf(const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    int x = va_arg(ap, int);
    // ... format and print x ...
    va_end(ap);
    return 0;
}
```

This compiles with the llvm-i286 codegen because:
1. `va_start` is lowered to inline codegen (no external `llvm_va_start` call)
2. `va_arg` uses standard C dereference (codegen handles SS-derived pointers)
3. `va_end` is a no-op (codegen skips it)

## Building the musl Library

### Current: Using musl's own build system

We now use musl's native build system with `clang_i286` as the compiler. This ensures
musl builds correctly with its own configuration and header dependencies.

```bash
cd musl
CC=../clang_i286 CFLAGS="-m32 -std=gnu99" ./configure \
  --target=i286-linux-gnu \
  --prefix=/tmp/musl-i286
make
```

This produces a complete `lib/libc.a` static library built with our toolchain.

### Why not our own build system?

- musl's build handles include paths, header generation, and platform configuration
- Ensures all musl internal dependencies are resolved correctly
- Our `clang_i286` wrapper is a drop-in clang replacement, works generically
- The wrapper supports the full pipeline: C → IR → NASM → .obj → .exe

### Wrapper scripts for build compatibility

When musl's build system calls external tools (ar, ranlib, etc.), we provide wrapper
scripts that map to OpenWatcom equivalents:

- `ar` → `wlib` (OpenWatcom library tool)
- `ranlib` → no-op (wlib handles indexing)
- `strip` → not needed (debug info preserved for td2ine)

These wrappers are generic and work with any project, not just musl.

## Current Test Coverage

Tests that use musl features:

| Test | musl Feature Used | Status |
|------|-------------------|--------|
| test_printf_companion | printf_min.c, va_start | Compiles, links, runtime crash |
| test_printf_simple | stdio.h printf | Expected to fail (known limitation) |

Most other tests use inline `output_helper.h` helpers instead of musl functions.

## Porting Status by Category

| Category | Status | Notes |
|----------|--------|-------|
| **Headers** | Mostly ported | Types, syscalls, atomic, CRT |
| **Stdio** | Minimal | printf_min.c only |
| **String** | Not ported | Tests use inline helpers |
| **Memory** | Not ported | No malloc/free |
| **File I/O** | Not ported | No fopen/fread/fwrite |
| **Math** | Not ported | No floating point support |
| **Time** | Not ported | No time/date functions |
| **Threads** | Not ported | No pthread support |
| **Locale** | Not ported | No locale support |

## Known Issues

1. **va_arg on i286**: The custom `va_arg` macro in `musl/include/stdarg.h` assumes
   that va_list pointers are SS-derived. This works because varargs are on the caller's
   stack frame, which is in SS. The codegen tracks this via `state.ssDerivedPtrVregs`.

2. **Printf formatting**: The minimal printf in `musl_port/printf_min.c` only handles
   `%d`, `%s`, `%%`, and literal characters. Complex format specifiers require additional
   porting work.

3. **Segmented memory**: The i286 segmented memory model requires special handling for
   far pointers (selector:offset pairs). The codegen handles this via SS-derived pointer
   tracking, but some musl code may not yet be compatible.

4. **Build system integration**: Musl's build system expects standard Unix tools (ar,
   ranlib, strip). We provide wrapper scripts that map to OpenWatcom equivalents (wlib,
   etc.). These wrappers are generic and work with any project, not just musl.

5. **POSIX API coverage**: Some POSIX system calls (readv, writev, etc.) don't have
   direct OS/2 equivalents. These are stubbed with -ENOSYS until proper OS/2 API
   implementations are added.

## Future Work

### Immediate (Phase 2 completion)

- ✅ Complete musl static library build (`lib/libc.a`)
- 🔄 Fix remaining musl source compatibility issues as they arise
- 🔄 Implement missing OS/2 syscalls for POSIX API (readv, writev, etc.)
- 🔄 Validate c-testsuite with built musl library

### Phase 3: Standard I/O

- Implement `fopen`/`fclose` via `DosOpen`/`DosClose`
- Implement `fread`/`fwrite` via `DosRead`/`DosWrite`
- Implement `printf`/`fprintf` with full format string support
- Implement `scanf`/`fscanf`
- Implement `fseek`/`ftell`/`fflush`

### Phase 4: Memory Allocation

- Implement `malloc`/`free`/`realloc`/`calloc` via `DosAllocSeg`/`DosFreeSeg`
- Integrate with musl's mallocng allocator

### Phase 5: Advanced Features

- Port more printf format specifiers (%f, %x, %ld, etc.)
- Add thread support (pthread)
- Add locale support
- Port math functions (sin, cos, sqrt, etc.)
- Add wide character support

## References

- musl libc: https://musl.libc.org/
- OS/2 1.x API: OS/2 Programmer's Reference
- 80286 architecture: Intel 80286 Processor Manual

## c-testsuite Integration

### Overview

The [c-testsuite](https://github.com/nickg/c-testsuite) is a comprehensive C compiler test suite with 220 test cases covering a broad range of C language features. We have integrated it into our test infrastructure to systematically validate musl porting progress and identify codegen issues.

### Running c-testsuite

```bash
# Run all c-testsuite tests
bash c_testsuite_run.sh

# Run a single test
./c-testsuite/runners/single-exec/llvm-i286 c-testsuite/tests/single-exec/00179.c
```

### Test Infrastructure

- **Runner script**: `c-testsuite/runners/single-exec/llvm-i286`
- **Skip list**: `c-testsuite/runners/single-exec/llvm-i286.skip`
- **Main runner**: `c_testsuite_run.sh`

The runner performs the full compilation pipeline:
1. C → LLVM IR (via `clang_i286`)
2. LLVM IR → NASM assembly (via `llvm-i286`)
3. Assembly → object file (via `nasm`)
4. Object file → OS/2 executable (via `wlink`)
5. Execute under `lx_loader` (OS/2 executable emulator)
6. Compare output against expected results

### Porting Plan

The c-testsuite integration drives a systematic porting plan organized into phases:

| Phase | Focus | Status | Target Tests Passing |
|-------|-------|--------|---------------------|
| **0** | Infrastructure (runner, skip lists) | ✅ Complete | 0 |
| **1** | Codegen fixes for musl compilation | ✅ Complete | 50-80 |
| **2** | Full musl build + OS/2 syscalls | 🔄 In Progress | 80-120 |
| **3** | c-testsuite validation + fixes | ⏳ Pending | 100-150 |
| **4** | Math functions | ⏳ Pending | 105-160 |
| **5** | Advanced features (threads, locale) | ⏳ Pending | 110-170+ |

### Current Status (Phase 2: Full musl build)

**Completed:**
- ✅ 143/143 musl functions compile cleanly (string, ctype, stdlib, exit, errno)
- ✅ `clang_i286` wrapper works as generic C compiler
- ✅ Musl's build system configured and progressing
- ✅ Codegen bugs fixed: addressing modes, NASM reserved words, GEP constants, inline asm

**In Progress:**
- 🔄 Completing full musl static library build (`lib/libc.a`)
- 🔄 Fixing remaining musl source compatibility issues (semctl, etc.)
- 🔄 Implementing missing OS/2 syscalls for POSIX API calls

**Not Yet Started:**
- ❌ Stdio implementation (fopen, fread, fwrite, fclose, printf, scanf)
- ❌ Memory allocation (malloc, free, realloc, calloc)
- ❌ Math functions (sin, cos, sqrt, etc.)
- ❌ Threads and locale support

### OS/2 API Reference

For implementing missing platform support (stdio, malloc, threads), we use:
- **Primary reference**: `os2_examples/OS2API.TXT` (28,000+ line OS/2 1.x API manual)
- **Syscall numbers**: `2ine_debugger/os2imports.pl`
- **Existing glue code**: `os2_glue/` (libc_init.c, stdinit.c, lockfile.c)
- **Runtime library**: `runtime/runtime.lib` (div32, mul32, os2_syscall, itoa, printnum)

### Key OS/2 APIs Used

| Category | OS/2 Function | Syscall # | musl Function |
|----------|---------------|-----------|---------------|
| **Stdout** | `DosWrite` | 81 | `printf`, `fprintf`, `fwrite` |
| **Stdin** | `DosRead` | 79 | `scanf`, `fscanf`, `fread` |
| **File open** | `DosOpen` | 70 | `fopen` |
| **File close** | `DosClose` | 59 | `fclose` |
| **File seek** | `DosChgFilePtr` | — | `fseek`, `ftell` |
| **Memory alloc** | `DosAllocSeg` | 34 | `malloc` |
| **Memory free** | `DosFreeSeg` | 39 | `free` |
| **Exit** | `DosExit` | 5 | `exit`, `_Exit` |

### Test Categories by c-testsuite Tags

| Tag | Count | Description |
|-----|-------|-------------|
| `c89` | 174 | Tests valid in C89/C90 (ANSI C) |
| `c99` | 43 | Tests requiring C99 features |
| `c11` | 2 | Tests requiring C11 features |
| `portable` | 218 | Tests that should work across all platforms |
| `needs-cpp` | 96 | Tests that require the C preprocessor |
| `needs-libc` | 63 | Tests that depend on the C standard library |

### References

- c-testsuite: https://github.com/nickg/c-testsuite
- OS/2 API documentation: `os2_examples/OS2API.TXT`
- musl libc: https://musl.libc.org/
- 80286 architecture: Intel 80286 Processor Manual
