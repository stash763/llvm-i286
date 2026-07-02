#!/bin/bash

# Build script for the runtime library
# Assembles runtime .asm files and creates runtime.lib using wlib

# Set OpenWatcom environment variables
export PATH=$PATH:~/ow/open-watcom-v2/rel/binl64
export WATCOM=~/ow/open-watcom-v2/rel

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

echo "=== Building runtime library ==="

# Assemble each .asm file
for asm_file in "$SCRIPT_DIR"/*.asm; do
    obj_file="${asm_file%.asm}.obj"
    echo "Assembling: $(basename "$asm_file")"
    nasm -g -f obj -o "$obj_file" "$asm_file"
    if [ $? -ne 0 ]; then
        echo "Error: nasm assembly failed for $(basename "$asm_file")"
        exit 1
    fi
done

# Create library using wlib
echo "Creating runtime.lib..."
wlib -c "$SCRIPT_DIR/runtime.lib" "$SCRIPT_DIR"/*.obj
if [ $? -ne 0 ]; then
    echo "Error: wlib failed"
    exit 1
fi

echo "=== Runtime library built: $SCRIPT_DIR/runtime.lib ==="