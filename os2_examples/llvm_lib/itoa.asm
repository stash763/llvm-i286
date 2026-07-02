BITS 16


segment _TEXT CLASS=CODE

; Divide routine for LLVM to 80286

; Caller should provide on the stack
;
; 32-bit number
; 32-bit divisor  - cx:dx
;
; Returns
;
; 32-bit result in di:si

global _DivideI32

_itoa:

                ; Stack used by routine
                ; sp - 2 & sp - 4	: copy of abs(num2)
                ; sp - 6	  	: true/false of if answer is negative
                
		mov	bp,sp			; set the base pointer

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

		; TODO something is wrong here
                ; pointers on stack are incorrect

                pop si				; get pointer to num_len
                pop es		
		sub dx,di			; find length
                mov [es:si],dx			; save length

                pop si
                pop es
                mov [es:si],di			; save start of string

                mov sp,bp			; restore the stack pointer


                retf
