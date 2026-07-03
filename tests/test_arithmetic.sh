#!/bin/bash
# Test script for basic arithmetic operations
# Tests the full pipeline: LLVM IR -> NASM -> object file

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"
OUTPUT_DIR="$SCRIPT_DIR/output"

# Configuration
LLVM_I286="$BUILD_DIR/llvm-i286"
NASM="${NASM:-nasm}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

echo "=== Basic Arithmetic Operation Tests ==="
echo ""

PASS=0
FAIL=0
TOTAL=0

run_test() {
    local test_name="$1"
    local test_file="$2"
    
    TOTAL=$((TOTAL + 1))
    echo -n "Test $TOTAL: $test_name... "
    
    local temp_dir="$OUTPUT_DIR/$test_name"
    mkdir -p "$temp_dir"
    
    # Run code generator
    if ! "$LLVM_I286" "$test_file" -o "$temp_dir/output.asm" 2>"$temp_dir/codegen_err.txt"; then
        echo -e "${RED}FAIL (codegen error)${NC}"
        cat "$temp_dir/codegen_err.txt"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Assemble with NASM
    if ! $NASM -f obj -o "$temp_dir/output.o" "$temp_dir/output.asm" 2>"$temp_dir/nasm_err.txt"; then
        echo -e "${RED}FAIL (nasm error)${NC}"
        cat "$temp_dir/nasm_err.txt"
        FAIL=$((FAIL + 1))
        return
    fi
    
    echo -e "${GREEN}PASS${NC}"
    PASS=$((PASS + 1))
}

mkdir -p "$OUTPUT_DIR"

# Test 1: i16 add
cat > "$OUTPUT_DIR/test_add.ll" <<'EOF'
define i16 @add_test(i16 %a, i16 %b) {
entry:
  %result = add i16 %a, %b
  ret i16 %result
}
EOF
run_test "i16_add" "$OUTPUT_DIR/test_add.ll"

# Test 2: i16 sub
cat > "$OUTPUT_DIR/test_sub.ll" <<'EOF'
define i16 @sub_test(i16 %a, i16 %b) {
entry:
  %result = sub i16 %a, %b
  ret i16 %result
}
EOF
run_test "i16_sub" "$OUTPUT_DIR/test_sub.ll"

# Test 3: i16 mul
cat > "$OUTPUT_DIR/test_mul.ll" <<'EOF'
define i32 @mul_test(i16 %a, i16 %b) {
entry:
  %result = mul i16 %a, %b
  ret i16 %result
}
EOF
run_test "i16_mul" "$OUTPUT_DIR/test_mul.ll"

# Test 4: i16 and
cat > "$OUTPUT_DIR/test_and.ll" <<'EOF'
define i16 @and_test(i16 %a, i16 %b) {
entry:
  %result = and i16 %a, %b
  ret i16 %result
}
EOF
run_test "i16_and" "$OUTPUT_DIR/test_and.ll"

# Test 5: i16 or
cat > "$OUTPUT_DIR/test_or.ll" <<'EOF'
define i16 @or_test(i16 %a, i16 %b) {
entry:
  %result = or i16 %a, %b
  ret i16 %result
}
EOF
run_test "i16_or" "$OUTPUT_DIR/test_or.ll"

# Test 6: i16 xor
cat > "$OUTPUT_DIR/test_xor.ll" <<'EOF'
define i16 @xor_test(i16 %a, i16 %b) {
entry:
  %result = xor i16 %a, %b
  ret i16 %result
}
EOF
run_test "i16_xor" "$OUTPUT_DIR/test_xor.ll"

# Test 7: Multiple operations
cat > "$OUTPUT_DIR/test_multi.ll" <<'EOF'
define i16 @multi_test(i16 %a, i16 %b, i16 %c) {
entry:
  %ab = add i16 %a, %b
  %result = sub i16 %ab, %c
  ret i16 %result
}
EOF
run_test "multiple_ops" "$OUTPUT_DIR/test_multi.ll"

# Test 8: Return constant
cat > "$OUTPUT_DIR/test_const.ll" <<'EOF'
define i16 @const_test() {
entry:
  ret i16 42
}
EOF
run_test "return_constant" "$OUTPUT_DIR/test_const.ll"

# Test 9: Musl memcpy.c (full pipeline: clang → llvm-i286 → nasm)
MUSL_MEMCPY="$PROJECT_DIR/musl/src/string/i286/memcpy.c"
if [ -f "$MUSL_MEMCPY" ]; then
    TOTAL=$((TOTAL + 1))
    echo -n "Test $TOTAL: musl_memcpy_c... "
    
    MUSL_MEMCPY_DIR="$OUTPUT_DIR/musl_memcpy_c"
    mkdir -p "$MUSL_MEMCPY_DIR"
    
    # Compile with clang to LLVM IR
    if ! clang -target i386-elf -S -emit-llvm -O0 \
         "$MUSL_MEMCPY" \
         -o "$MUSL_MEMCPY_DIR/memcpy.ll" 2>"$MUSL_MEMCPY_DIR/clang_err.txt"; then
        echo -e "${RED}FAIL (clang error)${NC}"
        cat "$MUSL_MEMCPY_DIR/clang_err.txt"
        FAIL=$((FAIL + 1))
    # Run code generator
    elif ! "$LLVM_I286" "$MUSL_MEMCPY_DIR/memcpy.ll" -o "$MUSL_MEMCPY_DIR/output.asm" 2>"$MUSL_MEMCPY_DIR/codegen_err.txt"; then
        echo -e "${RED}FAIL (codegen error)${NC}"
        cat "$MUSL_MEMCPY_DIR/codegen_err.txt"
        FAIL=$((FAIL + 1))
    # Assemble with NASM
    elif ! $NASM -f obj -o "$MUSL_MEMCPY_DIR/output.o" "$MUSL_MEMCPY_DIR/output.asm" 2>"$MUSL_MEMCPY_DIR/nasm_err.txt"; then
        echo -e "${RED}FAIL (nasm error)${NC}"
        cat "$MUSL_MEMCPY_DIR/nasm_err.txt"
        FAIL=$((FAIL + 1))
    else
        echo -e "${GREEN}PASS${NC}"
        PASS=$((PASS + 1))
    fi
else
    echo -e "${YELLOW}SKIP (musl memcpy.c not found)${NC}"
fi

echo ""
echo "=== Results ==="
echo "Total: $TOTAL"
echo -e "Pass: ${GREEN}$PASS${NC}"
echo -e "Fail: ${RED}$FAIL${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo "All arithmetic tests passed!"
    exit 0
else
    echo "Some arithmetic tests failed."
    exit 1
fi
