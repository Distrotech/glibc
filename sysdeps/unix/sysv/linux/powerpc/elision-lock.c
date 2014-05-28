/* elision-lock.c: Elided pthread mutex lock.
   Copyright (C) 2013 Free Software Foundation, Inc.
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

#include <stdio.h>
#include <pthread.h>
#include <pthreadP.h>
#include <lowlevellock.h>
#include <elision-conf.h>
#include "htm.h"

#if !defined(LLL_LOCK) && !defined(EXTRAARG)
/* Make sure the configuration code is always linked in for static
   libraries.  */
#include "elision-conf.c"
#endif

#ifndef EXTRAARG
# define EXTRAARG
#endif
#ifndef LLL_LOCK
# define LLL_LOCK(a,b) lll_lock(a,b), 0
#endif

#define aconf __elision_aconf

/* Adaptive lock using transactions.
   By default the lock region is run as a transaction, and when it
   aborts or the lock is busy the lock adapts itself.  */

int
__lll_lock_elision (int *lock, short *adapt_count, EXTRAARG int pshared)
{
  if (*adapt_count > 0)
    {
      (*adapt_count)--;
      goto use_lock;
    }

  int try_begin = aconf.try_tbegin;
  while (1)
    {
      if (__builtin_tbegin (0))
	{
	  /* Lock was busy.  Fall back to normal locking.  */
	  if (*lock == 0)
	     __builtin_tabort (0);
	  return 0;
	}
      else
	{
	  /* A persistent failure indicates that a retry will probably
	     result in another failure.  Use normal locking now and
	     for the next couple of calls.  */
	  if (try_begin-- <= 0
	      || _TEXASRU_FAILURE_PERSISTENT (__builtin_get_texasru ()))
	   {
	      if (aconf.skip_lock_internal_abort > 0) 
		*adapt_count = aconf.skip_lock_internal_abort;
	      goto use_lock;
	   }
	}
     }

  /* Same logic as above, but for for a number of temporary failures in a
     a row.  */
  if (aconf.skip_lock_out_of_tbegin_retries > 0 && aconf.try_tbegin > 0)
    *adapt_count = aconf.skip_lock_out_of_tbegin_retries;

use_lock:
  return LLL_LOCK ((*lock), pshared);
}
