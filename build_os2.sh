#!/bin/bash

# Build script for compiling LLVM i286 codegen output to OS/2 1.x executables
# Based on 2ine_debugger/tests/asm/build.sh - uses exact same OpenWatcom setup

# Set OpenWatcom environment variables (REQUIRED for wlink to find OS/2 system definitions)
export PATH=$PATH:~/ow/open-watcom-v2/rel/binl64
export WATCOM=~/ow/open-watcom-v2/rel
export INCLUDE=~/ow/open-watcom-v2/rel/h

# Check arguments
if [ $# -lt 2 ]; then
    echo "Usage: $0 <input.asm> <output.exe>"
    echo ""
    echo "Example:"
    echo "  $0 tests/output/test_hello.asm test_hello.exe"
    exit 1
fi

INPUT_ASM="$1"
OUTPUT_EXE="$2"

echo "=== LLVM i286 OS/2 Build Script ==="
echo "Input:  $INPUT_ASM"
echo "Output: $OUTPUT_EXE"
echo ""

# Step 1: Assemble with nasm (using same flags as working example)
echo "Step 1: Assembling with nasm..."
OBJ_FILE="${OUTPUT_EXE%.exe}.o"
LIST_FILE="${OUTPUT_EXE%.exe}.lst"
nasm -g -f obj -l "$LIST_FILE" -o "$OBJ_FILE" "$INPUT_ASM"

if [ $? -ne 0 ]; then
    echo "Error: nasm assembly failed!"
    exit 1
fi

echo "  Created: $OBJ_FILE"
echo ""

# Step 2: Link with wlink (exact same command as working example in 2ine_debugger/tests/asm/build.sh)
echo "Step 2: Linking with wlink..."
SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
wlink system os2 d all path ~/ow/open-watcom-v2/rel/lib286:~/ow/open-watcom-v2/lib286/os2 library ~/ow/open-watcom-v2/lib286/os2/os2.lib library "$SCRIPT_DIR/runtime/runtime.lib" name "$OUTPUT_EXE" file "$PWD/$OBJ_FILE"

if [ $? -ne 0 ]; then
    echo "Error: wlink linking failed!"
    exit 1
fi

echo ""
echo "=== Build Complete ==="
echo "Executable: $OUTPUT_EXE"
echo ""
echo "To run with lx_loader:"
echo "  cd 2ine_debugger/build"
echo "  LD_LIBRARY_PATH=. ./lx_loader ../../$OUTPUT_EXE"
