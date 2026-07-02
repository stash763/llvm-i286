#!/bin/bash
# Build musl libc for i286 OS/2 target
# This script builds musl using the configured toolchain

set -e

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
MUSL_DIR="$SCRIPT_DIR/musl"
BUILD_DIR="$SCRIPT_DIR/build/musl"

echo "=== Building musl libc for i286 OS/2 ==="
echo ""

# Check if config.mak exists
if [ ! -f "$MUSL_DIR/config.mak" ]; then
    echo "Error: config.mak not found in $MUSL_DIR"
    echo "Please run configure_i286.sh first"
    exit 1
fi

# Create build directory
mkdir -p "$BUILD_DIR"

echo "Build directory: $BUILD_DIR"
echo ""

# Copy config.mak to build directory
cp "$MUSL_DIR/config.mak" "$BUILD_DIR/"

# Build musl
echo "Building musl..."
cd "$BUILD_DIR"

# Set CC to our wrapper script
export CC="$SCRIPT_DIR/musl_cc.sh"

# Run make
make -C "$MUSL_DIR" \
    CC="$CC" \
    AR="ar" \
    RANLIB="ranlib" \
    all

echo ""
echo "=== Build Complete ==="
echo ""
echo "Output:"
echo "  Static library: $MUSL_DIR/lib/libc.a"
echo ""
echo "To use this library:"
echo "  - Link with: -L$MUSL_DIR/lib -lc"
echo "  - Include headers from: $MUSL_DIR/include"
echo ""
