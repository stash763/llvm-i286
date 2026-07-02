; OS/2 1.x Runtime Template for llvm-i286 code generator
; This provides the basic structure needed for OS/2 executables

; Define constants
cr		equ 0x0d
lf		equ 0x0a
stdin		equ 0
stdout		equ 1

; Group data segments
group DGROUP _DATA, _BSS

; External OS/2 API calls
extern  DOSEXIT
extern  DOSWRITE
extern  DOSREAD

; Main code entry point
; This is called by the runtime startup code
; On entry: DS = DGROUP, SS = stack, SP = top of stack
