# musl libc Port to llvm-i286 (OS/2 1.x)

This directory contains the initial port of musl libc to the llvm-i286 code generator, targeting OS/2 1.x in 16-bit protected mode.

## Overview

This port enables C programs compiled with the llvm-i286 backend to use the musl C library on OS/2 1.x systems running on 80286 processors.

### Key Features

- **32-bit data model**: int=32-bit, short=16-bit, long=32-bit, pointer=32-bit (all emulated on 16-bit 80286)
- **Single 64K segment**: All data fits within a single 64K DGROUP segment initially
- **C-only implementation**: No architecture-specific assembly (uses 80287 FPU instructions where needed)
- **OS/2 API integration**: Syscalls mapped to OS/2 Dos* API calls
- **Static linking only**: No dynamic linking support in initial port

## Directory Structure

```
musl/
├── arch/i286/              # Architecture-specific files for i286
│   ├── bits/               # Header files for types, syscalls, etc.
│   │   ├── alltypes.h.in   # Type definitions (32-bit model)
│   │   ├── syscall.h.in    # OS/2 syscall numbers
│   │   ├── signal.h        # Signal definitions
│   │   ├── fcntl.h         # File control flags
│   │   ├── float.h         # FPU constants
│   │   ├── limits.h        # System limits
│   │   ├── mman.h          # Memory mapping
│   │   ├── stat.h          # File stat structure
│   │   ├── setjmp.h        # Setjmp/longjmp
│   │   ├── termios.h       # Terminal settings
│   │   ├── poll.h          # Poll definitions
│   │   ├── errno.h         # Error codes
│   │   ├── user.h          # User structure
│   │   └── io.h            # I/O port access
│   ├── atomic_arch.h       # Atomic operations (C-only)
│   ├── syscall_arch.h      # Syscall interface
│   ├── crt_arch.h          # C runtime startup
│   ├── pthread_arch.h      # Thread pointer (stub)
│   ├── reloc.h             # Relocation types (stub)
│   └── kstat.h             # Kernel stat structure
├── src/
│   ├── string/i286/        # String functions (C implementations)
│   │   ├── memcpy.c
│   │   ├── memmove.c
│   │   └── memset.c
│   ├── fenv/i286/          # FPU environment (80287 instructions)
│   │   └── fenv.c
│   ├── setjmp/i286/        # Setjmp/longjmp
│   │   ├── setjmp.c
│   │   └── longjmp.c
│   ├── thread/i286/        # Thread stubs (single-threaded initially)
│   │   ├── clone.c
│   │   ├── tls.c
│   │   ├── __set_thread_area.c
│   │   ├── __unmapself.c
│   │   └── syscall_cp.c
│   └── signal/i286/        # Signal stubs
│       ├── restore.c
│       └── sigsetjmp.c
```

## Building

### Prerequisites

1. **LLVM/Clang** with i286 target support
2. **llvm-i286** code generator (built from parent directory)
3. **NASM** assembler
4. **OpenWatcom** wlink linker (for OS/2 NE executables)

### Quick Start

```bash
# 1. Configure musl for i286 target
./configure_i286.sh

# 2. Build musl
./build_musl.sh
```

### Manual Build

```bash
# Configure
cd musl
cat > config.mak << 'EOF'
ARCH = i286
CC = clang -target i286-unknown-os2
CFLAGS_C99FSE = -std=c99 -ffreestanding -nostdinc
CFLAGS_AUTO = -Os -pipe
shared = no
static = yes
EOF

# Build
make CC=path/to/musl_cc.sh
```

## Build Pipeline

The build process for each C source file:

```
source.c
  → clang -target i286-unknown-os2 -S -emit-llvm
  → source.ll (LLVM IR)
  → llvm-i286 source.ll -o source.asm
  → source.asm (NASM assembly)
  → nasm -f obj -o source.o source.asm
  → source.o (OS/2 object file)
```

## Architecture

### Data Model

The i286 target uses a 32-bit data model emulated on the 16-bit 80286:

| Type | Size | Notes |
|------|------|-------|
| `char` | 8-bit | Native |
| `short` | 16-bit | Native |
| `int` | 32-bit | Emulated (DX:AX) |
| `long` | 32-bit | Emulated (DX:AX) |
| `long long` | 64-bit | Emulated (future) |
| `pointer` | 32-bit | Emulated (DX:AX), low 16 bits used initially |
| `size_t` | 32-bit | Emulated |
| `float` | 32-bit | 80287 FPU |
| `double` | 64-bit | 80287 FPU |
| `long double` | 80-bit | 80287 FPU native |

### Memory Model

**Initial (single 64K segment)**:
- All data in DGROUP segment
- DS = ES = SS = DGROUP
- 32-bit pointers: only low 16 bits used as offset within segment
- Stack, data, BSS all in same segment

**Future (linear-to-segment emulation)**:
- 32-bit pointers = segment:offset pair
- Memory access via segment override prefixes
- Stack management via SS:SP pair

### Syscall Interface

musl's `syscall()` interface is mapped to OS/2 Dos* API calls:

| musl Syscall | OS/2 API |
|--------------|----------|
| `read` | DosRead |
| `write` | DosWrite |
| `open` | DosOpen |
| `close` | DosClose |
| `lseek` | DosChgFilePtr |
| `exit` | DosExit |
| `getpid` | DosGetPID |
| `sleep` | DosSleep |
| `chdir` | DosChDir |
| `unlink` | DosDelete |
| `mkdir` | DosMkDir |
| `rmdir` | DosRmDir |
| `rename` | DosMove |

### FPU Support

The 80287 FPU is required (no software emulation):
- All floating-point operations use FPU instructions
- FPU register stack managed by code generator
- Supports float (32-bit), double (64-bit), long double (80-bit)

## Limitations (Initial Port)

### Not Implemented

- **Threads**: Single-threaded only, thread functions return ENOSYS
- **Signals**: No signal handling
- **Dynamic linking**: Static linking only
- **Sockets**: No network support
- **mmap**: Uses DosAllocSeg/DosFreeSeg (simplified)
- **64-bit long long**: Not fully implemented
- **Linear-to-segment**: 64K segment limit

### Work in Progress

- File I/O (open, stat, fcntl)
- Time functions
- Complete syscall mapping
- Runtime library optimization

## Testing

### Basic Test

```c
#include <stdio.h>

int main() {
    printf("Hello from musl on OS/2 1.x!\n");
    return 0;
}
```

Compile and run:
```bash
clang -target i286-unknown-os2 -std=c99 -ffreestanding -nostdinc \
      -I musl/include -I musl/arch/i286 -I musl/arch/generic \
      -o test.ll -S -emit-llvm test.c

llvm-i286 test.ll -o test.asm

nasm -f obj -o test.o test.asm

wlink system os2 d all path ~/ow/open-watcom-v2/rel/lib286:~/ow/open-watcom-v2/lib286/os2 \
      library ~/ow/open-watcom-v2/lib286/os2/os2.lib \
      library build/runtime/runtime.lib \
      name test.exe file test.o
```

## Future Enhancements

1. **Multi-segment support**: Linear-to-segment emulation for >64K programs
2. **Thread support**: OS/2 thread API integration
3. **Signal handling**: OS/2 exception handling
4. **Dynamic linking**: NE format dynamic linker
5. **64-bit arithmetic**: Complete long long support
6. **Optimized runtime**: Hand-optimized 32-bit arithmetic routines
7. **Extended syscall support**: Complete OS/2 API coverage

## References

- [musl libc](https://musl.libc.org/)
- [OS/2 Programming Reference](os2_examples/OS2API.TXT)
- [80286 Programmer's Reference](https://en.wikipedia.org/wiki/Intel_80286)
- [NASM Documentation](https://nasm.us/doc/)

## License

musl libc is licensed under the MIT license. See the musl source tree for details.

This port (architecture-specific files and build scripts) is provided under the same license as the llvm-i286 project.
