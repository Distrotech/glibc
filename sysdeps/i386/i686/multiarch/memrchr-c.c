#if IS_IN_MODULE (libc)
# define MEMRCHR  __memrchr_ia32
# include <string.h>
extern void *__memrchr_ia32 (const void *, int, size_t);
#endif

#include "string/memrchr.c"
