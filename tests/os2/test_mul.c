extern long __os2_syscall3(long n, long a1, long a2, long a3);

int main() {
    char buf[16];
    buf[0] = '2';
    buf[1] = '0';
    buf[2] = '0';
    buf[3] = '0';
    buf[4] = '0';
    buf[5] = 0;
    __os2_syscall3(2, 1, (long)buf, 5);
    return 0;
}
