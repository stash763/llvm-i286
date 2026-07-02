; manual compilation of test_count.ll

BITS 16

group DGROUP _DATA

        	extern   DosExit

segment _DATA CLASS=DATA 			; Static data


segment	STACK STACK
		; this is defined in .def file
		resb 4096


segment _TEXT CLASS=CODE

..start:        mov 	bp,sp

		; First allocate local variables, no code generated.  Compiler must
                ; keep track of locations on the stack which will represent %1, %2, %3

                ; %1 = alloca i32	[bp - 4]	 - This is return value for main
                ; %2 = alloca i32	[bp - 8]	 - int c
                ; %3 = alloca i32	[bp - 12]        - int value

                ; store i32 0, i32* %1

		mov	ax, 0x0000
                mov	bx, 0x0000

                mov	[ss:bp - 4], ax
                mov	[ss:bp - 2], bx

                ; store i32 0, i32* %2

		mov	ax, 0x0000
                mov	bx, 0x0000

                mov	[ss:bp - 8], ax
                mov	[ss:bp - 6], bx

                ; store i32 0, i32* %3

		mov	ax, 0x0000
                mov	bx, 0x0000

                mov	[ss:bp - 12], ax
                mov	[ss:bp - 10], bx

                jmp label4

label4:
		; %5 = load i32, i32* %2
                
		mov	ax, [ss:bp - 8]			; load %2 variable
                mov	bx, [ss:bp - 6]

                ; %6 = icmp slt i32 %5, 10

                mov	cx, 0x000A			; load number to compare
                mov	dx, 0x0000

                cmp	ax,cx				; compare the 32-bit values
                jne	comp32_end1
                cmp	bx,dx

comp32_end1:
		; br i1 %6, label %7, label %13

                jnz	label7 			       ; jump to label 7 if zf = 1
                jmp	label13			       ; else jump to label13


label7:
		; %8 = load i32, i32* %3

                mov	dx, [ss:bp - 12]
                mov	ax, [ss:bp - 10]
                
                ; %9 = add nsw i32 %8, 1

                mov	cx, 0x0001
                mov	dx, 0x0000

                add	ax,bx
                adc	dx,cx
                
                ; store i32 %9, i32* %3

                mov	[ss:bp - 12], dx
                mov	[ss:bp - 10], ax
                
                ; br label %10

                jmp label10

label10:
		; %11 = load i32, i32* %2

		mov	dx, [ss:bp - 8]			; load %2 variable
                mov	ax, [ss:bp - 6]
                
                ; %12 = add nsw i32 %11, 1

                mov	cx, 0x0001
                mov	bx, 0x0000

                add	ax,bx
                adc	dx,cx

                ; store i32 %12, i32* %2

                mov	[ss:bp - 8], dx			; store %2 variable
                mov	[ss:bp - 6], ax
                
                ; br label %4

                jmp label4

label13:

		; %14 = load i32, i32* %1

                mov	ax, [ss:bp - 4]			; load %2 variable
                mov	bx, [ss:bp - 2]

exit:
		push    1               	; terminate process
                ; modify the following line to use %14
		push    0               	; return code
		call	far DosExit

segment _DATA

segment end
