; NASM example to create a 16-bit OS/2 segmented NE executable


BITS 16

cr		equ 0x0d
lf		equ 0x0a
stdin		equ 0
stdout		equ 1

group DGROUP _DATA

        	extern   DosExit
	        extern   DosRead
	        extern   DosWrite
                extern	 DosAllocSeg


segment _DATA CLASS=DATA 			; Static data

msg1		db 'Hello World',cr,lf
msg1_len	equ $-msg1

num1		resb 12

num_start	dw 0
num_len		dw 0

rlen		dw 0
wlen		dw 0

n1		dd 100000
n2		dd 100001
nresult		dd 0

segment	STACK STACK
		; this is defined in .def file
		resb 4096


segment _TEXT CLASS=CODE

..start:        mov 	bp,sp

		; Example of playing the value 100000 on the stack
                ; this has to take into account little endian byte ordering

                ;mov	eax,100000
                ;mov 	[n1], eax
                ;mov	eax,0
                ;mov	dx,[n1]
                ;mov	ax,[n1 + 2]

                ;mov	eax,100001
                ;mov 	[n1], eax
                ;mov	eax,0
                ;mov	dx,[n1]
                ;mov	ax,[n1 + 2]

                ;mov	eax,200000
                ;mov 	[n1], eax
                ;mov	eax,0
                ;mov	dx,[n1]
                ;mov	ax,[n1 + 2]

                ;mov	eax,200001
                ;mov 	[n1], eax
                ;mov	eax,0
                ;mov	dx,[n1]
                ;mov	ax,[n1 + 2]


                ;mov	[ss:bp - 4], word 0x86A0
                ;mov	[ss:bp - 2], word 0x0001

                ;mov	eax,0xFFFFFFFF
                ;mov 	[nresult], eax
                ;mov	eax,10
                ;mov	[nresult], eax

		; Add two 32-bit numbers
		mov	dx,[n1]
                mov	ax,[n1 + 2]
                mov	cx,[n2]
                mov     bx,[n2 + 2]
                add	ax,bx
                adc	dx,cx
                mov	[nresult],dx
                mov	[nresult + 2],ax
                mov	eax,0
                mov	eax,[nresult]

                ; Subtract

                mov	dx,[nresult]		; Get the result value
                mov	ax,[nresult + 2]
                mov	cx,[n1]			; Get the value 100000
                mov	bx,[n1 + 2]
                sub	ax,bx
                sbb	dx,cx
                mov	[nresult],dx
                mov	[nresult+2],ax
                mov	eax,0
                mov	eax,[nresult]

                ; multiply
                ; divide
                
                ; compare
                mov	ax,[n1]			; 1st number
                mov	bx,[n1 + 2]
                mov	cx,[n2]			; 2nd number
                mov	dx,[n2 + 2]
                cmp	ax,cx
                jnz	comp32_end
                cmp	bx,dx
comp32_end:	; zf = 1 if equal
		; > zf = 0 carry = 0
                ; < zf = 0 carry = 1
                
                

                ;  Add parameters to stack and then call OS
                
		push 	stdout			; specifying stdout
                push	ds                      ; Address of string ds:address
		push	msg1
                mov	cx,msg1_len             ; length of string
                push	cx
                push	ds                      ; Address to store bytes written ds:address
                push	wlen
                call    far DosWrite

		; Convert a number to a string
                ; the following parameters are return values
                push	ds                      ; beginning of string
                push	num_start
                push	ds			; length of string 
                push	num_len
                ; Input parameters
		mov     ax,0			; push MSB
                push	ax
                mov	ax,123                  ; push LSB
                push	ax
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

segment _DATA

segment end
