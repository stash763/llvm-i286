extern void printnum(char* buffer, int buffersize, int value);

int main(void) {
    char buf[16];
    printnum(buf, 16, 42);
    return 0;
}