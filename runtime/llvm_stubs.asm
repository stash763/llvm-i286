; LLVM intrinsic stubs for musl linking
; Provides stub implementations for LLVM intrinsics that our codegen doesn't lower
; Most are no-ops or return 0; memcpy/memset delegate to musl implementations

; Memory intrinsics - delegate to musl
extern memcpy
extern memset
extern memmove

segment _TEXT CLASS=CODE

global llvm.memcpy.p0.i32
global llvm.memcpy.p0.i64
global llvm.memset.p0.i32
global llvm.memset.p0.i64
global llvm.memmove.p0.i32
global llvm.memmove.p0.i64

llvm.memcpy.p0.i32:
llvm.memcpy.p0.i64:
    jmp far memcpy

llvm.memset.p0.i32:
llvm.memset.p0.i64:
    jmp far memset

llvm.memmove.p0.i32:
llvm.memmove.p0.i64:
    jmp far memmove

; Lifetime intrinsics - no-ops (just return, they have pointer arg)
global llvm.lifetime.start.p0
global llvm.lifetime.end.p0

llvm.lifetime.start.p0:
llvm.lifetime.end.p0:
    retf

; Stack save/restore - no-ops
global llvm.stacksave
global llvm.stackrestore

llvm.stacksave:
    xor ax, ax
    retf

llvm.stackrestore:
    retf

; VA copy - no-op
global llvm.va_copy

llvm.va_copy:
    retf

; Noalias scope decl - no-op
global llvm.experimental.noalias.scope.decl

llvm.experimental.noalias.scope.decl:
    retf

; Assume - no-op
global llvm.assume

llvm.assume:
    retf

; Integer intrinsics - these need actual implementations
; For now, stub returning 0
global llvm.abs.i32
global llvm.abs.i64
global llvm.smax.i32
global llvm.smin.i32
global llvm.umax.i32
global llvm.umax.i64
global llvm.umin.i32
global llvm.umin.i64
global llvm.umin.i8
global llvm.ctpop.i32
global llvm.bswap.i16
global llvm.bswap.i32
global llvm.fshl.i32
global llvm.fshl.i64
global llvm.usub.sat.i32
global llvm.umul.with.overflow.i32
global llvm.umul.with.overflow.i64

llvm.abs.i32:
llvm.abs.i64:
llvm.smax.i32:
llvm.smin.i32:
llvm.umax.i32:
llvm.umax.i64:
llvm.umin.i32:
llvm.umin.i64:
llvm.umin.i8:
llvm.ctpop.i32:
llvm.bswap.i16:
llvm.bswap.i32:
llvm.fshl.i32:
llvm.fshl.i64:
llvm.usub.sat.i32:
llvm.umul.with.overflow.i32:
llvm.umul.with.overflow.i64:
    xor ax, ax
    retf

; FP intrinsics - all return 0 (stub)
global llvm.copysign.f32
global llvm.copysign.f64

; Constrained FP intrinsics
global llvm.experimental.constrained.fadd.f32
global llvm.experimental.constrained.fadd.f64
global llvm.experimental.constrained.fadd.f80
global llvm.experimental.constrained.fsub.f32
global llvm.experimental.constrained.fsub.f64
global llvm.experimental.constrained.fsub.f80
global llvm.experimental.constrained.fmul.f32
global llvm.experimental.constrained.fmul.f64
global llvm.experimental.constrained.fmul.f80
global llvm.experimental.constrained.fdiv.f32
global llvm.experimental.constrained.fdiv.f64
global llvm.experimental.constrained.fdiv.f80
global llvm.experimental.constrained.fcmp.f32
global llvm.experimental.constrained.fcmp.f64
global llvm.experimental.constrained.fcmp.f80
global llvm.experimental.constrained.fcmps.f32
global llvm.experimental.constrained.fcmps.f64
global llvm.experimental.constrained.fcmps.f80
global llvm.experimental.constrained.fmuladd.f32
global llvm.experimental.constrained.fmuladd.f64
global llvm.experimental.constrained.fmuladd.f80
global llvm.experimental.constrained.fpext.f64.f32
global llvm.experimental.constrained.fpext.f80.f32
global llvm.experimental.constrained.fpext.f80.f64
global llvm.experimental.constrained.fptosi.i32.f32
global llvm.experimental.constrained.fptosi.i32.f64
global llvm.experimental.constrained.fptosi.i32.f80
global llvm.experimental.constrained.fptosi.i64.f32
global llvm.experimental.constrained.fptosi.i64.f64
global llvm.experimental.constrained.fptosi.i64.f80
global llvm.experimental.constrained.fptoui.i32.f80
global llvm.experimental.constrained.fptrunc.f32.f64
global llvm.experimental.constrained.fptrunc.f32.f80
global llvm.experimental.constrained.fptrunc.f64.f80
global llvm.experimental.constrained.sitofp.f32.i32
global llvm.experimental.constrained.sitofp.f64.i32
global llvm.experimental.constrained.sitofp.f64.i64
global llvm.experimental.constrained.sitofp.f80.i32
global llvm.experimental.constrained.sitofp.f80.i64
global llvm.experimental.constrained.uitofp.f64.i32
global llvm.experimental.constrained.uitofp.f80.i32

llvm.copysign.f32:
llvm.copysign.f64:
llvm.experimental.constrained.fadd.f32:
llvm.experimental.constrained.fadd.f64:
llvm.experimental.constrained.fadd.f80:
llvm.experimental.constrained.fsub.f32:
llvm.experimental.constrained.fsub.f64:
llvm.experimental.constrained.fsub.f80:
llvm.experimental.constrained.fmul.f32:
llvm.experimental.constrained.fmul.f64:
llvm.experimental.constrained.fmul.f80:
llvm.experimental.constrained.fdiv.f32:
llvm.experimental.constrained.fdiv.f64:
llvm.experimental.constrained.fdiv.f80:
llvm.experimental.constrained.fcmp.f32:
llvm.experimental.constrained.fcmp.f64:
llvm.experimental.constrained.fcmp.f80:
llvm.experimental.constrained.fcmps.f32:
llvm.experimental.constrained.fcmps.f64:
llvm.experimental.constrained.fcmps.f80:
llvm.experimental.constrained.fmuladd.f32:
llvm.experimental.constrained.fmuladd.f64:
llvm.experimental.constrained.fmuladd.f80:
llvm.experimental.constrained.fpext.f64.f32:
llvm.experimental.constrained.fpext.f80.f32:
llvm.experimental.constrained.fpext.f80.f64:
llvm.experimental.constrained.fptosi.i32.f32:
llvm.experimental.constrained.fptosi.i32.f64:
llvm.experimental.constrained.fptosi.i32.f80:
llvm.experimental.constrained.fptosi.i64.f32:
llvm.experimental.constrained.fptosi.i64.f64:
llvm.experimental.constrained.fptosi.i64.f80:
llvm.experimental.constrained.fptoui.i32.f80:
llvm.experimental.constrained.fptrunc.f32.f64:
llvm.experimental.constrained.fptrunc.f32.f80:
llvm.experimental.constrained.fptrunc.f64.f80:
llvm.experimental.constrained.sitofp.f32.i32:
llvm.experimental.constrained.sitofp.f64.i32:
llvm.experimental.constrained.sitofp.f64.i64:
llvm.experimental.constrained.sitofp.f80.i32:
llvm.experimental.constrained.sitofp.f80.i64:
llvm.experimental.constrained.uitofp.f64.i32:
llvm.experimental.constrained.uitofp.f80.i32:
    xor ax, ax
    retf

; musl internal stubs
global __stdio_exit_needed
global ___errno_location
global __malloc_replaced
global __aligned_alloc_replaced

__stdio_exit_needed:
    retf

___errno_location:
    xor ax, ax
    retf

__malloc_replaced:
__aligned_alloc_replaced:
    dw 0
    retf