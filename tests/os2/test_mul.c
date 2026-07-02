// test_mul.c - Test multiplication with local variables
int mul_test(int a, int b) {
    return a * b;
}

int main() {
    int a = 100;
    int b = 200;
    int result = mul_test(a, b);
    return result;
}
