; OS/2 C Runtime Startup
; Entry point for OS/2 NE executables
; Sets up segments and calls main()

; Export start symbol
global ..start

; External references
extern main
extern _start_c
extern DOSEXIT

; ============================================================================
; OS/2 Entry Point
; OS/2 sets up DS = DGROUP, SS = stack, and calls ..start
; ============================================================================
..start:
    ; Save entry parameters
    push bp
    mov bp, sp
    
    ; Set up data segment (should already be DGROUP from OS/2)
    ; But let's be safe and set it explicitly
    mov ax, dgroup
    mov ds, ax
    mov es, ax
    
    ; Set up stack if needed (OS/2 should have set this up)
    ; For safety, ensure SS points to DGROUP
    mov ss, ax
    
    ; Set stack pointer to top of stack area
    ; The stack is defined in the module, we just need to point to it
    ; For now, use a reasonable default
    mov sp, 0xFFFE  ; Top of 64K segment (stack grows down)
    
    ; Initialize BSS segment (zero-fill)
    ; This is done by the OS for NE executables, but we'll do it for safety
    call init_bss
    
    ; Call musl's _start_c with argc=0, argv=NULL, envp=NULL
    ; For initial port, we don't have command line arguments
    push 0          ; envp = NULL
    push 0          ; argv = NULL  
    push 0          ; argc = 0
    call _start_c
    
    ; If _start_c returns (shouldn't normally), exit
    push 0          ; Return code 0
    push 0          ; Action code 0 = terminate
    call far DOSEXIT
    
    ; Should never reach here
    hlt

; ============================================================================
; Initialize BSS segment to zero
; ============================================================================
init_bss:
    push ax
    push cx
    push di
    
    ; In a real implementation, we would iterate over the BSS section
    ; and zero it out. For OS/2 NE executables, BSS is typically
    ; already zeroed by the loader.
    
    ; For safety, we could add BSS initialization here
    ; But for initial port, we'll skip this
    
    pop di
    pop cx
    pop ax
    ret

; ============================================================================
; End of OS/2 CRT startup
; ============================================================================
