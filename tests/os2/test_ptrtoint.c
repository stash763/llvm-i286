// Test ptrtoint / inttoptr roundtrip
// Verifies that (inttoptr(ptrtoint(ptr))) yields a working pointer

extern void printnum(char* buffer, int buffersize, int value);

static int g_val = 42;

int main(void) {
    // Get address of g_val via ptrtoint
    long addr = (long)&g_val;

    // Cast back to pointer via inttoptr and read value
    int *ptr = (int *)addr;
    int read_back = *ptr;

    // Print the value read through the roundtrip'd pointer
    char buf[16];
    printnum(buf, 16, read_back);
    return 0;
}
