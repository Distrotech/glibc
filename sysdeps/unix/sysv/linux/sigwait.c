/* Copyright (C) 1997-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, see
   <http://www.gnu.org/licenses/>.  */

#include <errno.h>
#include <signal.h>
#define __need_NULL
#include <stddef.h>
#include <string.h>

#include <nptl/pthreadP.h>
#include <sysdep-cancel.h>
#include <sys/syscall.h>

#ifdef __NR_rt_sigtimedwait

/* Return any pending signal or wait for one for the given time.  */
static int
do_sigwait (const sigset_t *set, int *sig)
{
  int ret;

#ifdef SIGCANCEL
  sigset_t tmpset;
  if (set != NULL
      && (__builtin_expect (__sigismember (set, SIGCANCEL), 0)
# ifdef SIGSETXID
	  || __builtin_expect (__sigismember (set, SIGSETXID), 0)
# endif
	  ))
    {
      /* Create a temporary mask without the bit for SIGCANCEL set.  */
      // We are not copying more than we have to.
      memcpy (&tmpset, set, _NSIG / 8);
      __sigdelset (&tmpset, SIGCANCEL);
# ifdef SIGSETXID
      __sigdelset (&tmpset, SIGSETXID);
# endif
      set = &tmpset;
    }
#endif

  /* XXX The size argument hopefully will have to be changed to the
     real size of the user-level sigset_t.  */
  do
    ret = SYSCALL_CANCEL_NCS (rt_sigtimedwait, set, NULL, NULL, _NSIG / 8);
  while (SYSCALL_CANCEL_ERROR (ret)
	 && SYSCALL_CANCEL_ERRNO (ret) == EINTR);
  if (!SYSCALL_CANCEL_ERROR (ret))
    {
      *sig = ret;
      ret = 0;
    }
  else
    ret = SYSCALL_CANCEL_ERRNO (ret);

  return ret;
}

int
__sigwait (const sigset_t *set, int *sig)
{
  return do_sigwait (set, sig);
}
libc_hidden_def (__sigwait)
weak_alias (__sigwait, sigwait)
#else
# include <sysdeps/posix/sigwait.c>
#endif
strong_alias (__sigwait, __libc_sigwait)
