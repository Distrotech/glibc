/* No floating-point inline functions in rtld.  */
#if !IS_IN_MODULE (rtld)
# include <stdlib/bits/stdlib-float.h>
#endif
