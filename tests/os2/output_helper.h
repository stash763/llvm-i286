#ifndef OUTPUT_HELPER_H
#define OUTPUT_HELPER_H

extern long __os2_syscall3(long n, long a1, long a2, long a3);

static void print_num(int v, char *b) {
    int i = 0;
    if (v == 0) {
        b[0] = '0';
        b[1] = 0;
        return;
    }
    if (v < 0) {
        b[i++] = '-';
        v = -v;
    }
    char tmp[12];
    int ti = 0;
    while (v > 0) {
        tmp[ti++] = '0' + (v % 10);
        v /= 10;
    }
    while (ti > 0) {
        b[i++] = tmp[--ti];
    }
    b[i] = 0;
}

static long s_len(const char *s) {
    long l = 0;
    while (s[l]) l++;
    return l;
}

#endif
