/* Copyright (C) 2011 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Ulrich Drepper <drepper@gmail.com>, 2011.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with the GNU C Library; if not, write to the Free
   Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307 USA.  */

#include <fenv.h>
#include <math.h>
#include <math_private.h>


/* wrapper j1f */
float
j1f (float x)
{
  if (__builtin_expect (fabsf (x) > X_TLOSS, 0) && _LIB_VERSION != _IEEE_)
    /* j1(|x|>X_TLOSS) */
    return __kernel_standard_f (x, x, 136);

  return __ieee754_j1f (x);
}


/* wrapper y1f */
float
y1f (float x)
{
  if (__builtin_expect (x <= 0.0f || x > (float) X_TLOSS, 0)
      && _LIB_VERSION != _IEEE_)
    {
      if (x < 0.0f)
	{
	  /* d = zero/(x-x) */
	  feraiseexcept (FE_INVALID);
	  return __kernel_standard_f (x, x, 111);
	}
      else if (x == 0.0f)
	/* d = -one/(x-x) */
	return __kernel_standard_f (x, x, 110);
      else
	/* y1(x>X_TLOSS) */
	return __kernel_standard_f (x, x, 137);
    }

  return __ieee754_y1f (x);
}
