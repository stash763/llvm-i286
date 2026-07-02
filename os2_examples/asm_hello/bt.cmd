@echo off
nasm -g -f obj -l test.lst -o test.obj test.asm
link /co test,test,,C:\OS2TK45\lib\os2286,test,
