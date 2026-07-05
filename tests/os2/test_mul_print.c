extern void printnum(char* buffer, int buffersize, int value);

int mul_test(int a, int b) {
    return a * b;
}

int main() {
    char buf[16];
    int result = mul_test(100, 200);
    printnum(buf, 16, result & 0xFF);
    return 0;
}