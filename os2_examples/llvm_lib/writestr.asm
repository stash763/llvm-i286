BITS 16

	        extern   DosWrite
                ; DosWrite(int16 handle, ptr32 string, int16 length, ptr32 byteswritten)
                ; Returns
                ;
                ; O_ERROR
                ; 5 ERROR_ACCESS_DENIED
                ; 6 ERROR_INVALID_HANDLE
                ; 26 ERROR_NOT_DOS_DISK
                ; 33 ERROR_LOCK_VIOLATION
                ; 109 ERROR_BROKEN_PIPE 



segment _TEXT CLASS=CODE

; Print null terminated string
;
; PrintStr(char *str);

global _PrintStr

_printstr:

                ; Stack used by routine
                ; sp - 2 & sp - 4	: copy of abs(num2)
                ; sp - 6	  	: true/false of if answer is negative

                push 	sp			; current stack pointer has the return address
                sub	sp, 4

	        call	far DosWrite

	       	pop sp				; Restore sp to return address

                retf
