; OS/2 Syscall Dispatch Layer
; Implements __os2_syscall0 through __os2_syscall6 for musl libc
; All parameters are 32-bit (long), passed on stack in C calling convention
; Return value in DX:AX (32-bit signed)

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

segment _TEXT CLASS=CODE
global __os2_syscall2
global __os2_syscall3
global __os2_syscall4
global __os2_syscall5
global __os2_syscall6

; Helper: return -ENOSYS (syscall not implemented) - no longer used, each handler has inline return

; ========================================================================
; __os2_syscall0(long n)
; Stack: [bp+4] = n (32-bit, low word at [bp+4], high at [bp+6])
; ========================================================================
__os2_syscall0:
    push bp
    mov bp, sp
    
    ; Load syscall number (low word)
    mov ax, [bp+4]
    
    ; Dispatch based on syscall number
    cmp ax, 6         ; __NR_exit / __NR_exit_group
    je .syscall_exit_0
    
    ; Return -ENOSYS (not implemented)
    mov ax, -38
    cwd
    pop bp
    ret

.syscall_exit_0:
    ; Exit with code 0 (no argument provided)
    push 0            ; action = 0 (terminate process)
    push 0            ; return code = 0
    call far DOSEXIT

    pop bp
    ret

; ========================================================================
; __os2_syscall1(long n, long a1)
; Stack: [bp+4]=n [bp+8]=a1 (each 32-bit)
; ========================================================================
__os2_syscall1:
    push bp
    mov bp, sp
    
    ; Load syscall number
    mov ax, [bp+4]
    
    cmp ax, 6         ; __NR_exit / __NR_exit_group
    je .syscall_exit_1
    
    ; Return -ENOSYS (not implemented)
    mov ax, -38
    cwd
    pop bp
    ret

.syscall_exit_1:
    ; _Exit(exit_code) - exit with provided code
    ; a1 is at [bp+8], low word
    mov ax, [bp+8]    ; exit code (low word)
    
    push 0            ; action = 0 (terminate process)
    push ax           ; return code
    call far DOSEXIT

    pop bp
    ret

; ========================================================================
; __os2_syscall2(long n, long a1, long a2)
; Stack: [bp+4]=n [bp+8]=a1 [bp+12]=a2
; ========================================================================
__os2_syscall2:
    push bp
    mov bp, sp
    
    ; Load syscall number
    mov ax, [bp+4]
    
    cmp ax, 6         ; __NR_exit / __NR_exit_group
    je .syscall_exit_2
    
    cmp ax, 4         ; __NR_close
    je .syscall_close
    
    cmp ax, 7         ; __NR_getpid
    je .syscall_getpid
    
    ; Return -ENOSYS (not implemented)
    mov ax, -38
    cwd
    pop bp
    ret

.syscall_exit_2:
    ; _Exit(exit_code) - exit with provided code
    ; a1 is at [bp+8], low word
    mov ax, [bp+8]    ; exit code (low word)
    
    push 0            ; action = 0 (terminate process)
    push ax           ; return code
    call far DOSEXIT

.syscall_close:
    ; close(fd) - DosClose(hf)
    ; a1 = fd (at [bp+8])
    mov ax, [bp+8]    ; file handle
    
    push ax           ; hf
    call far DOSCLOSE
    add sp, 2         ; cleanup 1 arg
    
    ; Return value in AX (0 = success)
    cwd               ; sign-extend to DX:AX
    pop bp
    ret 8             ; cleanup 2 args (n, a1) = 4 bytes, but we already cleaned 2 in the call

.syscall_getpid:
    ; getpid() - return 0 for now (stub)
    xor ax, ax
    cwd
    pop bp
    ret 8             ; cleanup 2 args (n, a1)

; ========================================================================
; __os2_syscall3(long n, long a1, long a2, long a3)
; Stack: [bp+4]=n [bp+8]=a1 [bp+12]=a2 [bp+16]=a3
; ========================================================================
__os2_syscall3:
    push bp
    mov bp, sp
    
    ; Load syscall number (first arg is at [bp+4] for near calls)
    mov ax, [bp+4]
    
    cmp ax, 2         ; __NR_write
    je .syscall_write
    
    cmp ax, 1         ; __NR_read
    je .syscall_read
    
    ; Return -ENOSYS (not implemented)
    mov ax, -38
    cwd
    pop bp
    ret

.syscall_write:
    ; write(fd, buf, count) -> DosWrite(hf, buf, count, &bytesWritten)
    
    push si
    push di
    
    ; Get parameters from __os2_syscall3's frame
    mov ax, [bp+8]      ; hf (file handle)
    mov si, [bp+12]     ; buf offset (low word)
    mov di, [bp+14]     ; buf selector (high word)
    mov cx, [bp+16]     ; count (low word)
    
    ; Call DosWrite(hf, buf_selector, buf_offset, count, wlen_selector, wlen_offset)
    push ax             ; hf
    push di             ; buf selector
    push si             ; buf offset
    push cx             ; count
    push ds             ; wlen selector
    push _syscall_wlen  ; wlen offset
    call far DOSWRITE
    ; DOSWRITE cleans up the stack (Pascal convention, ret 12)
    
    ; Return count in AX
    mov ax, cx
    
    pop di
    pop si
    pop bp
    ret

.syscall_read:
    ; read(fd, buf, count) -> DosRead(hf, buf, count, &bytesRead)
    ; a1 = fd (at [bp+8])
    ; a2 = buf (at [bp+12])
    ; a3 = count (at [bp+16])
    
    mov ax, [bp+8]    ; fd / file handle
    
    push 0            ; bytesRead (placeholder)
    push word [bp+16] ; count (low word)
    push ds           ; buf segment
    push word [bp+12] ; buf offset (low word)
    push ax           ; hf
    call far DOSREAD
    add sp, 10        ; cleanup 5 args
    
    ; Return value in AX = bytes read
    cwd
    pop bp
    ret             ; caller cleanup

; ========================================================================
; __os2_syscall4(long n, long a1, long a2, long a3, long a4)
; ========================================================================
__os2_syscall4:
    push bp
    mov bp, sp
    
    ; Return -ENOSYS (not implemented)
    mov ax, -38
    cwd
    pop bp
    ret

; ========================================================================
; __os2_syscall5(long n, long a1, long a2, long a3, long a4, long a5)
; ========================================================================
__os2_syscall5:
    push bp
    mov bp, sp
    
    ; Return -ENOSYS (not implemented)
    mov ax, -38
    cwd
    pop bp
    ret

; ========================================================================
; __os2_syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
; ========================================================================
__os2_syscall6:
    push bp
    mov bp, sp
    
    ; Return -ENOSYS (not implemented)
    mov ax, -38
    cwd
    pop bp
    ret
