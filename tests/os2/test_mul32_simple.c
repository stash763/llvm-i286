extern void printnum(char* buffer, int buffersize, int value);

int main() {
    char buf[16];
    int result = 20000 & 0xFF;
    printnum(buf, 16, result);
    return 0;
}