BITS 16

segment _TEXT CLASS=CODE

; Unsigned Divide routine for LLVM to 80286
;
; Caller should provide on the stack
;
; 32-bit dividend - ax:bx
; 32-bit divisor  - cx:dx
;
; Returns
;
; 32-bit quotient in di:si (di=high, si=low)
; 32-bit remainder in ax:bx (ax=low, bx=high)

global _DivideU32

_DivideU32:

		mov	bp,sp			; set the base pointer

                mov	[ss:bp - 4], cx		; save the original divisor low
                mov	[ss:bp - 2], dx		; save the original divisor high

		xor	di,di			; result = 0
                xor	si,si


align_start:	; First align divisor with dividend

		cmp	dx,bx			; while (divisor < dividend)
                jl	shift
                cmp	dx,bx
                jne	div_cont
                cmp	cx,ax
                jae	div_cont

shift:		shl	cx,1			; divisor <<= 1
		rcl	dx,1

                jmp	align_start

div_cont:

div_loop:      	cmp	dx, [ss:bp - 2]		; while (divisor >= original_divisor)
                jg	do_div
                cmp	dx, [ss:bp - 2]
                jne	neg_check
                cmp	cx, [ss:bp - 4]
                jb	neg_check

do_div:		shl	di,1			; result <<= 1
		rcl	si,1

		cmp	bx, dx			; if (dividend >= divisor)
                jg	sub_result
                cmp	bx, dx
                jne	shift_num2
                cmp	ax, cx
                jb	shift_num2

sub_result:     sub	bx, dx			; dividend = dividend - divisor
		sbb	ax, cx

                or	di,1			; result = result | 1
                or	si,0

shift_num2:	sar	dx,1			; divisor >>= 1
		rcr	cx,1

                jmp	div_loop

neg_check:      ; Unsigned: no sign correction needed
                ; Save remainder in cx:dx (cx=low, dx=high)
                mov	cx, ax		; remainder low word
                mov	dx, bx		; remainder high word

div_done:
                retf
