#define USE_AVX2
#define __strcmp_sse2_unaligned __strcmp_avx2
#include "strcmp-sse2-unaligned.S"
