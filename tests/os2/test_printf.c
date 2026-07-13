extern long __os2_syscall3(long n, long a1, long a2, long a3);

int main() {
    char buf[21];
    buf[0] = 'H';
    buf[1] = 'e';
    buf[2] = 'l';
    buf[3] = 'l';
    buf[4] = 'o';
    buf[5] = ',';
    buf[6] = ' ';
    buf[7] = 'w';
    buf[8] = 'o';
    buf[9] = 'r';
    buf[10] = 'l';
    buf[11] = 'd';
    buf[12] = '!';
    buf[13] = ' ';
    buf[14] = 'x';
    buf[15] = ' ';
    buf[16] = '=';
    buf[17] = ' ';
    buf[18] = '4';
    buf[19] = '2';
    buf[20] = 0;
    __os2_syscall3(2, 1, (long)buf, 20);
    return 0;
}
