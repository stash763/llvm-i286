extern long __os2_syscall3(long n, long a1, long a2, long a3);

static int g_val = 42;

int main(void) {
    char buf[8];
    buf[0] = '4';
    buf[1] = '2';
    buf[2] = 0;
    __os2_syscall3(2, 1, (long)buf, 2);
    return 0;
}
