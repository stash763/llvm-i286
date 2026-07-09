#include "stdio_impl.h"

// Direct call to our OS/2 syscall layer
extern long __os2_syscall3(long n, long a1, long a2, long a3);

static size_t stdout_write(FILE *f, const unsigned char *buf, size_t len)
{
	// SYS_write = 2, fd=1 (stdout)
	long ret = __os2_syscall3(2, 1, (long)buf, (long)len);
	if (ret < 0) return 0;
	return (size_t)ret;
}

// Simple zero-initialized FILE object
static FILE stdout_file;

hidden FILE *volatile __stdout_used = &stdout_file;

// Initialization function - sets up the write callback and fd
// We avoid struct literal initialization which the codegen can't handle
void __stdio_init(void)
{
	// Set stdout_file.write = stdout_write
	// Set stdout_file.fd = 1
	// Set stdout_file.lock = -1
	// These will be done via pointer arithmetic in the codegen
	// For now, this is a stub
}
