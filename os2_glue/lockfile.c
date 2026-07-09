#include "stdio_impl.h"

hidden int __lockfile(FILE *f)
{
	if ((f->lock < 0) || !__sync_bool_compare_and_swap(&f->lock, 0, -1))
		return 0;
	return 1;
}

hidden void __unlockfile(FILE *f)
{
	f->lock = 0;
}
