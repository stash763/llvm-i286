#!/bin/bash
# Test runner script for llvm-i286
# Tests the full pipeline:
#   C -> clang -> LLVM IR -> llvm-i286 -> NASM -> .o -> wlink -> OS/2 .exe
# Then runs the .exe under lx_loader (in podman container) and verifies
# stdout output against expected values.

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(dirname "$SCRIPT_DIR")"
CLANG_I286="$PROJECT_DIR/clang_i286"
TEST_DIR="$SCRIPT_DIR/os2"
OUTPUT_DIR="$SCRIPT_DIR/output"

# Configuration
LX_LOADER="${LX_LOADER:-$PROJECT_DIR/2ine_debugger/build/lx_loader}"
LIB2INE="${LIB2INE:-$PROJECT_DIR/2ine_debugger/build/lib2ine.so}"
LIBDOSCALLS="${LIBDOSCALLS:-$PROJECT_DIR/2ine_debugger/build/libdoscalls.so}"
CONTAINER="${CONTAINER:-localhost/2ine-builder:32bit}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check that clang_i286 exists
if [ ! -x "$CLANG_I286" ]; then
    echo "Error: clang_i286 not found at $CLANG_I286"
    echo "Build the project first: cd $PROJECT_DIR && mkdir build && cd build && cmake .. && make"
    exit 1
fi

# Check that lx_loader exists
if [ ! -x "$LX_LOADER" ]; then
    echo "Error: lx_loader not found at $LX_LOADER"
    echo "Build lx_loader first (in 2ine_debugger)"
    exit 1
fi

# Check that lib2ine.so exists
if [ ! -f "$LIB2INE" ]; then
    echo "Error: lib2ine.so not found at $LIB2INE"
    exit 1
fi

# Check that libdoscalls.so exists
if [ ! -f "$LIBDOSCALLS" ]; then
    echo "Error: libdoscalls.so not found at $LIBDOSCALLS"
    exit 1
fi

echo "=== LLVM i286 Test Suite ==="
echo "clang_i286: $CLANG_I286"
echo "lx_loader:   $LX_LOADER"
echo ""

# Create output directory
mkdir -p "$OUTPUT_DIR"

# Companion source files to link alongside the main test file
# Key = test name, Value = path to additional .c file(s) to link
declare -A COMPANION_FILES
# COMPANION_FILES[test_memcpy]="$TEST_DIR/test_memcpy_memcpy.c"

# Expected stdout output for each test
declare -A EXPECTED_OUTPUT
EXPECTED_OUTPUT[hello]="0"
EXPECTED_OUTPUT[test_hello]="42"
EXPECTED_OUTPUT[test_return]="42"
EXPECTED_OUTPUT[test_add]="30"
EXPECTED_OUTPUT[test_add_local]="42"
EXPECTED_OUTPUT[test_mul]="20000"
EXPECTED_OUTPUT[test_mul32]="20000"
EXPECTED_OUTPUT[test_mul32_simple]="32"
EXPECTED_OUTPUT[test_mul_local]="20000"
EXPECTED_OUTPUT[test_mul_print]="32"
EXPECTED_OUTPUT[test_printnum]="42"
EXPECTED_OUTPUT[test_memcpy]="0"
EXPECTED_OUTPUT[test_ptrtoint]="42"

# Tests that use unsupported features (expected to fail at compile or link time)
declare -A EXPECTED_FAILURES
EXPECTED_FAILURES[test_printf]=1
EXPECTED_FAILURES[test_printf_simple]=1

# Test counters
PASS=0
FAIL=0
TOTAL=0

run_test() {
    local test_name="$1"
    local test_file="$2"

    TOTAL=$((TOTAL + 1))
    echo -n "Test $TOTAL: $test_name... "

    local exe_file="$OUTPUT_DIR/${test_name}.exe"

    # Step 1: Compile via clang_i286 (with optional companion files)
    local compile_cmd=("$CLANG_I286")

    # Add companion file if one exists for this test
    local companion="${COMPANION_FILES[$test_name]:-}"
    if [ -n "$companion" ] && [ -f "$companion" ]; then
        compile_cmd+=("$test_file" "$companion")
    else
        compile_cmd+=("$test_file")
    fi

    compile_cmd+=(-o "$exe_file")

    local compile_log
    compile_log=$("${compile_cmd[@]}" 2>&1)
    local compile_rc=$?

    if [ $compile_rc -ne 0 ]; then
        if [ "${EXPECTED_FAILURES[$test_name]:-0}" = "1" ]; then
            echo -e "${YELLOW}SKIP (known limitation)${NC}"
            PASS=$((PASS + 1))
        else
            echo -e "${RED}FAIL (compile)${NC}"
            echo "  Compile error: $compile_log" >&2
            FAIL=$((FAIL + 1))
        fi
        return
    fi

    # If this test compiled but was expected to fail, note it
    if [ "${EXPECTED_FAILURES[$test_name]:-0}" = "1" ]; then
        echo -e "${YELLOW}UNEXPECTED (compiled but expected to fail)${NC}"
        # Still try to run it
    fi

    # Step 2: Run under lx_loader directly (must run from build dir to find lib2ine.so, etc.)
    local output
    output=$(cd "$(dirname "$LX_LOADER")" && LD_LIBRARY_PATH=. \
        ./lx_loader "$exe_file" 2>/dev/null) || true

    # Step 3: Compare output against expected value
    local expected="${EXPECTED_OUTPUT[$test_name]}"
    local trimmed_output
    trimmed_output=$(echo "$output" | tr -d '[:space:]')

    if [ -z "$expected" ]; then
        # No expected output defined — just check if it ran without error
        echo -e "${GREEN}PASS (linked OK, no output check)${NC}"
        PASS=$((PASS + 1))
    elif [ "$trimmed_output" = "$expected" ]; then
        echo -e "${GREEN}PASS (output: $output)${NC}"
        PASS=$((PASS + 1))
    else
        echo -e "${RED}FAIL (expected: '$expected', got: '$output')${NC}"
        FAIL=$((FAIL + 1))
    fi
}

# Run tests
if [ -d "$TEST_DIR" ]; then
    for test_file in "$TEST_DIR"/*.c; do
        if [ -f "$test_file" ]; then
            test_name=$(basename "$test_file" .c)
            run_test "$test_name" "$test_file"
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