// Test basic syscall functionality - test __os2_syscall3 with write()
extern long __os2_syscall3(long n, long a1, long a2, long a3);
extern void printnum(char* buffer, int buffersize, int value);

int main() {
    char buf[16];
    
    // Test 1: Write 'X' to stdout
    char c = 'X';
    long result = __os2_syscall3(2, 1, (long)&c, 1);
    
    // Test 2: Write 'Y' to stdout
    char d = 'Y';
    result = __os2_syscall3(2, 1, (long)&d, 1);
    
    // Print the return value from the last syscall (should be 1)
    printnum(buf, 16, (int)result);
    
    return 0;
}
