# Musl printf Porting Plan - Practical Approach

## Goal
Port musl's `printf()` with proper OS/2 1.x integration, using OS/2 threading primitives for file locking.

## Test Case
```c
// tests/os2/test_printf.c (existing)
#include <stdio.h>
int main() {
    int x = 42;
    printf("Hello, world! x = %d\n", x);
    return 0;
}
```
Expected output: `Hello, world! x = 42\n`

## Strategy

Rather than trying to compile musl's entire internal header chain (which pulls in pthreads, atomic ops, etc.), we will:

1. **Create OS/2-specific glue files** that provide the minimal infrastructure musl needs
2. **Use OS/2 1.x semaphore API** for file locking (proper implementation)
3. **Port musl's actual printf/vfprintf** with only minimal modifications for OS/2
4. **Build as companion files** linked with test programs

This approach:
- ✅ Actually ports musl code (not standalone implementation)
- ✅ Uses OS/2 native APIs (semaphores for locking)
- ✅ Minimizes header dependency issues
- ✅ Can be extended later for full musl port

## Files to Create

### 1. os2_glue/lockfile.c
**Purpose:** File locking using OS/2 semaphores

**OS/2 API Used:**
- `DosCreateSem()` - Create semaphore for file locking
- `DosSemRequest()` - Wait on semaphore (lock)
- `DosSemClear()` - Clear semaphore (unlock)
- `DosCloseSem()` - Close semaphore handle

**Implementation:**
```c
#include "stdio_impl.h"

// OS/2 semaphore-based file locking
// Each FILE object gets a semaphore for thread safety

hidden int __lockfile(FILE *f)
{
    if (f->lock >= 0) {
        // Semaphore-based locking
        // f->lock contains semaphore handle
        DosSemRequest((HSEM)f->lock);
        return 1; // indicate we locked it
    }
    return 0; // no locking needed
}

hidden void __unlockfile(FILE *f)
{
    if (f->lock >= 0) {
        DosSemClear((HSEM)f->lock);
    }
}
```

**Why this works:**
- OS/2 1.x provides semaphores via `DosCreateSem`/`DosSemRequest`/`DosSemClear`
- Matches musl's expected interface (`__lockfile` returns int, `__unlockfile` is void)
- Proper thread safety for multi-threaded programs

### 2. os2_glue/stdinit.c
**Purpose:** Initialize stdout FILE object for OS/2

**Implementation:**
```c
#include "stdio_impl.h"

// OS/2 write syscall wrapper
static size_t os2_stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
    // Use our existing syscall layer
    // __os2_syscall3(SYS_write, fd=1, buf, len)
    return __os2_syscall3(2, 1, (long)buf, (long)len);
}

// Static FILE object for stdout
// Initialized with OS/2-specific callbacks
static FILE stdout_file = {
    .flags = 0,
    .rpos = 0,
    .rend = 0,
    .close = 0,
    .wend = 0,
    .wpos = 0,
    .mustbezero_1 = 0,
    .wbase = 0,
    .read = 0,
    .write = os2_stdout_write,
    .seek = 0,
    .buf = 0,
    .buf_size = 0,
    .prev = 0,
    .next = 0,
    .fd = 1,  // OS/2 file descriptor for stdout
    .pipe_pid = 0,
    .lockcount = 0,
    .mode = 0,
    .lock = -1,  // No locking by default (single-threaded)
    .lbf = 0,
    .cookie = 0,
    .off = 0,
    .getln_buf = 0,
    .mustbezero_2 = 0,
    .shend = 0,
    .shlim = 0,
    .shcnt = 0,
    .prev_locked = 0,
    .next_locked = 0,
    .locale = 0,
};

// Export the stdout pointer (musl expects this)
hidden FILE *volatile __stdout_used = &stdout_file;
```

**Why this works:**
- Provides the `FILE` structure musl expects
- Sets up write callback to use our OS/2 syscall layer
- File descriptor 1 = stdout in OS/2
- Lock = -1 means no locking (safe for single-threaded use)
- Can be extended later for multi-threaded locking

### 3. os2_glue/libc_init.c
**Purpose:** Initialize __libc global

**Implementation:**
```c
#include "libc.h"

// Define the global __libc structure
struct __libc __libc;

// OS/2-specific fields
size_t __hwcap = 0;
char *__progname = 0;
char *__progname_full = 0;
```

**Why this works:**
- Provides the `__libc` global musl expects
- Minimal initialization (can be extended later)

## Files to Port from musl (unmodified)

### 1. musl/src/stdio/printf.c
**Lines:** ~15
**Changes needed:** None

**Content:**
```c
#include <stdio.h>
#include <stdarg.h>

int printf(const char *restrict fmt, ...)
{
    int ret;
    va_list ap;
    va_start(ap, fmt);
    ret = vfprintf(stdout, fmt, ap);
    va_end(ap);
    return ret;
}
```

### 2. musl/src/stdio/vfprintf.c
**Lines:** ~703
**Changes needed:** None (uses musl's stdio_impl.h, our os2_glue provides the infrastructure)

**Key functions:**
- `printf_core()` - Main formatting logic
- `vfprintf()` - Entry point, unpacks va_list
- Helper functions for integer formatting, padding, etc.

### 3. musl/src/stdio/__fwritex.c
**Lines:** ~15
**Changes needed:** None

**Content:**
```c
#include "stdio_impl.h"

hidden size_t __fwritex(const unsigned char *s, size_t l, FILE *f)
{
    if (!ferror(f)) f->write(f, s, l);
    return l;
}
```

### 4. musl/src/stdio/__overflow.c
**Lines:** ~25
**Changes needed:** None

**Content:**
```c
#include "stdio_impl.h"

int __overflow(FILE *f, int c)
{
    // Flush buffer, write single character
    // Calls f->write callback
}
```

### 5. musl/src/stdio/__stdio_exit.c
**Lines:** ~10
**Changes needed:** None

**Content:**
```c
#include "stdio_impl.h"

hidden void __stdio_exit(void)
{
    // Flush all streams on exit
}
```

## Build Integration

### Create os2_glue/ directory
```bash
mkdir -p os2_glue
```

### Build script for test_printf
```bash
#!/bin/bash
# build_test_printf.sh

GLUE_DIR="os2_glue"

clang_i286 \
    tests/os2/test_printf.c \
    "$GLUE_DIR/lockfile.c" \
    "$GLUE_DIR/stdinit.c" \
    "$GLUE_DIR/libc_init.c" \
    musl/src/stdio/printf.c \
    musl/src/stdio/vfprintf.c \
    musl/src/stdio/__fwritex.c \
    musl/src/stdio/__overflow.c \
    musl/src/stdio/__stdio_exit.c \
    -I musl/src/include \
    -I musl/src \
    -I musl/src/internal \
    -I musl/include \
    -I musl/arch/i286 \
    -I musl/arch/generic \
    -o tests/output/test_printf.exe
```

### Alternative: Build as static library
```bash
# Build os2_glue objects
clang_i286 -c os2_glue/lockfile.c -o os2_glue/lockfile.o
clang_i286 -c os2_glue/stdinit.c -o os2_glue/stdinit.o
clang_i286 -c os2_glue/libc_init.c -o os2_glue/libc_init.o

# Build musl objects
for f in musl/src/stdio/printf.c musl/src/stdio/vfprintf.c \
         musl/src/stdio/__fwritex.c musl/src/stdio/__overflow.c \
         musl/src/stdio/__stdio_exit.c; do
    clang_i286 -c "$f" -o "${f%.c}.o"
done

# Link test
clang_i286 tests/os2/test_printf.c \
    os2_glue/lockfile.o \
    os2_glue/stdinit.o \
    os2_glue/libc_init.o \
    musl/src/stdio/printf.o \
    musl/src/stdio/vfprintf.o \
    musl/src/stdio/__fwritex.o \
    musl/src/stdio/__overflow.o \
    musl/src/stdio/__stdio_exit.o \
    -o tests/output/test_printf.exe
```

## Codegen Features Required

Based on musl's printf implementation, the codegen must support:

### 1. Struct access (GEP)
**Used in:** FILE->flags, FILE->wpos, FILE->write, etc.
**Status:** Partially implemented (works for simple arrays, needs struct field support)

**Test case:**
```c
// tests/os2/test_struct_field.c
struct file {
    int fd;
    int flags;
    char *buf;
};

int main() {
    struct file f;
    f.fd = 1;
    f.flags = 0;
    return f.fd;
}
```

### 2. Function pointer calls
**Used in:** `f->write(f, buf, len)` in __fwritex.c
**Status:** Not implemented

**Test case:**
```c
// tests/os2/test_func_ptr.c
typedef int (*writer_t)(int, const char *, int);

int my_write(int fd, const char *buf, int len) {
    return len; // stub
}

int main() {
    writer_t w = my_write;
    return w(1, "hello", 5);
}
```

### 3. Switch statements
**Used in:** printf_core() state machine
**Status:** Partially implemented (basic switch works)

**Test case:**
```c
// tests/os2/test_switch.c
int main() {
    int x = 2;
    switch (x) {
        case 0: return 10;
        case 1: return 20;
        case 2: return 30;
        default: return 0;
    }
}
```

### 4. Variadic functions (va_list)
**Used in:** printf(), vfprintf()
**Status:** Not implemented

**Test case:**
```c
// tests/os2/test_variadic.c
#include <stdarg.h>

int sum(int count, ...) {
    va_list ap;
    va_start(ap, count);
    int total = 0;
    for (int i = 0; i < count; i++) {
        total += va_arg(ap, int);
    }
    va_end(ap);
    return total;
}

int main() {
    return sum(3, 10, 20, 30);
}
```

### 5. Union access
**Used in:** `union arg` in vfprintf.c
**Status:** Not implemented

**Test case:**
```c
// tests/os2/test_union.c
union data {
    int i;
    long l;
    void *p;
};

int main() {
    union data d;
    d.i = 42;
    return d.i;
}
```

### 6. Complex control flow
**Used in:** printf_core() has many branches, loops, early returns
**Status:** Partially implemented (basic branches work)

**Test case:**
```c
// tests/os2/test_complex_flow.c
int test(int x) {
    if (x > 0) {
        for (int i = 0; i < x; i++) {
            if (i == 5) return i;
        }
        return x;
    } else if (x < 0) {
        return -1;
    }
    return 0;
}

int main() {
    return test(10);
}
```

## Implementation Plan

### Phase 1: Create OS/2 glue (1 day)
1. Create `os2_glue/` directory
2. Implement `lockfile.c` with OS/2 semaphore API
3. Implement `stdinit.c` with stdout initialization
4. Implement `libc_init.c` with __libc global

### Phase 2: Test codegen features (2 days)
1. Create test cases for each required codegen feature
2. Run tests and identify failures
3. Fix codegen issues:
   - Struct field access (GEP)
   - Function pointer calls
   - Switch statements
   - Variadic functions
   - Union access
   - Complex control flow

### Phase 3: Port musl files (1 day)
1. Copy musl's printf.c, vfprintf.c, __fwritex.c, __overflow.c, __stdio_exit.c
2. Attempt to compile each through our pipeline
3. Fix any compilation issues (header includes, etc.)

### Phase 4: Build and test (1 day)
1. Link test_printf.exe with all components
2. Run under lx_loader
3. Verify output: `Hello, world! x = 42\n`

### Phase 5: Cleanup and documentation (1 day)
1. Update PLAN_MUSL_NEXT.md with progress
2. Document OS/2 glue layer
3. Create build scripts

**Total estimated time:** 6 days

## Risk Assessment

### High risk:
- **Variadic functions:** va_list handling may require significant codegen work
- **Function pointers:** Calling through pointer may have calling convention issues
- **vfprintf.c complexity:** 703 lines, may expose many codegen bugs

### Medium risk:
- **Struct access:** GEP for nested structs may not work correctly
- **Large translation units:** vfprintf.c may hit codegen limits

### Low risk:
- **OS/2 glue:** Simple C code, well-understood API
- **Write syscall:** Already working
- **Locking:** Simple semaphore API calls

## Success Criteria

1. ✅ `test_printf.c` compiles and links
2. ✅ Runs under lx_loader
3. ✅ Outputs: `Hello, world! x = 42\n`
4. ✅ Uses musl's actual printf implementation
5. ✅ Uses OS/2 semaphore API for locking (proper implementation)
6. ✅ No codegen errors in musl source files

## Future Work (After printf works)

1. **Full stdio:** Port fopen, fclose, fread, fwrite, etc.
2. **Memory allocation:** Port malloc, free, etc.
3. **String functions:** Port strlen, strcpy, etc. (already have standalone tests)
4. **Exit handling:** Port exit, atexit, etc.
5. **Full libc build:** Integrate into musl's build system

## Next Steps

1. Create `os2_glue/` directory
2. Implement `os2_glue/lockfile.c` with OS/2 semaphore API
3. Implement `os2_glue/stdinit.c` with stdout initialization
4. Implement `os2_glue/libc_init.c` with __libc global
5. Create test cases for required codegen features
6. Start fixing codegen issues
7. Port musl printf files
8. Build and test

---

**Status:** Plan created, ready to begin Phase 1
**Date:** 2026-07-08
