#include <rt/mqueue.h>

#if defined _LIBC && IS_IN_MODULE (librt)
hidden_proto (mq_timedsend)
hidden_proto (mq_timedreceive)
hidden_proto (mq_setattr)
#endif
