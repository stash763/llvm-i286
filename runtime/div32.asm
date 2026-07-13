BITS 16

true	equ 1
false	equ 0

segment _TEXT CLASS=CODE

; Divide routine for LLVM to 80286

; Caller should provide on the stack
;
; 32-bit dividend - ax:bx
; 32-bit divisor  - cx:dx
;
; Returns
;
; 32-bit result in di:si

global _DivideI32

_DivideI32:

                ; Stack used by routine
                ; sp - 2 & sp - 4	: copy of abs(num2)
                ; sp - 6	  	: true/false of if answer is negative
                
		mov	bp,sp			; set the base pointer

                mov	[ss:bp - 6], word false	; variable for if number is negative

		; Test if the answer is +/-
                ; change all negative numbers to postive
                cmp	bx,0			; if (num1 < 0)
                jl	t1_neg
                jmp	t1_pos
t1_neg:		mov	[ss:bp - 6], word true	; set true if negative
		neg	bx			; num1 = -num1
		neg	ax
                sbb	bx,0
t1_pos:		cmp	dx,0			; if (num2 < 0)
                jl	t2_neg
                jmp	t2_pos
t2_neg:		xor	[ss:bp - 6], word true  ; if true already becomes false
		neg	dx			; num2 = -num2
		neg	cx
                sbb	dx,0


t2_pos:         mov	[ss:bp - 4], cx		; save the original number
                mov	[ss:bp - 2], dx

		xor	di,di			; result = 0
                xor	si,si


align_start:	; First align num2 with num1

		cmp	dx,bx			; while (num2 < num1)
                jl	shift
                cmp	dx,bx
                jne	div_cont
                cmp	cx,ax
                jae	div_cont
                
shift:		shl	cx,1			; num2 <<= 1
		rcl	dx,1

                jmp	align_start

div_cont:

div_loop:      	cmp	dx, [ss:bp - 2]		; while (num2 >= original_num2)
                jg	do_div
                cmp	dx, [ss:bp - 2]
                jne	neg_check
                cmp	cx, [ss:bp - 4]
                jb	neg_check

do_div:		shl	di,1			; result <<= 1
		rcl	si,1

		cmp	bx, dx			; if (num1 >= num2)
                jg	sub_result
                cmp	bx, dx
                jne	shift_num2
                cmp	ax, cx
                jb	shift_num2

sub_result:     sub	bx, dx			; num1 = num1 - num2
		sbb	ax, cx

                or	di,1			; result = result | 1
                or	si,0

shift_num2:	sar	dx,1			; num2 >>= 1
		rcr	cx,1

                jmp	div_loop

               ; Save remainder in cx:dx before neg_check modifies ax
                mov	cx, ax		; save remainder low word
                mov	dx, bx		; save remainder high word

neg_check:      mov	ax, [ss:bp - 6]		; restore negative true/false off the stack
		cmp	ax,0			; if (negative)
                je 	div_done

                neg	si			; result = -result
                neg	di
                sbb	si,0
div_done:
                ; Restore remainder in ax:bx
                mov	ax, cx
                mov	bx, dx
                retf
