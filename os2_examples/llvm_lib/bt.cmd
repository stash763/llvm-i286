@echo off
nasm -g -f obj -o div.obj div.asm
nasm -g -f obj -o mul.obj mul.asm
rem nasm -g -f obj -o itoa.obj itoa.asm
nasm -g -f obj -o printstr.obj printstr.asm

wlib llvm.lib div.obj mul.obj
