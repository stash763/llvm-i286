// test_mul_print.c - Test multiplication with string output
int mul_test(int a, int b) {
    return a * b;
}

int main() {
    int result = mul_test(100, 200);
    return result & 0xFF;  // Return low byte
}
