// test_mul32.c - Test 32-bit multiplication
int mul_test(int a, int b) {
    return a * b;
}

int main() {
    int result = mul_test(100, 200);
    return result & 0xFF;  // Return low byte
}
