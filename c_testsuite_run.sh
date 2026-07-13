#!/bin/bash
# c-testsuite runner for llvm-i286
# Runs all c-testsuite tests and reports results

set -u

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
TESTSUITE_DIR="$SCRIPT_DIR/c-testsuite"
RUNNER="$TESTSUITE_DIR/runners/single-exec/llvm-i286"
SKIP_LIST="$TESTSUITE_DIR/runners/single-exec/llvm-i286.skip"
TESTS_DIR="$TESTSUITE_DIR/tests/single-exec"

# Colors
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Check runner exists
if [ ! -x "$RUNNER" ]; then
    echo "ERROR: runner not found at $RUNNER"
    echo "Please ensure c-testsuite runner is executable"
    exit 1
fi

# Load skip list
declare -A SKIP_TESTS
if [ -f "$SKIP_LIST" ]; then
    while IFS= read -r line; do
        # Skip comments and empty lines
        [[ "$line" =~ ^#.*$ ]] && continue
        [[ -z "$line" ]] && continue
        # Extract test number (first field)
        test_num=$(echo "$line" | awk '{print $1}')
        reason=$(echo "$line" | cut -d' ' -f2-)
        if [ -n "$test_num" ]; then
            SKIP_TESTS["$test_num"]="$reason"
        fi
    done < "$SKIP_LIST"
fi

echo "=== c-testsuite Runner for llvm-i286 ==="
echo "Test directory: $TESTS_DIR"
echo "Runner: $RUNNER"
echo "Skip list: $SKIP_LIST"
echo ""

# Count tests
TOTAL=0
PASS=0
FAIL=0
SKIP=0

# Run each test
for test_file in "$TESTS_DIR"/*.c; do
    [ -f "$test_file" ] || continue
    
    TOTAL=$((TOTAL + 1))
    BASENAME=$(basename "$test_file")
    TEST_NUM=$(echo "$BASENAME" | sed 's/\.c$//')
    
    # Check if this test is in the skip list
    if [ -n "${SKIP_TESTS[$TEST_NUM]:-}" }; then
        SKIP=$((SKIP + 1))
        echo -e "${YELLOW}SKIP${NC} $TEST_NUM: ${SKIP_TESTS[$TEST_NUM]}"
        continue
    fi
    
    echo -n "Test $TEST_NUM... "
    
    # Run the test
    if "$RUNNER" "$test_file" > /dev/null 2>&1; then
        PASS=$((PASS + 1))
        echo -e "${GREEN}PASS${NC}"
    else
        FAIL=$((FAIL + 1))
        echo -e "${RED}FAIL${NC}"
    fi
done

echo ""
echo "=== Results ==="
echo -e "Total:  $TOTAL"
echo -e "Pass:   ${GREEN}$PASS${NC}"
echo -e "Fail:   ${RED}$FAIL${NC}"
echo -e "Skip:   ${YELLOW}$SKIP${NC}"
echo ""

if [ $FAIL -eq 0 ]; then
    echo "All non-skipped tests passed!"
    exit 0
else
    echo "Some tests failed."
    exit 1
fi
