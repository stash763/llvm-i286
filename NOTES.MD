We need to create a plan for building a code generator for LLVM IR, we will be targetting the intel 80286 cpu.  
The code generator needs to support the 16-bit protected mode of the 286 processor, and will initially targetting 
the OS/2 1.x operating system.  To help support this will be using the following:

2ine_debugger - fork of 2ine which allows executing OS/2 executables on linux
antlr - source provided
grammars-v4 - contains example antlr grammars if needed

OS/2 support will be used for initial testing and verification of functionality.  once this is completed we 
will be using this to port NetBSD to the 286 processor.  To enable this our code generator must be capable of 
translation the 32-bit operations and addressing that would be used transparently on the 286.  Ie. all 32-bit 
and 64-bit operations must be emulated using 16-bit instructions.  Code which expects a linear address space 
must be translated to a 16-bit segmented on transparantly.  This support for
linear address space translation will be supported for the OS/2 1.x target
as well


We will initially support C17 but with plans to expact this to C++ support.
We will require an FPU and will not be supporting software emulation of one.

The build system for the project should be cmake, we will be implementing
this in C++.