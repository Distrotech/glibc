#include <sys/select.h>

extern int __call_pselect6 (int nfds, fd_set *readfds, fd_set *writefds,
			    fd_set *exceptfds, const struct timespec *timeout,
			    void *data) attribute_hidden;


#define __CALL_PSELECT6(nfds, readfds, writefds, exceptfds, timeout, data) \
  ({ int r = __call_pselect6 (nfds, readfds, writefds, exceptfds, timeout,    \
			      data);					      \
     if (r < 0 && r > -4096)						      \
       {								      \
	 __set_errno (-r);						      \
	 r = -1;							      \
       }								      \
     r; })

#define CALL_PSELECT6(nfds, readfds, writefds, exceptfds, timeout, data) \
  ({									      \
    int __r;								      \
    if (SINGLE_THREAD_P)						      \
      __r = __CALL_PSELECT6 (nfds, readfds, writefds, exceptfds, timeout,     \
                             data);					      \
    else								      \
      {									      \
	int oldtype = LIBC_CANCEL_ASYNC ();				      \
	__r = __CALL_PSELECT6 (nfds, readfds, writefds, exceptfds, timeout,   \
			       data);					      \
	LIBC_CANCEL_RESET (oldtype);					      \
      }									      \
   __r;									      \
  })

#include "../pselect.c"
