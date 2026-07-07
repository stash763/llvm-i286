// Test isalpha - verify character classification
extern void printnum(char* buffer, int buffersize, int value);

int my_isalpha(int c)
{
    int r = (c|32)-'a';
    return r < 26;
}

int main(void) {
    char buf[16];
    int result = my_isalpha('A');
    printnum(buf, 16, result);
    return 0;
}
