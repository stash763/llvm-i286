extern void printnum(char* buffer, int buffersize, int value);

int main() {
    char buf[16];
    int a = 100;
    int b = 200;
    int result = a * b;
    printnum(buf, 16, result);
    return 0;
}