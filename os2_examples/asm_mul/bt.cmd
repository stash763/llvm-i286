@echo off
nasm -g -f obj -l test.lst -o test.obj test.asm
link /co test,test,,d:\toolkit\lib\os2286,test,
