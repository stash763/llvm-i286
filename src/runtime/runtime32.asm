; 32-Bit Arithmetic Runtime for 80286
; Provides emulation of 32-bit operations using 16-bit instructions
; All operations work on DX:AX (high:low) pairs

; Export all functions
global _MultiplyI32
global _MultiplyU32
global _DivideI32
global _DivideU32
global _ShiftL32
global _ShiftR32
global _ShiftAr32

; ============================================================================
; 32-Bit Signed Multiply
; Input: DX:AX = multiplicand (high:low)
;        CX:BX = multiplier (high:low)
; Output: DI:SI = product (high:low)
; ============================================================================
_MultiplyI32:
    push bp
    mov bp, sp
    
    ; Save registers
    push bx
    push cx
    push dx
    push si
    push di
    
    ; Get operands from stack
    ; [bp+12] = multiplicand low (AX)
    ; [bp+14] = multiplicand high (DX)
    ; [bp+16] = multiplier low (BX)
    ; [bp+18] = multiplier high (CX)
    
    mov ax, [bp+12]  ; Multiplicand low
    mov dx, [bp+14]  ; Multiplicand high
    mov bx, [bp+16]  ; Multiplier low
    mov cx, [bp+18]  ; Multiplier high
    
    ; Perform 32x32 -> 64 bit multiply (we only need lower 32 bits)
    ; Multiply low words
    mul bx           ; AX = AX * BX (low*low)
    mov si, ax       ; SI = low word of result
    
    ; Cross multiply and add
    mov ax, dx       ; Get multiplicand high
    mul bx           ; AX = high * low
    mov di, ax       ; DI = partial product
    
    mov ax, si       ; Get low result
    mul cx           ; AX = low * high
    add di, ax       ; Add to partial product
    
    ; Handle carries
    ; (simplified - full implementation would handle all carries)
    
    ; Result in DI:SI
    pop di
    pop si
    pop dx
    pop cx
    pop bx
    pop bp
    ret

; ============================================================================
; 32-Bit Unsigned Multiply
; Same as signed multiply for the lower 32 bits
; ============================================================================
_MultiplyU32:
    jmp _MultiplyI32

; ============================================================================
; 32-Bit Signed Divide
; Input: DX:AX = dividend (high:low)
;        CX:BX = divisor (high:low)
; Output: DX:AX = quotient (high:low)
;         CX:BX = remainder (high:low)
; ============================================================================
_DivideI32:
    push bp
    mov bp, sp
    
    ; Save registers
    push si
    push di
    
    ; For 32-bit division on 80286, we need to implement long division
    ; This is a simplified version using repeated subtraction
    ; A full implementation would use a more efficient algorithm
    
    ; Get operands
    mov si, ax       ; Dividend low
    mov di, dx       ; Dividend high
    mov cx, bx       ; Divisor low
    ; Divisor high is in CX:BX originally, but we need to handle this properly
    
    ; Simplified: assume divisor fits in 16 bits (BX)
    ; Check if divisor is negative
    test dx, dx
    js .dividend_negative
    
    test cx, cx
    js .divisor_negative
    
    ; Both positive - perform unsigned division
    mov ax, si
    mov dx, di
    call .unsigned_divide
    jmp .done

.dividend_negative:
    ; Negate dividend
    neg si
    sbb di, 0
    mov ax, si
    mov dx, di
    call .unsigned_divide
    ; Negate quotient
    neg ax
    sbb dx, 0
    jmp .done

.divisor_negative:
    ; Negate divisor
    neg cx
    sbb bx, 0
    mov ax, si
    mov dx, di
    call .unsigned_divide
    ; Negate quotient
    neg ax
    sbb dx, 0
    
.done:
    ; Result in DX:AX (quotient)
    pop di
    pop si
    pop bp
    ret

.unsigned_divide:
    ; Perform unsigned 32/16 division
    ; AX = quotient low, DX = quotient high
    ; Simplified - assumes divisor fits in 16 bits
    div cx           ; AX = quotient, DX = remainder
    mov cx, bx       ; Clear high word
    mov bx, dx       ; Remainder in BX
    ret

; ============================================================================
; 32-Bit Unsigned Divide
; ============================================================================
_DivideU32:
    push bp
    mov bp, sp
    
    push si
    push di
    
    ; Similar to signed divide but without sign handling
    mov si, ax
    mov di, dx
    mov cx, bx
    
    ; Perform division
    mov ax, si
    mov dx, di
    div cx           ; AX = quotient, DX = remainder
    mov cx, bx       ; Remainder high = 0
    mov bx, dx       ; Remainder low
    
    ; Result in DX:AX (quotient), CX:BX (remainder)
    pop di
    pop si
    pop bp
    ret

; ============================================================================
; 32-Bit Left Shift
; Input: DX:AX = value (high:low)
;        CX = shift count
; Output: DX:AX = result (high:low)
; ============================================================================
_ShiftL32:
    push bp
    mov bp, sp
    
    push cx
    push si
    
    ; Get operands
    mov ax, [bp+8]   ; Value low
    mov dx, [bp+10]  ; Value high
    mov cx, [bp+12]  ; Shift count
    
    ; Handle shift count > 16
    cmp cx, 16
    jge .shift_ge16
    
    ; Shift < 16 bits
    shl ax, cl
    rcl dx, cl
    jmp .done

.shift_ge16:
    ; Shift >= 16 bits
    sub cx, 16
    mov ax, dx       ; Move high to low
    xor dx, dx       ; Clear high
    shl ax, cl       ; Shift remaining bits
    
.done:
    pop si
    pop cx
    pop bp
    ret

; ============================================================================
; 32-Bit Logical Right Shift
; Input: DX:AX = value (high:low)
;        CX = shift count
; Output: DX:AX = result (high:low)
; ============================================================================
_ShiftR32:
    push bp
    mov bp, sp
    
    push cx
    push si
    
    ; Get operands
    mov ax, [bp+8]   ; Value low
    mov dx, [bp+10]  ; Value high
    mov cx, [bp+12]  ; Shift count
    
    ; Handle shift count > 16
    cmp cx, 16
    jge .shift_ge16
    
    ; Shift < 16 bits
    shr dx, cl
    rcr ax, cl
    jmp .done

.shift_ge16:
    ; Shift >= 16 bits
    sub cx, 16
    mov dx, ax       ; Move low to high
    xor ax, ax       ; Clear low
    shr dx, cl       ; Shift remaining bits
    
.done:
    pop si
    pop cx
    pop bp
    ret

; ============================================================================
; 32-Bit Arithmetic Right Shift
; Input: DX:AX = value (high:low)
;        CX = shift count
; Output: DX:AX = result (high:low) (sign-extended)
; ============================================================================
_ShiftAr32:
    push bp
    mov bp, sp
    
    push cx
    push si
    
    ; Get operands
    mov ax, [bp+8]   ; Value low
    mov dx, [bp+10]  ; Value high
    mov cx, [bp+12]  ; Shift count
    
    ; Check if negative (high word sign bit set)
    test dx, 0x8000
    jz .shift_positive
    
    ; Negative - need to sign-extend
    ; Set high bits when shifting
    cmp cx, 16
    jge .shift_ar_ge16
    
    ; Shift < 16 bits with sign extension
    sar dx, cl
    rcr ax, cl
    jmp .done

.shift_ar_ge16:
    ; Shift >= 16 bits
    sub cx, 16
    mov ax, dx       ; Move high to low
    mov dx, 0xFFFF   ; Fill high with 1s (sign extension)
    sar ax, cl       ; Shift remaining bits
    jmp .done

.shift_positive:
    ; Positive - same as logical shift
    cmp cx, 16
    jge .shift_ar_ge16_pos
    
    shr dx, cl
    rcr ax, cl
    jmp .done

.shift_ar_ge16_pos:
    sub cx, 16
    mov dx, ax
    xor ax, ax
    shr dx, cl

.done:
    pop si
    pop cx
    pop bp
    ret

; ============================================================================
; End of 32-bit arithmetic runtime
; ============================================================================
