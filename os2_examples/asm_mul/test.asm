; NASM example to create a 16-bit OS/2 segmented NE executable


BITS 16

cr		equ 0x0d
lf		equ 0x0a
stdin		equ 0
stdout		equ 1
true		equ 1
false		equ 0

group DGROUP _DATA

        	extern   DosExit
	        extern   DosRead
	        extern   DosWrite
                extern	 DosAllocSeg


segment _DATA CLASS=DATA 			; Static data

msg1		db 'Hello World',cr,lf
msg1_len	equ $-msg1

msg_pos		db 'Positive',cr,lf
msg_pos_len	equ $-msg_pos

msg_neg		db 'Negative',cr,lf
msg_neg_len	equ $-msg_neg

num1		resb 12

num_start	dw 0
num_len		dw 0

rlen		dw 0
wlen		dw 0
n1		dd 100000
n2		dd -2
nresult		dd 0

segment	STACK STACK
		; this is defined in .def file
		resb 4096


segment _TEXT CLASS=CODE

..start:        mov 	bp,sp

multiply:      	mov	di,false       		; set false

		; num1 = n1
                mov	ax,[n1]
                mov	bx,[n1 + 2]

                ; num2 = n2
                mov	cx,[n2]
                mov	dx,[n2 + 2]
		
		; Test if the answer is +/-
                ; change all negative numbers to postive
                cmp	bx,0			; if (num1 < 0)
                jl	t1_neg
                jmp	t1_pos
t1_neg:         mov	di,true			; set true if negative
		neg	bx			; num1 = -num1
		neg	ax
                sbb	bx,0
t1_pos:		cmp	dx,0			; if (num2 < 0)
                jl	t2_neg
                jmp	t2_pos
t2_neg:		xor	di,true			; if true already becomes false
		neg	dx			; num2 = -num2
		neg	cx
                sbb	dx,0

t2_pos:		push	di			; Save sign result to stack

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

		pop	ax			; is negative = true
                cmp	ax,true
                jne	mul_done

                neg si				; result = -result
                neg di
                sbb si,0
mul_done:

                ; print the number

		; Convert a number to a string
                ; the following parameters are return values
                push	ds                      ; beginning of string
                push	num_start
                push	ds			; length of string 
                push	num_len
                ; Input parameters
                ;push	dx			; push MSB
                ;push	cx		        ; push LSB
                push	si			; MSB
                push	di			; LSB
                push	ds                      ; far pointer to buffer
                push	num1 
		call	far itoa
                add	sp,16			; clean up the stack pointer

		; display the string
                push	stdout
                push	ds
                mov	ax,[num_start]
                push	ax
                mov 	ax,[num_len]
                push 	ax
                push	ds
                push	wlen
                call	far DosWrite

exit:
		push    1               	; terminate process
		push    0               	; return code
		call	far DosExit

                ; *********** Convert integer to string ***********
		;
                ; ==== Call parameters on stack
                ;
		; 4 bytes - return address
                ; 4 bytes - pointer to buffer
                ; 4 bytes - 32-bit number
                ; 4 bytes - pointer to dw number length
                ; 4 bytes - pointer to dw string start

itoa:           mov bp,sp			; save the current stack pointer
		add sp,4			; move to the start of parameters

		pop di				; far pointer to buffer
		pop es

                pop bx				; load the 32-bit integer
                pop dx

                mov cx,0x000b
                mov al,' '
                repz stosb
                mov [es:di],cl
                push di				; end of the string
                mov ax,bx
                mov si,0x000a
                or dx,dx
                pushf
                jns lts_cont
                not ax
                not dx
                add ax,1
                adc dx,+00

lts_cont:       mov cx,ax
		mov ax,dx
                xor dx,dx
                div si
                mov bx,ax
                mov ax,cx
                div si
                xchg dx,bx
                add bl,'0'
                dec di
                mov [es:di],bl
                mov bx,ax
                or bx,dx
                jnz lts_cont
                dec di
                mov al,' '
                popf
                jns notneg			; if not negative start exit
                mov al,'-'
                mov [es:di],al
                jmp lts_exit			; '-' added exit now
notneg:		inc di                          ; This wasn't negative move the start of string

lts_exit:       pop dx                          ; end of string

                pop si				; get pointer to num_len
                pop es		
		sub dx,di			; find length
                mov [es:si],dx			; save length

                pop si
                pop es
                mov [es:si],di			; save start of string

                mov sp,bp			; restore the stack pointer
		retf

segment _DATA

segment end
