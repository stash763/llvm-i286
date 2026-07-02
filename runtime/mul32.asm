; NASM example to create a 16-bit OS/2 segmented NE executable


BITS 16

true		equ 1
false		equ 0


segment _TEXT CLASS=CODE

; Multiply routine for LLVM to 80286

; Caller
;
; 32-bit number 1 - ax:bx
; 32-bit number 2 - cx:dx
;
; Returns
;
; 32-bit result in di:si

global _MultiplyI32

_MultiplyI32:

	        mov 	bp,sp
                mov	[ss:bp - 2], word false
	      	;mov	di,false       		; set false

		; Test if the answer is +/-
                ; change all negative numbers to postive
                cmp	bx,0			; if (num1 < 0)
                jl	t1_neg
                jmp	t1_pos
t1_neg:         mov	[ss:bp - 2], word true
		;mov	di,true			; set true if negative
		neg	bx			; num1 = -num1
		neg	ax
                sbb	bx,0
t1_pos:		cmp	dx,0			; if (num2 < 0)
                jl	t2_neg
                jmp	t2_pos
t2_neg:         xor	[ss:bp - 2], word true
		;xor	di,true			; if true already becomes false
		neg	dx			; num2 = -num2
		neg	cx
                sbb	dx,0

t2_pos:		;push	di			; Save sign result to stack

		xor	di,di			; result = 0
                xor	si,si

mul_start:     	cmp	cx,0			; while (num2 > 0)
                jne	comp32_end
                cmp	dx,0
comp32_end:	jnz	mul_cont
		jmp	mul_end

mul_cont:	test	cx,1			; if num2 & 0x0001
		je	skip_add

                add	si,bx  			; result += num1
                adc	di,ax

skip_add:	shl	ax,1			; num1 <<= 1
		rcl	bx,1

                sar	cx,1			; num2 >>= 1
                rcr	dx,1

                jmp	mul_start
mul_end:	; result is in di:si

		mov     ax, [ss:bp - 2] 	; is negative = true
                cmp	ax,true
                jne	mul_done

                neg si				; result = -result
                neg di
                sbb si,0
mul_done:

		retf