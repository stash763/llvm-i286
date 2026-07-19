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
_writev_total_hi dw 0

segment _TEXT CLASS=CODE

; ========================================================================
; Shared syscall dispatch handlers
; These are jumped to (not called) from __os2_syscallN entry points
; All use [bp+relative] addressing; bp is set up by the calling __os2_syscallN
; Each handler ends with pop bp; retf to return to the original caller
; ========================================================================

syscall_write:
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
    ; DOSWRITE uses Pascal convention (retf 12) - callee cleans up
    
    mov ax, [_syscall_wlen]  ; return bytes written
    xor dx, dx
    
    pop di
    pop si
    pop bp
    retf

syscall_read:
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
    ; DOSREAD uses Pascal convention (retf 12) - callee cleans up
    
    mov ax, [_syscall_wlen]  ; return bytes read
    xor dx, dx
    
    pop di
    pop si
    pop bp
    retf

syscall_ioctl:
    ; SYS_ioctl(fd, request, ...) - stub
    xor ax, ax           ; return 0 (success)
    cwd
    pop bp
    retf

syscall_writev:
    push si
    push di
    push bx
    push cx
    
    mov bx, [bp+10]    ; fd (file handle) - keep in bx
    mov si, [bp+14]    ; iov offset (low word)
    mov di, [bp+16]    ; iov selector (high word)
    mov cx, [bp+18]    ; iovcnt (count)
    xor dx, dx         ; dx = total bytes written (low word)
    mov word [_writev_total_hi], 0  ; high word = 0
    
writev_loop:
    test cx, cx
    jz writev_done
    
    ; Load iov_base (32-bit pointer)
    mov ax, [si]       ; iov_base offset (low word)
    push dx            ; save total low word
    mov dx, [si+2]     ; iov_base selector (high word)
    
    ; Load iov_len (32-bit at [si+4])
    push cx            ; save iovcnt
    push si            ; save iov offset
    mov cx, [si+4]     ; iov_len (low word)
    
    ; Push arguments for DOSWRITE: hf, buf selector, buf offset, count, wlen selector, wlen offset
    push bx            ; hf (file handle)
    push dx            ; buf selector (HIGH word)
    push ax            ; buf offset (LOW word)
    push cx            ; count
    push ds            ; wlen selector (HIGH word)
    push _syscall_wlen ; wlen offset (LOW word)
    call far DOSWRITE
    ; DOSWRITE uses Pascal convention (retf 12) - callee cleans up
    
    ; Add bytes written to total
    mov ax, [_syscall_wlen]
    pop si             ; restore iov offset
    pop cx             ; restore iovcnt
    pop dx             ; restore total low word
    add dx, ax         ; add bytes written to total
    adc word [_writev_total_hi], 0  ; add carry to high word
    
    add si, 8          ; advance to next iovec (8 bytes: 4 base + 4 len)
    dec cx
    jmp writev_loop
    
writev_done:
    mov ax, dx         ; ax = total bytes written (low word)
    mov dx, [_writev_total_hi]  ; dx = total bytes written (high word)
    ; DX:AX = 32-bit total bytes written
    
    pop cx
    pop bx
    pop di
    pop si
    pop bp
    retf

syscall_close:
    mov ax, [bp+10]      ; file handle (low word of a1)
    
    push ax              ; hf
    call far DOSCLOSE
    ; DOSCLOSE uses Pascal convention (retf 2) - callee cleans up
    
    cwd                  ; sign-extend to DX:AX
    pop bp
    retf

syscall_getpid:
    xor ax, ax
    cwd
    pop bp
    retf

syscall_exit:
    mov ax, [bp+10]      ; exit code (low word of a1)
    
    push 0               ; action = 0 (terminate process)
    push ax              ; return code
    call far DOSEXIT
    ; DOSEXIT does not return

; ========================================================================
; __os2_syscall0(long n)
; Far call: [bp+6] = n (32-bit, low word at [bp+6], high at [bp+8])
; ========================================================================
__os2_syscall0:
    push bp
    mov bp, sp
    
    mov ax, [bp+6]       ; syscall number (low word)
    
    cmp ax, 6            ; __NR_exit / __NR_exit_group
    je syscall_exit
    
    mov ax, -38          ; -ENOSYS
    cwd
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
    je syscall_exit
    
    mov ax, -38          ; -ENOSYS
    cwd
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
    je syscall_exit
    
    cmp ax, 4            ; __NR_close
    je syscall_close
    
    cmp ax, 7            ; __NR_getpid
    je syscall_getpid
    
    mov ax, -38          ; -ENOSYS
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
    je syscall_write
    
    cmp ax, 1            ; __NR_read
    je syscall_read
    
    cmp ax, 24           ; __NR_ioctl
    je syscall_ioctl
    
    cmp ax, 999          ; __NR_writev
    je syscall_writev
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

; ========================================================================
; __os2_syscall4(long n, long a1, long a2, long a3, long a4)
; ========================================================================
__os2_syscall4:
    push bp
    mov bp, sp
    
    mov ax, [bp+6]       ; syscall number
    
    cmp ax, 2            ; __NR_write
    je syscall_write
    cmp ax, 1            ; __NR_read
    je syscall_read
    cmp ax, 24           ; __NR_ioctl
    je syscall_ioctl
    cmp ax, 999          ; __NR_writev
    je syscall_writev
    cmp ax, 6            ; __NR_exit
    je syscall_exit
    cmp ax, 4            ; __NR_close
    je syscall_close
    cmp ax, 7            ; __NR_getpid
    je syscall_getpid
    
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
    
    mov ax, [bp+6]       ; syscall number
    
    cmp ax, 2            ; __NR_write
    je syscall_write
    cmp ax, 1            ; __NR_read
    je syscall_read
    cmp ax, 24           ; __NR_ioctl
    je syscall_ioctl
    cmp ax, 999          ; __NR_writev
    je syscall_writev
    cmp ax, 6            ; __NR_exit
    je syscall_exit
    cmp ax, 4            ; __NR_close
    je syscall_close
    cmp ax, 7            ; __NR_getpid
    je syscall_getpid
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

; ========================================================================
; __os2_syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
; musl's sccp always calls __syscall6 with 6 args (padding unused with 0)
; so __os2_syscall6 must dispatch all syscalls, not just return -ENOSYS
; ========================================================================
__os2_syscall6:
    push bp
    mov bp, sp
    
    mov ax, [bp+6]       ; syscall number
    
    cmp ax, 2            ; __NR_write
    je syscall_write
    cmp ax, 1            ; __NR_read
    je syscall_read
    cmp ax, 24           ; __NR_ioctl
    je syscall_ioctl
    cmp ax, 999          ; __NR_writev
    je syscall_writev
    cmp ax, 6            ; __NR_exit
    je syscall_exit
    cmp ax, 4            ; __NR_close
    je syscall_close
    cmp ax, 7            ; __NR_getpid
    je syscall_getpid
    
    mov ax, -38          ; -ENOSYS
    cwd
    pop bp
    retf

segment end