// Test strlen - verify string length calculation
extern void printnum(char* buffer, int buffersize, int value);

int my_strlen(const char *s)
{
    int len = 0;
    while (*s) {
        len++;
        s++;
    }
    return len;
}

int main(void) {
    char buf[16];
    const char *msg = "hello";
    int len = my_strlen(msg);
    printnum(buf, 16, len);
    return 0;
}
