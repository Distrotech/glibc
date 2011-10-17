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


/* __ieee754_atanh(x)
   Method :
      1.Reduced x to positive by atanh(-x) = -atanh(x)
      2.For x>=0.5
		    1              2x                          x
	atanh(x) = --- * log(1 + -------) = 0.5 * log1p(2 * --------)
		    2             1 - x                      1 - x

	For x<0.5
	atanh(x) = 0.5*log1p(2x+2x*x/(1-x))

   Special cases:
	atanh(x) is NaN if |x| > 1 with signal;
	atanh(NaN) is that NaN with no signal;
	atanh(+-1) is +-INF with signal.

 */

#include <inttypes.h>
#include "math.h"
#include "math_private.h"

static const float huge = 1e30;

float
__ieee754_atanhf (float x)
{
  float xa = fabsf (x);
  float t;
  if (xa < 0.5f)
    {
      if (__builtin_expect (xa < 0x1.0p-28f, 0) && (huge + x) > 0.0f)
	return x;

      t = xa + xa;
      t = 0.5f * __log1pf (t + t * xa / (1.0f - xa));
    }
  else if (__builtin_expect (xa < 1.0f, 1))
    t = 0.5f * __log1pf ((xa + xa) / (1.0f - xa));
  else
    {
      if (xa > 1.0f)
	return (x - x) / (x - x);

      return x / 0.0f;
    }

  return __copysignf (t, x);
}
strong_alias (__ieee754_atanhf, __atanhf_finite)
