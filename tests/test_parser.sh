#!/bin/bash
# Test script for LLVM IR parser
# Tests that the parser can handle basic LLVM IR constructs

set -e

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
BUILD_DIR="$PROJECT_DIR/build"
OUTPUT_DIR="$SCRIPT_DIR/output"

# Configuration
CLANG="${CLANG:-clang}"
LLVM_I286="$BUILD_DIR/llvm-i286"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

echo "=== LLVM IR Parser Tests ==="
echo ""

# Test counter
PASS=0
FAIL=0
TOTAL=0

run_parser_test() {
    local test_name="$1"
    local test_file="$2"
    local expected_func="$3"
    
    TOTAL=$((TOTAL + 1))
    echo -n "Test $TOTAL: $test_name... "
    
    local temp_dir="$OUTPUT_DIR/$test_name"
    mkdir -p "$temp_dir"
    
    # Run the parser
    if ! "$LLVM_I286" "$test_file" -o "$temp_dir/output.asm" 2>"$temp_dir/parser_err.txt"; then
        echo -e "${RED}FAIL (parser error)${NC}"
        echo "  Error: $(cat $temp_dir/parser_err.txt)"
        FAIL=$((FAIL + 1))
        return
    fi
    
    # Check that output contains expected function
    if [ -n "$expected_func" ]; then
        if grep -q "global $expected_func" "$temp_dir/output.asm"; then
            echo -e "${GREEN}PASS${NC}"
            PASS=$((PASS + 1))
        else
            echo -e "${RED}FAIL (missing function)${NC}"
            echo "  Expected function '$expected_func' not found in output"
            FAIL=$((FAIL + 1))
        fi
    else
        echo -e "${GREEN}PASS${NC}"
        PASS=$((PASS + 1))
    fi
}

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Test 1: Simple return 0
cat > "$OUTPUT_DIR/test1.ll" <<'EOF'
define i16 @main() {
entry:
  ret i16 0
}
EOF
run_parser_test "simple_return_0" "$OUTPUT_DIR/test1.ll" "main"

# Test 2: Return constant
cat > "$OUTPUT_DIR/test2.ll" <<'EOF'
define i32 @test_func() {
entry:
  ret i32 42
}
EOF
run_parser_test "return_constant" "$OUTPUT_DIR/test2.ll" "test_func"

# Test 3: Function with parameters
cat > "$OUTPUT_DIR/test3.ll" <<'EOF'
define i32 @add(i32 %a, i32 %b) {
entry:
  %result = add i32 %a, %b
  ret i32 %result
}
EOF
run_parser_test "function_with_params" "$OUTPUT_DIR/test3.ll" "add"

# Test 4: Multiple functions
cat > "$OUTPUT_DIR/test4.ll" <<'EOF'
declare i32 @printf(ptr, ...)

define i32 @main() {
entry:
  %result = call i32 (ptr, ...) @printf(ptr null)
  ret i32 0
}
EOF
run_parser_test "multiple_functions" "$OUTPUT_DIR/test4.ll" "main"

# Test 5: clang-generated IR
cat > "$OUTPUT_DIR/test5.c" <<'EOF'
int main(void) {
    return 0;
}
EOF
$CLANG -S -emit-llvm -o "$OUTPUT_DIR/test5.ll" "$OUTPUT_DIR/test5.c"
run_parser_test "clang_generated" "$OUTPUT_DIR/test5.ll" "main"

echo ""
echo "=== Results ==="
echo "Total: $TOTAL"
echo -e "Pass: ${GREEN}$PASS${NC}"
echo -e "Fail: ${RED}$FAIL${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo "All parser tests passed!"
    exit 0
else
    echo "Some parser tests failed."
    exit 1
fi
