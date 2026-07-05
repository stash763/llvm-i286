extern void printnum(char* buffer, int buffersize, int value);

int add(int a, int b) {
    return a + b;
}

int main(void) {
    char buf[16];
    int result = add(10, 20);
    printnum(buf, 16, result);
    return 0;
}