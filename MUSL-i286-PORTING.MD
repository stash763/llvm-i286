Based on my thorough review of the codebase and your clarifications, here is the comprehensive plan:
Plan: Initial Port of musl libc to llvm_i286 (OS/2 1.x, 16-bit Protected Mode)
Overview
This plan ports musl libc to the llvm_i286 target, targeting OS/2 1.x in 16-bit protected mode. The data model matches i386 defaults (int=32-bit, short=16-bit, long=32-bit, pointer=32-bit), with all 32-bit operations emulated via 16-bit 80286 instructions. The initial port uses a single 64K data segment, designed to be extended to linear-to-segment emulation in the future. Only C implementations of musl are used—no arch-specific assembly.
Phase 1: Code Generator Enhancements (llvm_i286)
The current code generator (src/codegen/InstructionSelect.cpp, Emitter.cpp, CodeGen.cpp) supports basic 16-bit operations with partial 32-bit emulation (add, sub, mul). It must be significantly enhanced to compile musl's C code.
1.1 Comprehensive 32-Bit Emulation
Problem: musl assumes 32-bit int, long, size_t, ssize_t, pointers, etc. The 80286 is a 16-bit CPU—all 32-bit operations must be emulated with 16-bit instruction sequences.
Tasks:
- 32-bit arithmetic: Complete the existing 32-bit add/sub (partially done), implement 32-bit mul (improve existing), div, udiv, sdiv, rem, urem, srem using multi-word sequences
- 32-bit bitwise: and, or, xor using two 16-bit operations on low/high words
- 32-bit shifts: shl, lshr, ashr using CL loop or immediate shifts with carry propagation
- 32-bit comparisons: icmp eq/ne/slt/sgt/sle/sge/ult/ugt/ule/uge using CMP + conditional jumps on both words
- 32-bit load/store: Load/store 32-bit values from memory (two 16-bit loads/stores to consecutive addresses)
- 32-bit GEP: getelementptr with 32-bit indices and 32-bit pointer arithmetic
- 32-bit function arguments: Push 32-bit values (high word then low word) on stack
- 32-bit return values: Return in DX:AX (DX=high word, AX=low word)
- 32-bit alloca: Allocate 4 bytes on stack
- 32-bit constants: Load 32-bit constants (two immediate loads)
Files to modify:
- src/codegen/InstructionSelect.cpp - Add 32-bit variants for all operations
- src/codegen/CodeGen.cpp - Handle 32-bit type detection from LLVM IR
- src/codegen/Emitter.cpp - Emit 32-bit-aware NASM
- src/ir/IrAst.h - Ensure type system supports 32-bit types properly
Runtime helpers (in src/runtime/):
- _MultiplyI32 (exists, needs verification)
- _DivideI32 (exists, needs verification)
- _DivideU32, _MultiplyU32 (unsigned variants)
- _ShiftL32, _ShiftR32, _ShiftAr32 (shift helpers)
- _Cmp32 (32-bit comparison helper)
1.2 Missing LLVM IR Instructions
Currently supported: Add, Sub, Mul, And, Or, Xor, Shl, LShr, AShr, Alloca, Store, Load, Br, Ret, Call, ICmp, CondBr, Trunc, ZExt, SExt, GetElementPtr, Select
Missing instructions needed for musl:
- Switch (switch): Jump table or chained comparisons
- Phi (phi): SSA phi nodes → register allocation at predecessor blocks
- Unreachable (unreachable): Emit a halt or trap instruction
- BitCast (bitcast): No-op for same-size types
- PtrToInt (ptrtoint): Convert pointer to int (no-op for 32-bit)
- IntToPtr (inttoptr): Convert int to pointer (no-op for 32-bit)
- SDiv/UDiv/SRem/URem (sdiv, udiv, srem, urem): 16-bit and 32-bit division
- FAdd/FSub/FMul/FDiv/FRem (fadd, fsub, fmul, fdiv, frem): 80287 FPU operations
- FPTrunc/FPExt/FPToUI/FPToSI/UIToFP/SIToFP (fptrunc, fpext, fptoui, fptosi, uitofp, sitofp): FPU conversions
- FCmp (fcmp): FPU comparisons (fcom, fcomi)
- AtomicRMW/CmpXchg (atomicrmw, cmpxchg): Atomic operations (lock prefix)
- Invoke (invoke): Exception handling (can stub for initial port)
- VAArg (va_arg): Variadic arguments (needed for printf/sprintf)
- ExtractValue/InsertValue (extractvalue, insertvalue): Struct access
- Freeze (freeze): No-op
Files to modify:
- src/codegen/InstructionSelect.cpp - Implement each new opcode
- src/ir/IrAst.h - Ensure opcodes are defined (most are)
- src/ir/IrVisitor.cpp - Ensure parsing of these instructions
1.3 Segment:Offset Addressing & Memory Model
Problem: With 32-bit pointers and a 64K segment limit, we need a memory model that works now but can be extended later.
Initial model (single 64K segment):
- All data lives in a single 64K DGROUP segment (_DATA + _BSS)
- 32-bit pointers stored as 32-bit values, but only the low 16 bits are used as offsets within the segment
- DS register points to DGROUP and is never changed during normal operation
- ES = DS for data operations
- SS = DS for stack operations (stack is in the same segment)
- CS is separate for code
Future linear-to-segment emulation (design for, don't implement yet):
- 32-bit pointers = segment:offset pair (16:16)
- Memory access: load segment register from high word, load offset from low word, use segment override
- Stack: 32-bit SP emulated with SS:SP pair
- near pointers (16-bit) vs far pointers (32-bit)
Tasks:
- Define memory model in src/target/x86_286.h / Addressing.h
- Ensure all 32-bit memory accesses use [bp+offset] or [bx+offset] with DS segment
- Design Addressing class to track segment assignments (future: segment override prefixes)
- Ensure stack frame layout handles 32-bit values (4-byte slots)
Files to modify:
- src/codegen/Addressing.h / Addressing.cpp - Segment:offset tracking
- src/target/x86_286.h / x86_286.cpp - Memory model definitions
- src/codegen/InstructionSelect.cpp - Use Addressing for memory ops
1.4 Global Variables and Data Segments
Problem: musl has many global variables (errno, environ, stdin/stdout/stderr, etc.) that need proper _DATA and _BSS segment placement.
Tasks:
- Emit initialized globals in _DATA segment with proper 32-bit values
- Emit uninitialized globals in _BSS segment with resw/resd directives
- Handle string constants (already partially done in CodeGen.cpp)
- Handle extern declarations for OS/2 API calls and runtime helpers
- Handle common symbols (tentative definitions → BSS)
- Support global string constants with proper 32-bit pointer layout
Files to modify:
- src/codegen/CodeGen.cpp - Global variable handling
- src/codegen/Emitter.cpp - BSS segment emission, extern declarations
1.5 Floating Point Support (80287 FPU)
Problem: musl's math library and some core functions use floating point. The 80287 FPU is required (per NOTES.MD: "We will require an FPU and will not be supporting software emulation of one").
Tasks:
- FPU register stack management (ST(0)-ST(7))
- FPU arithmetic: fadd, fsub, fmul, fdiv, frem
- FPU comparisons: fcom, fcomi, fcompp, conditional jumps via fstsw + sahf
- FPU load/store: fld, fst, fstp, fild, fist, fistp
- FPU constants: fldz, fld1, fldpi
- FPU transcendental: fsin, fcos, fsqrt, fprem (if needed)
- FPU control: fnstcw, fldcw, fnstsw, fclex
- Convert between FPU and integer: fild (int→float), fistp (float→int)
- Handle double (64-bit) and float (32-bit) and long double (80-bit)
Files to modify:
- src/codegen/InstructionSelect.cpp - FPU instruction selection
- src/target/x86_286.h / x86_286.cpp - FPU instruction definitions
1.6 Improved Calling Convention
Current: Parameters on stack right-to-left, caller cleans, return in AX. BX/SI/DI/BP callee-saved, AX/CX/DX caller-saved.
Needed for 32-bit model:
- 32-bit arguments: push high word then low word (4 bytes each)
- 32-bit return: DX:AX (DX=high, AX=low)
- Stack alignment: 2-byte (16-bit) for now, 4-byte if needed
- Struct returns: pointer to return struct passed as hidden first parameter
- Variadic arguments: va_list as pointer to stack arguments
Files to modify:
- src/target/CallingConv.h / CallingConv.cpp - Calling convention definitions
- src/codegen/InstructionSelect.cpp - Call/return handling
1.7 Function Prologue/Epilogue Improvements
Current: Fixed prologue (push bp/bx/si/di), fixed epilogue (pop di/si/bx/bp, ret)
Needed:
- Proper stack frame with local variable allocation
- Adjust SP for local variables (sub sp, N after prologue)
- Restore SP before epilogue (add sp, N before pop)
- 32-bit local variables (4-byte stack slots)
- Address locals via [bp-N] with correct offsets for 32-bit values
Files to modify:
- src/codegen/InstructionSelect.cpp - Prologue/epilogue in lowerFunction
- src/codegen/Emitter.cpp - Function emission
Phase 2: musl arch/i286/ Directory
Create a new architecture directory musl/arch/i286/ with all required files, adapted for OS/2 1.x 16-bit protected mode.
2.1 arch.mak
# No compat source dirs for initial port
COMPAT_SRC_DIRS =
2.2 bits/alltypes.h.in
Defines the type model. Since we're matching i386 defaults (32-bit int, 32-bit long, 32-bit pointer):
#define _REDIR_TIME64 0
#define _Addr int           /* 32-bit address type */
#define _Int64 long long   /* 64-bit integer (emulated) */
#define _Reg int           /* 32-bit register type */

#define __BYTE_ORDER 1234  /* Little-endian */
#define __LONG_MAX 0x7fffffffL

/* wchar_t, float_t, double_t, max_align_t */
Key: _Addr = int (32-bit) means size_t, uintptr_t, ptrdiff_t, ssize_t are all 32-bit. This matches i386 but requires 32-bit emulation on the 80286.
2.3 bits/fenv.h
80287 FPU environment:
#define FE_INVALID    1
#define FE_DIVBYZERO  4
#define FE_OVERFLOW   8
#define FE_UNDERFLOW  16
#define FE_INEXACT    32
#define FE_ALL_EXCEPT 63
#define FE_TONEAREST  0
#define FE_DOWNWARD   0x400
#define FE_UPWARD     0x800
#define FE_TOWARDZERO 0xc00
typedef unsigned short fexcept_t;
typedef struct { /* 80287 FPU state */ } fenv_t;
#define FE_DFL_ENV ((const fenv_t *)-1)
2.4 bits/float.h
80287 FPU supports extended precision (80-bit long double):
#define FLT_EVAL_METHOD 2  /* long double */
#define LDBL_MANT_DIG 64
#define LDBL_MIN_EXP (-16381)
#define LDBL_MAX_EXP 16384
/* ... */
2.5 bits/limits.h
#define PAGESIZE 4096  /* OS/2 page size */
2.6 bits/signal.h
OS/2 1.x signal definitions. OS/2 uses a different signal model than Unix. For the initial port, define minimal signals:
/* OS/2 signals - mapped to OS/2 exception/error codes */
#define SIGHUP    1
#define SIGINT    2
/* ... map to OS/2 signal equivalents */
#define _NSIG 65  /* Keep musl's signal count */
2.7 bits/stat.h
OS/2 file stat structure (using DosFindFirst/DosQFileInfo):
struct stat {
    /* OS/2-specific file info structure */
    /* Map to musl's stat structure */
};
2.8 bits/syscall.h.in
OS/2 API call numbers (not Linux syscall numbers). Map each musl syscall to an OS/2 Dos* function:
#define __NR_read       1   /* DosRead */
#define __NR_write      2   /* DosWrite */
#define __NR_open       3   /* DosOpen */
#define __NR_close      4   /* DosClose */
#define __NR_lseek      5   /* DosChgFilePtr */
#define __NR_exit       6   /* DosExit */
#define __NR_getpid     7   /* DosGetPID */
/* ... etc. */
2.9 bits/errno.h
OS/2 error codes mapped to errno values:
/* Map OS/2 error codes to errno */
#define EERROR_...  /* OS/2 errors */
2.10 bits/fcntl.h
#define O_RDONLY   0
#define O_WRONLY   1
#define O_RDWR    2
#define O_CREAT  0x100
#define O_EXCL  0x200
#define O_TRUNC 0x400
/* ... OS/2 open flags */
2.11 bits/mman.h
OS/2 memory mapping (DosAllocSeg/DosAllocHuge):
#define PROT_NONE   0
#define PROT_READ   1
#define PROT_WRITE  2
#define PROT_EXEC   4
#define MAP_SHARED  ... 
#define MAP_PRIVATE ...
#define MAP_ANON    ...
2.12 Other bits/ headers
- bits/dirent.h - OS/2 directory entry structure
- bits/termios.h - OS/2 terminal settings (minimal)
- bits/poll.h - OS/2 poll (minimal)
- bits/resource.h - OS/2 resource limits
- bits/setjmp.h - Setjmp/longjmp for 80286
- bits/user.h - User structure (minimal)
- bits/ioctl.h - OS/2 I/O control (minimal)
- bits/socket.h - Sockets (stub/minimal)
- bits/sem.h, bits/shm.h, bits/msg.h - IPC (stub/minimal)
- bits/ptrace.h - Ptrace (stub)
- bits/termios.h - Terminal
- bits/vt.h - Virtual terminal (stub)
- bits/kd.h - KD (stub)
- bits/soundcard.h - Soundcard (stub)
- bits/hwcap.h - Hardware capabilities (empty - no CPU features)
- bits/io.h - I/O port access
- bits/link.h - Dynamic linking (stub for initial port)
- bits/stdint.h - Stdint (standard, no changes needed)
2.13 atomic_arch.h
C-only atomics (no inline assembly). The 80286 has lock prefix for atomic memory operations:
/* 80286 atomic operations using lock prefix */
/* Note: 80286 supports lock cmpxchg, lock xchg, etc. */
#define a_cas a_cas
static inline int a_cas(volatile int *p, int t, int s)
{
    /* Use lock cmpxchg - but 80286 cmpxchg only supports 16-bit */
    /* For 32-bit, use a critical section or disable interrupts */
    /* Initial: use simple non-atomic fallback (single-threaded) */
    int old = *p;
    if (old == t) *p = s;
    return old;
}
/* ... similarly for a_swap, a_fetch_add, a_and, a_or, a_inc, a_dec, a_store, a_barrier, a_spin, a_crash */
Note: For the initial single-threaded port, simple non-atomic implementations suffice. The 80286 lock prefix works on 16-bit memory operands, but 32-bit atomics require either critical sections or interrupt disabling.
2.14 syscall_arch.h
OS/2 API call wrappers. This replaces Linux's int $128 with OS/2 far call to Dos* functions:
/* OS/2 syscall implementation via Dos* API calls */
/* Parameters passed on stack (OS/2 calling convention) */
/* Return value in AX (16-bit) or DX:AX (32-bit) */

#define __SYSCALL_LL_E(x) \
    ((union { long long ll; long l[2]; }){ .ll = x }).l[0], \
    ((union { long long ll; long l[2]; }){ .ll = x }).l[1]
#define __SYSCALL_LL_O(x) __SYSCALL_LL_E((x))

static inline long __syscall0(long n)
{
    /* Call OS/2 API by number */
    extern long os2_syscall0(long);
    return os2_syscall0(n);
}
/* ... __syscall1 through __syscall6 */
Implementation: The actual OS/2 calls will be in a runtime library (os2_syscall.asm or os2_syscall.c) that maps syscall numbers to Dos* API calls.
2.15 crt_arch.h
C runtime startup for OS/2 1.x:
/* OS/2 NE executable entry point */
/* ..start: is the entry point */
/* Stack has: return address, then nothing */
/* We need to set up DS=DGROUP, then call __libc_start_main */
__asm__(
".global _start\n"
"_start:\n"
"  mov ax, dgroup\n"     /* Set up data segment */
"  mov ds, ax\n"
"  mov es, ax\n"
"  mov ss, ax\n"
"  mov sp, [stack_top]\n"  /* Set stack pointer */
"  /* Pass argc=0, argv=NULL, envp=NULL */"
"  push 0\n"   /* envp */
"  push 0\n"   /* argv */
"  push 0\n"   /* argc */
"  call __libc_start_main\n"
);
Note: OS/2 1.x NE executables have a different entry convention than Linux. The entry point (..start) receives DS pointing to the DGROUP segment, with a stack set up by the OS. We need to adapt crt_arch.h and __libc_start_main for OS/2.
2.16 pthread_arch.h
Thread pointer access (stub for initial port - no threads):
/* OS/2 1.x does not have thread-local storage in the Linux sense */
/* For initial port, use a global variable for the thread pointer */
static inline uintptr_t __get_tp()
{
    return 0;  /* No thread pointer */
}
#define MC_PC eip  /* Program counter for setjmp */
2.17 reloc.h
Relocation types (stub for initial port - no dynamic linking):
#define LDSO_ARCH "i286"
/* Relocation types for NE format */
/* Initial port: no dynamic linking, so minimal */
#define REL_SYMBOLIC 0
#define REL_OFFSET   0
/* ... */
2.18 kstat.h
OS/2 kernel stat structure (maps to OS/2 file info):
/* OS/2 file information structure */
struct kstat {
    /* OS/2 file info fields */
    /* Map to musl stat structure */
};
Phase 3: OS/2 Syscall Layer
Replace Linux syscalls with OS/2 Dos* API calls. The syscall layer translates musl's syscall interface to OS/2 API calls.
3.1 OS/2 Syscall Runtime (src/runtime/os2_syscall.asm)
Assembly routines that map syscall numbers to OS/2 Dos* calls:
; OS/2 syscall dispatch table
; Input: AX = syscall number, parameters on stack
; Output: AX = return value (or DX:AX for 32-bit)

os2_syscall0:
    ; Map syscall number to OS/2 API
    ; AX = syscall number
    ; Jump to appropriate handler
    ret

os2_syscall_read:
    ; DosRead(fd, buf, count)
    ; Push parameters, far call DosRead
    ; Return bytes read in AX
    ret

os2_syscall_write:
    ; DosWrite(fd, buf, count)
    ; Push parameters, far call DosWrite
    ; Return bytes written in AX
    ret
; ... etc.
3.2 OS/2 API Wrappers (src/unistd/os2/, src/fcntl/os2/, etc.)
C wrappers that call OS/2 Dos* functions:
- os2_read.c → DosRead
- os2_write.c → DosWrite
- os2_open.c → DosOpen
- os2_close.c → DosClose
- os2_lseek.c → DosChgFilePtr
- os2_exit.c → DosExit
- os2_getpid.c → DosGetPID
- os2_gettime.c → DosGetDateTime
- os2_sleep.c → DosSleep
- os2_alloc.c → DosAllocSeg
- os2_free.c → DosFreeSeg
3.3 Syscall Mapping (arch/i286/syscall_arch.h)
The syscall_arch.h file defines how syscalls are made. For OS/2:
/* OS/2 syscall dispatch via C wrappers */
static inline long __syscall0(long n) {
    extern long __os2_syscall0(long);
    return __os2_syscall0(n);
}
static inline long __syscall1(long n, long a1) {
    extern long __os2_syscall1(long, long);
    return __os2_syscall1(n, a1);
}
/* ... __syscall2 through __syscall6 */
3.4 OS/2 API Definitions (arch/i286/bits/syscall.h.in)
Map Linux syscall numbers to OS/2 API functions:
/* OS/2 API call numbers */
#define __NR_read       1   /* DosRead */
#define __NR_write      2   /* DosWrite */
#define __NR_open       3   /* DosOpen */
#define __NR_close      4   /* DosClose */
#define __NR_lseek      5   /* DosChgFilePtr */
#define __NR_exit       6   /* DosExit */
#define __NR_exit_group 6   /* DosExit (same) */
#define __NR_getpid     7   /* DosGetPID */
/* ... etc. */
Phase 4: musl Source Modifications
4.1 Replace Arch-Specific Assembly with C
musl has arch-specific assembly in src/{dir}/{arch}/ subdirectories. For i286, we provide C-only implementations:
src/string/i286/ (new directory):
- memcpy.c - C implementation of memcpy (replace src/string/i386/memcpy.s)
- memmove.c - C implementation of memmove
- memset.c - C implementation of memset
src/fenv/i286/ (new directory):
- fenv.c - FPU environment management using 80287 FPU instructions (fnstcw, fldcw, fnstsw, fclex, etc.) via inline assembly or C with compiler intrinsics
src/thread/i286/ (new directory):
- clone.c - Thread clone (stub - return ENOSYS)
- tls.c - TLS (stub - return 0)
- __set_thread_area.c - Set thread area (stub - return 0)
- __unmapself.c - Unmap self (stub)
- syscall_cp.c - Cancellation point syscall (stub - just call regular syscall)
src/setjmp/i286/ (new directory):
- setjmp.c - C implementation of setjmp (save registers to buffer)
- longjmp.c - C implementation of longjmp (restore registers from buffer)
src/signal/i286/ (new directory):
- restore.c - Signal restore (stub - return ENOSYS)
- sigsetjmp.c - Signal setjmp (stub)
src/ldso/i286/ (new directory):
- Skip entirely for initial port (no dynamic linking)
4.2 OS/2-Specific Source Files
src/unistd/os2/ (new directory):
- os2_read.c - Read via DosRead
- os2_write.c - Write via DosWrite
- os2_close.c - Close via DosClose
- os2_lseek.c - Lseek via DosChgFilePtr
- os2_exit.c - Exit via DosExit
src/fcntl/os2/ (new directory):
- os2_open.c - Open via DosOpen
- os2_fcntl.c - Fcntl (stub/minimal)
src/stat/os2/ (new directory):
- os2_stat.c - Stat via DosFindFirst/DosQFileInfo
- os2_fstat.c - Fstat via DosQFileInfo
src/time/os2/ (new directory):
- os2_gettimeofday.c - Get time via DosGetDateTime
- os2_time.c - Time via DosGetDateTime
- os2_sleep.c - Sleep via DosSleep
src/exit/os2/ (new directory):
- os2_Exit.c - Exit via DosExit
src/mman/os2/ (new directory):
- os2_mmap.c - Mmap via DosAllocSeg
- os2_munmap.c - Munmap via DosFreeSeg
4.3 Adapt Core Source Files
Some musl source files need modifications for OS/2:
- src/env/__libc_start_main.c - Adapt for OS/2 entry point (no auxv, no argv/envp from stack)
- src/env/__init_tls.c - Stub out TLS initialization (no TLS on OS/2 1.x initially)
- src/env/__stack_chk_fail.c - Stack canary (can use simple implementation)
- src/internal/libc.c - Initialize libc state (no auxv, no page_size from kernel)
- src/internal/syscall.h - Use OS/2 syscall numbers, not Linux
4.4 Configure Script Changes
Modify musl/configure to detect the i286 target:
# Add to the case statement in configure:
i286*) ARCH=i286 ;;
i286-os2*) ARCH=i286 ;;
Set appropriate CFLAGS:
if test "$ARCH" = "i286" ; then
    # 80286 target: use clang with llvm-i286 backend
    CC="${CROSS_COMPILE}clang"
    CFLAGS_AUTO="$CFLAGS_AUTO -target i286-unknown-os2"
    CFLAGS_AUTO="$CFLAGS_AUTO -ffreestanding -nostdinc"
    # No dynamic linking
    shared=no
    # No threads initially
    # No CFI
    ADD_CFI=no
fi
Phase 5: Build System Integration
5.1 musl Build Integration
Approach: musl's build system is Make-based. We need to integrate it with the llvm_i286 CMake build.
Option A (Recommended): Use musl's existing Makefile with a custom config.mak:
# config.mak for i286-os2
ARCH = i286
CC = clang -target i286-unknown-os2 -S -emit-llvm -nostdinc -ffreestanding
# Then pipe through llvm-i286 → NASM → wlink
CC_CMD = $(CC) $(CFLAGS_ALL) -S -emit-llvm -o $@.ll $<
    llvm-i286 $@.ll -o $@.asm
    nasm -f obj -o $@ $@.asm
Option B: Create a CMake-based build that wraps the musl sources:
- Scan musl source directories for C files
- Compile each with: clang -target i286-unknown-os2 -S -emit-llvm → llvm-i286 → nasm -f obj
- Link with wlink into a static library (libc.a equivalent)
Tasks:
- Create musl/config.mak for i286 target
- Create a build script that runs: configure → make with the custom CC pipeline
- Ensure the CC pipeline produces object files compatible with wlink
- Create CRT startup files (crt1.c, crti.c, crtn.c) for OS/2
5.2 clang Target Configuration
Create a clang target configuration for i286-unknown-os2:
Data layout (for LLVM IR generation):
; 80286 OS/2 target
target datalayout = "e-p:32:32:32-i16:16:32-i32:32:32-i64:32:32-f32:32:32-f64:32:32-f80:32:32-n8:16:32-S0"
target triple = "i286-unknown-os2"
- e = little-endian
- p:32:32:32 = 32-bit pointer, 32-bit alignment, 32-bit allocation size
- i16:16:32 = 16-bit int (short), 16-bit alignment, 32-bit size
- i32:32:32 = 32-bit int, 32-bit alignment
- i64:32:32 = 64-bit int (emulated), 32-bit alignment
- f32:32:32 = 32-bit float, 32-bit alignment
- f64:32:32 = 64-bit float, 32-bit alignment
- f80:32:32 = 80-bit long double (80287 FPU), 32-bit alignment
- n8:16:32 = native alignments: 8-bit, 16-bit, 32-bit
- S0 = stack alignment: 0 (no specific alignment)
Note: The user specified int=32-bit, short=16-bit, pointer=32-bit. This means:
- short = 16-bit (i16)
- int = 32-bit (i32)
- long = 32-bit (i32)
- long long = 32-bit (emulated, stored as 32-bit for now) - actually, long long should be 64-bit but emulated. Let me reconsider: the user said "Our data types should match the defaults of a 32-bit machine like a 386." On i386, long long is 64-bit. So long long = 64-bit (emulated with 32-bit operations). But for the initial port, we may want to start with 32-bit long long and extend to 64-bit later.
Actually, looking at the musl alltypes.h.in, _Int64 defines long long / int64_t / uint64_t. On i386, _Int64 = long long which is 64-bit. So we should keep _Int64 = long long (64-bit, emulated).
Updated data layout:
target datalayout = "e-p:32:32:32-i16:16:32-i32:32:32-i64:64:64-f32:32:32-f64:64:64-f80:80:80-n8:16:32-S0"
Wait, but the 80286 can only handle 16-bit operations. 64-bit operations would need to be emulated with 32-bit operations, which themselves are emulated with 16-bit operations. This is a lot of emulation.
For the initial port, let me suggest:
- int = 32-bit (emulated with 16-bit ops)
- long = 32-bit (emulated with 16-bit ops)
- long long = 32-bit (for simplicity initially, can extend to 64-bit later)
- pointer = 32-bit (but only low 16 bits used initially within 64K segment)
- size_t = 32-bit (emulated)
- ssize_t = 32-bit (emulated)
This simplifies the initial port significantly. We can extend to 64-bit long long later.
5.3 Build Pipeline
musl source (.c)
  → clang -target i286-unknown-os2 -S -emit-llvm -ffreestanding -nostdinc
  → LLVM IR (.ll)
  → llvm-i286
  → NASM assembly (.asm)
  → nasm -f obj
  → object file (.o)
  → wlink system os2
  → OS/2 NE executable (.exe) or static library (.lib)
Integration steps:
1. Create a wrapper script musl_cc.sh that wraps clang + llvm-i286 + nasm
2. Set CC=musl_cc.sh in musl/config.mak
3. Set LIBCC= (no compiler runtime library needed - we provide our own)
4. Run configure with --target=i286-os2
5. Run make to build musl
Phase 6: Runtime Library Enhancements
6.1 32-Bit Arithmetic Runtime (src/runtime/runtime32.asm)
Comprehensive 32-bit arithmetic helpers:
; 32-bit signed multiply: DX:AX * CX:BX → DI:SI
_MultiplyI32:
    ; Implementation using 16-bit multiply and add
    ret

; 32-bit unsigned multiply
_MultiplyU32:
    ret

; 32-bit signed divide: DX:AX / CX:BX → quotient in DX:AX, remainder in CX:BX
_DivideI32:
    ret

; 32-bit unsigned divide
_DivideU32:
    ret

; 32-bit shift left
_ShiftL32:
    ret

; 32-bit shift right (logical)
_ShiftR32:
    ret

; 32-bit shift right (arithmetic)
_ShiftAr32:
    ret

; 32-bit comparison
_Cmp32:
    ; Returns -1, 0, or 1 in AX
    ret
6.2 OS/2 API Call Wrappers (src/runtime/os2_api.asm)
Assembly wrappers for OS/2 Dos* API calls:
; OS/2 API call wrappers
; These provide a C-callable interface to OS/2 Dos* functions

; DosExit(action, result)
_os2_DosExit:
    push bp
    mov bp, sp
    call far DOSEXIT
    pop bp
    ret

; DosWrite(file, buf, count, bytesWritten)
_os2_DosWrite:
    ; Push args, far call DOSWRITE, return
    ret

; DosRead(file, buf, count, bytesRead)
_os2_DosRead:
    ret

; DosOpen(path, openMode, shareMode, fileHandle)
_os2_DosOpen:
    ret

; DosClose(fileHandle)
_os2_DosClose:
    ret

; DosChgFilePtr(fileHandle, offset, method, newPosition)
_os2_DosChgFilePtr:
    ret

; DosGetDateTime(year, month, day, ...)
_os2_DosGetDateTime:
    ret

; DosSleep(milliseconds)
_os2_DosSleep:
    ret
6.3 OS/2 Startup (src/runtime/os2_crt.asm)
C runtime startup for OS/2 NE executables:
; OS/2 entry point
..start:
    ; Set up data segment
    mov ax, dgroup
    mov ds, ax
    mov es, ax
    mov ss, ax
    
    ; Set up stack
    mov sp, stack_top
    
    ; Call __libc_start_main(main, 0, NULL, _init, _fini, 0)
    ; For initial port: argc=0, argv=NULL, envp=NULL
    push 0      ; envp
    push 0      ; argv
    push 0      ; argc
    call _start_c
    
    ; Exit
    push 0     ; exit code
    push 0     ; action code
    call far DOSEXIT
Phase 7: Testing and Verification
7.1 Code Generator Unit Tests
Extend existing test suite:
- 32-bit arithmetic tests (add, sub, mul, div, shifts, comparisons)
- 32-bit memory operations (load, store, GEP)
- Floating point tests (fadd, fsub, fmul, fdiv)
- Function call tests with 32-bit arguments
- Global variable tests
- Switch statement tests
- Phi node tests
7.2 musl Component Tests
Test individual musl components:
- string/memcpy.c → compile → run → verify
- string/memset.c → compile → run → verify
- string/strlen.c → compile → run → verify
- stdlib/atoi.c → compile → run → verify
- ctype/isalpha.c → compile → run → verify
- unistd/write.c → compile → run → verify (write to stdout)
- unistd/read.c → compile → run → verify (read from stdin)
- time/time.c → compile → run → verify
7.3 Integration Tests
End-to-end tests linking musl components:
- Hello world (using DosWrite via musl's write())
- File copy (using open, read, write, close)
- String manipulation (using strlen, strcpy, strcmp)
- Integer parsing (using atoi, strtol)
- Time display (using time, ctime)
7.4 Test Pipeline
test.c → clang -target i286 → test.ll → llvm-i286 → test.asm
→ nasm -f obj → test.o → wlink → test.exe → 2ine → verify output
Implementation Order
Step	Task	Depends On
1	32-bit emulation: complete add/sub/mul/div/rem	-
2	32-bit emulation: bitwise (and/or/xor)	1
3	32-bit emulation: shifts (shl/shr/sar)	1
4	32-bit emulation: comparisons (icmp)	1
5	32-bit emulation: load/store/GEP	1
6	32-bit function arguments and return values	1
7	Missing IR instructions: switch, phi, unreachable	-
8	Missing IR instructions: bitcast, ptrtoint, inttoptr	-
9	Missing IR instructions: sdiv, udiv, srem, urem	1
10	FPU support: fadd, fsub, fmul, fdiv, frem	-
11	FPU support: fcmp, fptrunc, fpext, fptoui, fptosi, uitofp, sitofp	10
12	Global variables and data segments	-
13	BSS segment support	12
14	Improved function prologue/epilogue with local variables	-
15	Create arch/i286/ directory with all headers	-
16	Create arch/i286/bits/alltypes.h.in (32-bit types)	15
17	Create arch/i286/bits/syscall.h.in (OS/2 syscall numbers)	15
18	Create arch/i286/atomic_arch.h (C-only atomics)	15
19	Create arch/i286/syscall_arch.h (OS/2 syscall wrappers)	15, 3.1
20	Create arch/i286/crt_arch.h (OS/2 startup)	15, 6.3
21	Create arch/i286/pthread_arch.h (thread stub)	15
22	Create arch/i286/reloc.h (relocation stub)	15
23	Create arch/i286/bits/ (all other headers)	15
24	Create OS/2 syscall runtime (os2_syscall.asm)	19
25	Create OS/2 API wrappers (os2_read, os2_write, etc.)	24
26	Create OS/2 CRT startup (os2_crt.asm)	20
27	Create 32-bit arithmetic runtime (runtime32.asm)	1
28	Replace arch-specific assembly with C (string, fenv, setjmp, thread)	15
29	Adapt musl source files for OS/2 (exit, unistd, fcntl, stat, time)	25
30	musl configure script changes (i286 target)	15
31	Build pipeline (clang → llvm-i286 → nasm → wlink)	30
32	clang target configuration (i286-unknown-os2)	31
33	Integration tests (string, stdlib, ctype, unistd, time)	29, 31
34	Segment:offset addressing design (for future)	-
Key Design Decisions
1. 32-bit data model: All types match i386 (int=32-bit, long=32-bit, pointer=32-bit, size_t=32-bit). The 80286 emulates 32-bit operations with 16-bit instruction sequences. long long is initially 32-bit for simplicity, can be extended to 64-bit later.
2. Single 64K segment: All data (code, data, BSS, stack) fits within a single 64K segment initially. 32-bit pointers are stored as 32-bit values but only the low 16 bits are used as offsets. Future: linear-to-segment emulation converts 32-bit pointers to segment:offset pairs.
3. C-only musl: No arch-specific assembly files. All musl arch-specific code is written in C (using inline assembly only for FPU instructions and atomic operations where necessary).
4. OS/2 syscall layer: musl's syscall() interface is mapped to OS/2 Dos* API calls via C wrappers. The syscall_arch.h calls C functions that invoke OS/2 APIs via far call.
5. Initial scope: Core libc (string, ctype, stdlib, errno, exit) + file I/O (unistd, fcntl) + time. Threads, signals, sockets, mmap, dynamic linking, locale, and math are deferred.
6. FPU: 80287 FPU is required (per NOTES.MD). FPU instructions (fadd, fmul, etc.) are supported via the code generator. No software FPU emulation.
7. No dynamic linking: Initial port is static-only. No ldso (dynamic linker). All symbols resolved at link time via wlink.
8. No threads: Initial port is single-threaded. Thread functions return ENOSYS. __get_tp() returns 0. __init_tls is stubbed.
Risks and Mitigations
1. 32-bit emulation complexity: The code generator must handle all 32-bit operations correctly. This is the highest-risk area. Mitigation: Start with comprehensive 32-bit tests before attempting musl compilation.
2. musl assumes Linux syscalls: Many musl source files call __syscall() directly. Mitigation: The syscall_arch.h maps syscall numbers to OS/2 API calls. Source files that use unsupported syscalls (e.g., mmap, fork) return ENOSYS.
3. Stack frame management: 32-bit values on a 16-bit stack require careful stack frame layout. Mitigation: All stack slots are 4 bytes (32-bit aligned). Stack pointer manipulation uses 16-bit add/sub but with 4-byte increments.
4. Global variable addressing: musl has many global variables. Mitigation: All globals in DGROUP segment, accessed via DS:offset. 32-bit globals stored as two 16-bit words.
5. FPU support: The 80287 FPU has a register stack (ST(0)-ST(7)) that must be managed. Mitigation: FPU register stack tracking in the code generator, with spill to memory when needed.
6. Function pointer calls: musl uses function pointers (e.g., __funcs_on_exit, atexit handlers). Mitigation: Ensure call instruction supports indirect calls (call far [bx] or call [bx]).
7. String constants and data: musl has many string constants (error messages, etc.). Mitigation: String constants in _DATA segment, accessed via DS:offset.
This plan provides a comprehensive roadmap for the initial musl port to llvm_i286. The highest priority items are the 32-bit emulation in the code generator and the arch/i286/ directory with OS/2 syscall wrappers.
# Todos
[✓] Create comprehensive plan for porting musl to llvm_i286
+ Thought: 12.3s
The plan is now complete. Here's a summary of the key decisions and structure:
Summary
Data Model: i386-compatible (int=32-bit, short=16-bit, long=32-bit, pointer=32-bit, size_t=32-bit). All 32-bit operations emulated with 16-bit 80286 instructions. long long initially 32-bit, extensible to 64-bit later.
Scope: Core libc (string, ctype, stdlib, errno, exit) + file I/O (unistd, fcntl) + time. Threads, signals, sockets, mmap, dynamic linking deferred.
Seven Phases:
1. Code Generator Enhancements - Comprehensive 32-bit emulation, missing LLVM IR instructions (switch, phi, sdiv, udiv, FPU ops, etc.), segment:offset addressing, global variables, FPU support, improved calling convention
2. musl arch/i286/ Directory - All architecture-specific headers (alltypes.h.in, syscall.h.in, atomic_arch.h, crt_arch.h, pthread_arch.h, reloc.h, bits/*)
3. OS/2 Syscall Layer - Dos* API wrappers replacing Linux syscalls, runtime assembly for OS/2 API calls
4. musl Source Modifications - Replace arch-specific assembly with C, create OS/2-specific source files, adapt core sources
5. Build System Integration - Configure script, clang target configuration, build pipeline (clang → llvm-i286 → nasm → wlink)
6. Runtime Library - 32-bit arithmetic helpers, OS/2 API call wrappers, CRT startup
7. Testing - Unit tests, component tests, integration tests
Key Design Decisions:
- Single 64K DGROUP segment initially, designed for future linear-to-segment emulation
- C-only musl implementations (no arch-specific assembly files)
- OS/2 Dos* API calls via far call convention
- Static linking only (no dynamic linker)
- 80287 FPU required (no software FPU emulation)
- Initial port is single-threaded
Would you like me to proceed with implementation, or do you have questions or adjustments to the plan?
