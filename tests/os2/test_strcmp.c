// Test strcmp - verify string comparison
extern void printnum(char* buffer, int buffersize, int value);

int my_strcmp(const char *l, const char *r)
{
    for (; *l==*r && *l; l++, r++);
    return *(unsigned char *)l - *(unsigned char *)r;
}

int main(void) {
    char buf[16];
    const char *s1 = "hello";
    const char *s2 = "hello";
    int result = my_strcmp(s1, s2);
    printnum(buf, 16, result);  // Should print 0 (equal)
    return 0;
}
