#define USE_AVX2
#define AS_STRNCMP
#define __strcmp_sse2_unaligned __strncmp_avx2
#include "strcmp-sse2-unaligned.S"
