; itoa - Convert 32-bit integer to ASCII string
; Based on the working implementation from os2_examples/asm_libtest/test.asm
;
; Stack parameters (pushed right-to-left by caller):
;   push ds          ; start segment (return: start offset written here)
;   push num_start   ; start offset
;   push ds          ; length segment (return: length written here)
;   push num_len     ; length offset
;   push MSB         ; high word of value
;   push LSB         ; low word of value
;   push ds          ; buffer segment
;   push buf_offset  ; buffer offset
;   call far itoa
;   add sp, 16       ; caller cleans up
;
; Returns via memory: writes start offset to [ds:num_start], length to [ds:num_len]

BITS 16

segment _TEXT CLASS=CODE

global itoa

itoa:
    mov bp, sp
    add sp, 4            ; skip far return address

    pop di              ; buffer offset
    pop es              ; buffer segment

    pop bx              ; LSB (low word)
    pop dx              ; MSB (high word)

    mov cx, 0x000b      ; 12 chars
    mov al, ' '
    repz stosb
    mov [es:di], cl      ; null terminator
    push di             ; save end position

    mov ax, bx
    mov si, 0x000a      ; divisor = 10
    or dx, dx
    pushf
    jns lts_cont
    not ax
    not dx
    add ax, 1
    adc dx, 0

lts_cont:
    mov cx, ax
    mov ax, dx
    xor dx, dx
    div si
    mov bx, ax
    mov ax, cx
    div si
    xchg dx, bx
    add bl, '0'
    dec di
    mov [es:di], bl
    mov bx, ax
    or bx, dx
    jnz lts_cont

    dec di
    mov al, ' '
    popf
    jns notneg
    mov al, '-'
    mov [es:di], al
    jmp lts_exit

notneg:
    inc di

lts_exit:
    pop dx              ; end of string

    pop si              ; num_len offset
    pop es              ; num_len segment
    sub dx, di          ; length = end - start
    mov [es:si], dx     ; save length

    pop si              ; num_start offset
    pop es              ; num_start segment
    mov [es:si], di     ; save start of string

    mov sp, bp
    retf