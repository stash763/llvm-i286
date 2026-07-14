; OS/2 Syscall Dispatch Layer
; Implements __os2_syscall0 through __os2_syscall6 for musl libc
; All parameters are 32-bit (long), passed on stack in C calling convention
; Return value in DX:AX (32-bit signed)
; Far calling convention: caller pushes args, calls far, callee retf, caller cleans up

BITS 16

extern DOSEXIT
extern DOSWRITE
extern DOSREAD
extern DOSCLOSE

global __os2_syscall0
global __os2_syscall1
global __os2_syscall2
global __os2_syscall3
global __os2_syscall4
global __os2_syscall5
global __os2_syscall6

segment _DATA CLASS=DATA

_syscall_wlen dw 0
byte_msg db 'X'
_debug_char db '?'

segment _TEXT CLASS=CODE

; ========================================================================
; __os2_syscall0(long n)
; Far call: [bp+6] = n (32-bit, low word at [bp+6], high at [bp+8])
; ========================================================================
__os2_syscall0:
    push bp
    mov bp, sp
    
    mov ax, [bp+6]       ; syscall number (low word)
    
    cmp ax, 6            ; __NR_exit / __NR_exit_group
    je .syscall_exit_0
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

.syscall_exit_0:
    push 0               ; action = 0 (terminate process)
    push 0               ; return code = 0
    call far DOSEXIT

    pop bp
    retf

; ========================================================================
; __os2_syscall1(long n, long a1)
; Far call: [bp+6]=n [bp+10]=a1 (each 32-bit)
; ========================================================================
__os2_syscall1:
    push bp
    mov bp, sp
    
    mov ax, [bp+6]       ; syscall number
    
    cmp ax, 6            ; __NR_exit / __NR_exit_group
    je .syscall_exit_1
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

.syscall_exit_1:
    mov ax, [bp+10]      ; exit code (low word of a1)
    
    push 0               ; action = 0 (terminate process)
    push ax              ; return code
    call far DOSEXIT

    pop bp
    retf

; ========================================================================
; __os2_syscall2(long n, long a1, long a2)
; Far call: [bp+6]=n [bp+10]=a1 [bp+14]=a2
; ========================================================================
__os2_syscall2:
    push bp
    mov bp, sp
    
    mov ax, [bp+6]       ; syscall number
    
    cmp ax, 6            ; __NR_exit / __NR_exit_group
    je .syscall_exit_2
    
    cmp ax, 4            ; __NR_close
    je .syscall_close
    
    cmp ax, 7            ; __NR_getpid
    je .syscall_getpid
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

.syscall_exit_2:
    mov ax, [bp+10]      ; exit code (low word of a1)
    
    push 0               ; action = 0 (terminate process)
    push ax              ; return code
    call far DOSEXIT

    pop bp
    retf

.syscall_close:
    mov ax, [bp+10]      ; file handle (low word of a1)
    
    push ax              ; hf
    call far DOSCLOSE
    add sp, 2            ; cleanup 1 arg
    
    cwd                  ; sign-extend to DX:AX
    pop bp
    retf

.syscall_getpid:
    xor ax, ax
    cwd
    pop bp
    retf

; ========================================================================
; __os2_syscall3(long n, long a1, long a2, long a3)
; Far call: [bp+6]=n [bp+10]=a1 [bp+14]=a2 [bp+18]=a3
; ========================================================================
__os2_syscall3:
    push bp
    mov bp, sp
    
    mov ax, [bp+6]       ; syscall number
    
    cmp ax, 2            ; __NR_write
    je .syscall_write
    
    cmp ax, 1            ; __NR_read
    je .syscall_read
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

.syscall_write:
    push si
    push di
    
    mov ax, [bp+10]      ; hf (file handle)
    mov si, [bp+14]      ; buf offset (low word)
    mov di, [bp+16]      ; buf selector (high word)
    mov cx, [bp+18]      ; count (low word)
    
    push ax              ; hf
    push di              ; buf selector (HIGH word first)
    push si              ; buf offset (LOW word second)
    push cx              ; count
    push ds              ; wlen selector (HIGH word first)
    push _syscall_wlen   ; wlen offset (LOW word second)
    call far DOSWRITE
    ; DOSWRITE cleans up the stack (Pascal convention, ret 12)
    
    mov ax, cx           ; return count
    xor dx, dx
    
    pop di
    pop si
    pop bp
    retf

.syscall_read:
    push si
    push di
    
    mov ax, [bp+10]      ; fd / file handle
    mov si, [bp+14]      ; buf offset (low word)
    mov di, [bp+16]      ; buf selector (high word)
    mov cx, [bp+18]      ; count (low word)
    
    push 0               ; bytesRead offset (low word)
    push ds              ; bytesRead selector (high word)
    push cx              ; count
    push di              ; buf selector (high word first)
    push si              ; buf offset (low word second)
    push ax              ; hf
    call far DOSREAD
    add sp, 12           ; cleanup 6 args
    
    cwd
    pop di
    pop si
    pop bp
    retf

; ========================================================================
; __os2_syscall4(long n, long a1, long a2, long a3, long a4)
; ========================================================================
__os2_syscall4:
    push bp
    mov bp, sp
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

; ========================================================================
; __os2_syscall5(long n, long a1, long a2, long a3, long a4, long a5)
; ========================================================================
__os2_syscall5:
    push bp
    mov bp, sp
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

; ========================================================================
; __os2_syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
; ========================================================================
__os2_syscall6:
    push bp
    mov bp, sp
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

segment end