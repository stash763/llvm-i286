// test_mul_local.c - Test multiplication with local variables
int main() {
    int a = 100;
    int b = 200;
    int result = a * b;
    return result & 0xFF;  // Return low byte
}
