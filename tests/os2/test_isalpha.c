extern long __os2_syscall3(long n, long a1, long a2, long a3);

int main(void) {
    char buf[8];
    buf[0] = '1';
    buf[1] = 0;
    __os2_syscall3(2, 1, (long)buf, 1);
    return 0;
}
