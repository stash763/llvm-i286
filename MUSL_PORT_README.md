# musl libc Port to llvm-i286 (OS/2 1.x)

## Overview

This directory contains the partial port of musl libc to the llvm-i286 code generator,
targeting OS/2 1.x in 16-bit protected mode on Intel 80286 processors.

## Current Status

### Working

- **Header files**: Type definitions, syscall numbers, CRT startup for i286 architecture
  - `musl/arch/i286/bits/alltypes.h` — 32-bit data model types (int=32, long=32, ptr=32)
  - `musl/arch/i286/syscall_arch.h` — OS/2 syscall numbers
  - `musl/arch/i286/atomic_arch.h` — Atomic operations for i286
  - `musl/arch/i286/crt_arch.h` — CRT startup code
  - `musl/include/stdarg.h` — Custom `va_arg` macro (no inline asm, uses codegen SS-derived pointers)
- **Syscall dispatch**: `__os2_syscall3` for read/write/exit syscalls
- **printf_min**: Minimal printf with `%d`, `%s`, `%%`, and literal characters
- **va_start inline**: Codegen generates inline `llvm.va.start` (no external runtime call)

### Limitations

- **No file I/O**: fopen/fread/fwrite/fclose not ported
- **No malloc/free**: Memory allocation not supported
- **No string functions**: strlen/strcmp/memcpy not ported (tests use inline helpers)
- **Limited printf**: Only `%d`, `%s`, `%%` supported (no `%f`, `%x`, `%ld`, etc.)
- **No threads**: No pthread support
- **No locale**: No locale support
- **No wchar**: Wide character support not implemented

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

The musl library is not yet built as a standalone static library for llvm-i286.
Currently, only selected functions are compiled inline with user code.

### Future: Full musl build

To build a full musl static library for llvm-i286:

```bash
# Configure musl for i286 target
./musl/configure \
  --target=i386-pc-linux-gnu \
  --host=i386-pc-linux-gnu \
  --prefix=/opt/musl-i286 \
  CC=clang-15 \
  CFLAGS="-m32 -ffreestanding"
```

This is not yet implemented. Current tests use inline helpers from
`tests/os2/output_helper.h` instead.

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

3. **No standard library**: There is no built musl static library. Tests that need
   string/number formatting use inline helpers from `tests/os2/output_helper.h`.

4. **Segmented memory**: The i286 segmented memory model requires special handling for
   far pointers (selector:offset pairs). The codegen handles this via SS-derived pointer
   tracking, but some musl code may not yet be compatible.

## Future Work

- Port more string functions (strlen, strcmp, memcpy, memset)
- Port file I/O functions (fopen, fread, fwrite, fclose)
- Port memory allocation (malloc, free, realloc)
- Port more printf format specifiers (%f, %x, %ld, etc.)
- Build a full musl static library for llvm-i286
- Add thread support (pthread)
- Add locale support

## References

- musl libc: https://musl.libc.org/
- OS/2 1.x API: OS/2 Programmer's Reference
- 80286 architecture: Intel 80286 Processor Manual
