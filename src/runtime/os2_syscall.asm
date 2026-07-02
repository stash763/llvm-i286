; OS/2 Syscall Dispatch
; Maps musl syscall numbers to OS/2 Dos* API calls
; Input: AX = syscall number, parameters on stack
; Output: AX = return value (or DX:AX for 32-bit)

; OS/2 syscall numbers (from bits/syscall.h.in)
%define SYS_read       1
%define SYS_write      2
%define SYS_open       3
%define SYS_close      4
%define SYS_lseek      5
%define SYS_exit       6
%define SYS_getpid     7
%define SYS_gettime    8
%define SYS_sleep      9
%define SYS_alloc     10
%define SYS_free      11
%define SYS_dup       12
%define SYS_pipe      13
%define SYS_chdir     14
%define SYS_getcwd    15
%define SYS_mkdir     16
%define SYS_rmdir     17
%define SYS_unlink    18
%define SYS_rename    19
%define SYS_stat      20
%define SYS_fstat     21
%define SYS_access    22
%define SYS_fcntl     23
%define SYS_ioctl     24
%define SYS_getenv    25

; Export the syscall entry point
global __os2_syscall0
global __os2_syscall1
global __os2_syscall2
global __os2_syscall3
global __os2_syscall4
global __os2_syscall5
global __os2_syscall6

; ============================================================================
; Syscall dispatch functions
; ============================================================================

; __os2_syscall0(long n) - syscall with 0 arguments
__os2_syscall0:
    push bp
    mov bp, sp
    
    ; Get syscall number from stack
    mov ax, [bp+4]  ; First parameter (syscall number)
    
    ; Dispatch to appropriate handler
    jmp dispatch_syscall

; __os2_syscall1(long n, long a1) - syscall with 1 argument
__os2_syscall1:
    push bp
    mov bp, sp
    
    ; Get syscall number and first argument
    mov ax, [bp+4]  ; Syscall number
    mov bx, [bp+6]  ; First argument
    
    jmp dispatch_syscall

; __os2_syscall2(long n, long a1, long a2) - syscall with 2 arguments
__os2_syscall2:
    push bp
    mov bp, sp
    
    mov ax, [bp+4]  ; Syscall number
    mov bx, [bp+6]  ; First argument
    mov cx, [bp+8]  ; Second argument
    
    jmp dispatch_syscall

; __os2_syscall3(long n, long a1, long a2, long a3) - syscall with 3 arguments
__os2_syscall3:
    push bp
    mov bp, sp
    
    mov ax, [bp+4]  ; Syscall number
    mov bx, [bp+6]  ; First argument
    mov cx, [bp+8]  ; Second argument
    mov dx, [bp+10] ; Third argument
    
    jmp dispatch_syscall

; __os2_syscall4(long n, long a1, long a2, long a3, long a4)
__os2_syscall4:
    push bp
    mov bp, sp
    
    mov ax, [bp+4]  ; Syscall number
    mov bx, [bp+6]  ; First argument
    mov cx, [bp+8]  ; Second argument
    mov dx, [bp+10] ; Third argument
    ; Fourth argument at [bp+12] - push to stack for Dos* calls
    
    jmp dispatch_syscall

; __os2_syscall5(long n, long a1, long a2, long a3, long a4, long a5)
__os2_syscall5:
    push bp
    mov bp, sp
    
    mov ax, [bp+4]  ; Syscall number
    mov bx, [bp+6]  ; First argument
    mov cx, [bp+8]  ; Second argument
    mov dx, [bp+10] ; Third argument
    ; Fourth and fifth arguments on stack
    
    jmp dispatch_syscall

; __os2_syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
__os2_syscall6:
    push bp
    mov bp, sp
    
    mov ax, [bp+4]  ; Syscall number
    mov bx, [bp+6]  ; First argument
    mov cx, [bp+8]  ; Second argument
    mov dx, [bp+10] ; Third argument
    
    jmp dispatch_syscall

; ============================================================================
; Syscall dispatcher
; ============================================================================
dispatch_syscall:
    ; AX = syscall number
    ; BX, CX, DX = first three arguments
    ; Stack = additional arguments
    
    ; Save registers
    push si
    push di
    
    ; Compare syscall number and dispatch
    cmp ax, SYS_read
    je syscall_read
    cmp ax, SYS_write
    je syscall_write
    cmp ax, SYS_open
    je syscall_open
    cmp ax, SYS_close
    je syscall_close
    cmp ax, SYS_exit
    je syscall_exit
    cmp ax, SYS_getpid
    je syscall_getpid
    cmp ax, SYS_sleep
    je syscall_sleep
    cmp ax, SYS_chdir
    je syscall_chdir
    cmp ax, SYS_unlink
    je syscall_unlink
    cmp ax, SYS_mkdir
    je syscall_mkdir
    cmp ax, SYS_rmdir
    je syscall_rmdir
    cmp ax, SYS_rename
    je syscall_rename
    cmp ax, SYS_dup
    je syscall_dup
    cmp ax, SYS_pipe
    je syscall_pipe
    
    ; Unknown syscall - return ENOSYS
    mov ax, -38  ; ENOSYS
    jmp syscall_return

; ============================================================================
; Syscall handlers
; ============================================================================

; SYS_read(fd, buf, count)
; Returns: bytes read in AX, or -1 on error
syscall_read:
    ; Parameters: BX = fd, CX = buf, DX = count
    push dx         ; count
    push cx         ; buf
    push bx         ; fd
    push 0          ; placeholder for bytesRead
    call far DOSREAD
    add sp, 8       ; Clean up stack
    ; DOSREAD returns: AX = 0 on success, bytes read in word at [ss:bp-2]
    ; For simplicity, return AX directly (may need adjustment)
    jmp syscall_return

; SYS_write(fd, buf, count)
; Returns: bytes written in AX, or -1 on error
syscall_write:
    ; Parameters: BX = fd, CX = buf, DX = count
    push dx         ; count
    push cx         ; buf
    push bx         ; fd
    push 0          ; placeholder for bytesWritten
    call far DOSWRITE
    add sp, 8       ; Clean up stack
    jmp syscall_return

; SYS_open(path, flags, mode)
; Returns: file handle in AX, or -1 on error
syscall_open:
    ; This is complex - requires DosOpen with many parameters
    ; For initial port, return ENOSYS
    mov ax, -38     ; ENOSYS
    jmp syscall_return

; SYS_close(fd)
; Returns: 0 on success, -1 on error
syscall_close:
    ; Parameters: BX = fd
    push bx         ; File handle
    call far DOSCLOSE
    add sp, 2
    jmp syscall_return

; SYS_exit(code)
; Does not return
syscall_exit:
    ; Parameters: BX = exit code
    push bx         ; Return code
    push 0          ; Action code 0 = terminate
    call far DOSEXIT
    ; Should never return

; SYS_getpid()
; Returns: process ID in AX
syscall_getpid:
    ; DosGetPID returns a structure, but for simplicity
    ; we'll return a fixed value or use a simplified approach
    mov ax, 1       ; Return PID 1 (simplified)
    jmp syscall_return

; SYS_sleep(milliseconds)
; Returns: 0 on success
syscall_sleep:
    ; Parameters: BX = milliseconds
    push bx         ; Milliseconds
    call far DOSSLEEP
    add sp, 2
    jmp syscall_return

; SYS_chdir(path)
; Returns: 0 on success, -1 on error
syscall_chdir:
    ; Parameters: BX = path
    push bx         ; Directory path
    call far DOSCHDIR
    add sp, 2
    jmp syscall_return

; SYS_unlink(path)
; Returns: 0 on success, -1 on error
syscall_unlink:
    ; Parameters: BX = path
    push bx         ; File path
    call far DOSDELETE
    add sp, 2
    jmp syscall_return

; SYS_mkdir(path)
; Returns: 0 on success, -1 on error
syscall_mkdir:
    ; Parameters: BX = path
    push bx         ; Directory path
    call far DOSMKDIR
    add sp, 2
    jmp syscall_return

; SYS_rmdir(path)
; Returns: 0 on success, -1 on error
syscall_rmdir:
    ; Parameters: BX = path
    push bx         ; Directory path
    call far DOSRMDIR
    add sp, 2
    jmp syscall_return

; SYS_rename(oldpath, newpath)
; Returns: 0 on success, -1 on error
syscall_rename:
    ; Parameters: BX = oldpath, CX = newpath
    push cx         ; New path
    push bx         ; Old path
    call far DOSMOVE
    add sp, 4
    jmp syscall_return

; SYS_dup(fd)
; Returns: new fd in AX, or -1 on error
syscall_dup:
    ; Parameters: BX = fd
    push bx         ; File handle
    call far DOSDUPHANDLE
    add sp, 2
    jmp syscall_return

; SYS_pipe(fildes)
; Returns: 0 on success, -1 on error
syscall_pipe:
    ; Parameters: BX = fildes (array of 2 ints)
    push bx         ; Pipe handles
    call far DOSMAKEPIPE
    add sp, 2
    jmp syscall_return

; ============================================================================
; Return from syscall
; ============================================================================
syscall_return:
    ; Restore registers
    pop di
    pop si
    
    ; Return to caller
    pop bp
    ret

; ============================================================================
; End of OS/2 syscall dispatch
; ============================================================================
