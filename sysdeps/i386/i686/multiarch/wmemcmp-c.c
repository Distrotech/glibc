#include <wchar.h>

#if IS_IN_MODULE (libc)
# define WMEMCMP  __wmemcmp_ia32
#endif

extern __typeof (wmemcmp) __wmemcmp_ia32;

#include "wcsmbs/wmemcmp.c"
