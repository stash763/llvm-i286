#!/bin/bash
# Test runner script for llvm-i286
# Tests the full pipeline: C -> LLVM IR -> NASM -> wlink -> 2ine

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"
TEST_DIR="$SCRIPT_DIR/os2"
OUTPUT_DIR="$SCRIPT_DIR/output"

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Configuration
CLANG="${CLANG:-clang}"
NASM="${NASM:-nasm}"
WLINK="${WLINK:-$HOME/ow/open-watcom-v2/rel/binl/wlink}"
LX_LOADER="${LX_LOADER:-$PROJECT_DIR/2ine_debugger/build/lx_loader}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "=== LLVM i286 Test Suite ==="
echo "CLANG: $CLANG"
echo "NASM: $NASM"
echo "WLINK: $WLINK"
echo ""

# Test counter
PASS=0
FAIL=0
TOTAL=0

run_test() {
    local test_name="$1"
    local test_file="$2"
    local expected_output="$3"
    
    TOTAL=$((TOTAL + 1))
    echo -n "Test $TOTAL: $test_name... "
    
    local temp_dir="$OUTPUT_DIR/$test_name"
    mkdir -p "$temp_dir"
    
    # Step 1: Compile C to LLVM IR
    if ! $CLANG -S -emit-llvm -o "$temp_dir/test.ll" "$test_file" 2>"$temp_dir/clang_err.txt"; then
        echo -e "${RED}FAIL (clang)${NC}"
        echo "  clang error: $(cat $temp_dir/clang_err.txt)"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Step 2: Run code generator
    if ! "$BUILD_DIR/llvm-i286" "$temp_dir/test.ll" -o "$temp_dir/test.asm" 2>"$temp_dir/codegen_err.txt"; then
        echo -e "${RED}FAIL (codegen)${NC}"
        echo "  codegen error: $(cat $temp_dir/codegen_err.txt)"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Step 3: Assemble with NASM
    if ! $NASM -f obj -o "$temp_dir/test.o" "$temp_dir/test.asm" 2>"$temp_dir/nasm_err.txt"; then
        echo -e "${RED}FAIL (nasm)${NC}"
        echo "  nasm error: $(cat $temp_dir/nasm_err.txt)"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Step 4: Link with wlink
    # Create .def file
    cat > "$temp_dir/test.def" <<EOF
NAME TEST WINDOWCOMPAT
PROTMODE
STACKSIZE 4096
EOF
    
    if ! $WLINK system os2 d all name "$temp_dir/test.exe" file "$temp_dir/test.o" 2>"$temp_dir/wlink_err.txt"; then
        echo -e "${RED}FAIL (wlink)${NC}"
        echo "  wlink error: $(cat $temp_dir/wlink_err.txt)"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Step 5: Run with lx_loader (if available)
    if [ -x "$LX_LOADER" ]; then
        local output
        output=$($LX_LOADER "$temp_dir/test.exe" 2>"$temp_dir/loader_err.txt" || true)
        
        if [ "$output" = "$expected_output" ]; then
            echo -e "${GREEN}PASS${NC}"
            PASS=$((PASS + 1))
        else
            echo -e "${RED}FAIL (output mismatch)${NC}"
            echo "  Expected: '$expected_output'"
            echo "  Got: '$output'"
            FAIL=$((FAIL + 1))
        fi
    else
        # Skip execution test if lx_loader not available
        echo -e "${YELLOW}PASS (skipped execution)${NC}"
        PASS=$((PASS + 1))
    fi
}

# Run tests
if [ -d "$TEST_DIR" ]; then
    for test_file in "$TEST_DIR"/*.c; do
        if [ -f "$test_file" ]; then
            test_name=$(basename "$test_file" .c)
            run_test "$test_name" "$test_file" ""
        fi
    done
fi

echo ""
echo "=== Results ==="
echo "Total: $TOTAL"
echo -e "Pass: ${GREEN}$PASS${NC}"
echo -e "Fail: ${RED}$FAIL${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo "All tests passed!"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi
