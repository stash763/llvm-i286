@echo off
nasm -g -f obj -o test.obj test.asm
wlink @test.lk1
