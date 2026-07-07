// Test memcmp - verify memory comparison
extern void printnum(char* buffer, int buffersize, int value);

int my_memcmp(const void *s1, const void *s2, int n)
{
    const unsigned char *p1 = s1, *p2 = s2;
    for (int i = 0; i < n; i++) {
        if (p1[i] != p2[i])
            return p1[i] - p2[i];
    }
    return 0;
}

int main(void) {
    char buf[16];
    const char *s1 = "hello";
    const char *s2 = "hello";
    int result = my_memcmp(s1, s2, 5);
    printnum(buf, 16, result);  // Should print 0 (equal)
    return 0;
}
