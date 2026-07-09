#!/bin/bash
# musl_cc.sh - Compile musl source file through llvm-i286 pipeline
#
# Usage: ./musl_cc.sh <input.c> [output.o]
#
# Compiles a musl C source file to an object file using clang_i286

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
CLANG_I286="$SCRIPT_DIR/clang_i286"

if [ $# -lt 1 ]; then
    echo "Usage: $0 <input.c> [output.o]" >&2
    exit 1
fi

INPUT="$1"
OUTPUT="${2:-${INPUT%.c}.o}"

if [ ! -f "$INPUT" ]; then
    echo "Error: input file '$INPUT' not found" >&2
    exit 1
fi

echo "Compiling: $INPUT -> $OUTPUT"

# Compile using clang_i286 with musl include paths
# Order matters: musl/src/include must come BEFORE musl/include
# so that src/include/features.h (which defines 'hidden') is found first
# Use -nostdinc to prevent system headers from conflicting with musl headers
MUSL_INCLUDES=(
    "-nostdinc"
    "-I${SCRIPT_DIR}/musl/src/include"
    "-I${SCRIPT_DIR}/musl/src"
    "-I${SCRIPT_DIR}/musl/src/internal"
    "-I${SCRIPT_DIR}/musl/include"
    "-I${SCRIPT_DIR}/musl/arch/i286"
    "-I${SCRIPT_DIR}/musl/arch/generic"
)

if ! "$CLANG_I286" "${MUSL_INCLUDES[@]}" -c "$INPUT" -o "$OUTPUT" 2>&1; then
    echo "Error: compilation failed for '$INPUT'" >&2
    exit 1
fi

echo "Success: $OUTPUT"
