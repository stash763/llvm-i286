; printnum - Print a 32-bit signed integer to stdout
; Based on the itoa + DosWrite pattern from os2_examples/asm_libtest/test.asm
;
; Stack parameters (pushed right-to-left by caller, near call):
;   [bp+4]  = buffer offset (ds-relative)
;   [bp+6]  = buffer size low word (ignored, we use 12)
;   [bp+8]  = buffer size high word (ignored)
;   [bp+10] = value low word (LSB)
;   [bp+12] = value high word (MSB)
; Caller cleans up: add sp, 10

BITS 16

segment _TEXT CLASS=CODE

global printnum

extern itoa
extern DOSWRITE

printnum:
    push bp
    mov bp, sp
    push bx
    push si
    push di

    ; Get parameters from stack
    mov di, [bp+4]      ; buffer offset
    mov ax, [bp+10]     ; value low word (LSB)
    mov dx, [bp+12]     ; value high word (MSB)

    ; Call itoa to convert value to ASCII string
    ; itoa parameters (pushed right-to-left):
    ;   push ds          ; start segment (return)
    ;   push _pn_num_start; start offset (return)
    ;   push ds          ; length segment (return)
    ;   push _pn_num_len ; length offset (return)
    ;   push dx          ; MSB (high word)
    ;   push ax          ; LSB (low word)
    ;   push ds          ; buffer segment
    ;   push di          ; buffer offset
    ;   call far itoa
    ;   add sp, 16
    push ds
    push _pn_num_start
    push ds
    push _pn_num_len
    push dx             ; MSB
    push ax             ; LSB
    push ds
    push di             ; buffer offset
    call far itoa
    add sp, 16

    ; Display the string via DosWrite
    ; DosWrite parameters (pushed right-to-left, Pascal convention - callee cleans):
    ;   push stdout      ; file handle (1)
    ;   push ds           ; buffer segment
    ;   push [num_start]  ; start offset
    ;   push [num_len]    ; length
    ;   push ds           ; wlen segment
    ;   push _pn_wlen    ; wlen offset
    ;   call far DosWrite
    push 1              ; stdout
    push ds
    mov ax, [_pn_num_start]
    push ax
    mov ax, [_pn_num_len]
    push ax
    push ds
    push _pn_wlen
   call far DOSWRITE

    pop di
    pop si
    pop bx
    pop bp
    ret

segment _DATA CLASS=DATA

_pn_num_start dw 0
_pn_num_len   dw 0
_pn_wlen      dw 0