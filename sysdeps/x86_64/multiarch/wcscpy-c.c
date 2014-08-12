#if IS_IN_MODULE (libc)
# define wcscpy  __wcscpy_sse2
#endif

#include "wcsmbs/wcscpy.c"
