/* Test and measure STRCMP functions.
   Copyright (C) 1999, 2002, 2003, 2005, 2011 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Written by Jakub Jelinek <jakub@redhat.com>, 1999.
   Added wcscmp support by Liubov Dmitrieva <liubov.dmitrieva@gmail.com>, 2011.

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

#define TEST_MAIN
#include "test-string.h"

#ifdef WIDE
# include <inttypes.h>
# include <wchar.h>

# define L(str) L##str
# define STRCMP wcscmp
# define STRCPY wcscpy
# define STRLEN wcslen
# define MEMCPY wmemcpy
# define SIMPLE_STRCMP simple_wcscmp
# define STUPID_STRCMP stupid_wcscmp
# define CHAR wchar_t
# define UCHAR uint32_t
# define CHARBYTES 4
# define CHARBYTESLOG 2
# define CHARALIGN __alignof__ (CHAR)
# define MIDCHAR 0x7fffffff
# define LARGECHAR 0xfffffffe
#else
# define L(str) str
# define STRCMP strcmp
# define STRCPY strcpy
# define STRLEN strlen
# define MEMCPY memcpy
# define SIMPLE_STRCMP simple_strcmp
# define STUPID_STRCMP stupid_strcmp
# define CHAR char
# define UCHAR unsigned char
# define CHARBYTES 1
# define CHARBYTESLOG 0
# define CHARALIGN 1
# define MIDCHAR 0x7f
# define LARGECHAR 0xfe
#endif
typedef int (*proto_t) (const CHAR *, const CHAR *);

int
SIMPLE_STRCMP (const CHAR *s1, const CHAR *s2)
{
  int ret;

  while ((ret = *(UCHAR *) s1 - *(UCHAR *) s2++) == 0 && *s1++);
  return ret;
}

int
STUPID_STRCMP (const CHAR *s1, const CHAR *s2)
{
  size_t ns1 = STRLEN (s1) + 1;
  size_t ns2 = STRLEN (s2) + 1;
  size_t n = ns1 < ns2 ? ns1 : ns2;
  int ret = 0;

  while (n--)
    if ((ret = *(UCHAR *) s1++ - *(UCHAR *) s2++) != 0)
      break;
  return ret;
}

IMPL (STUPID_STRCMP, 1)
IMPL (SIMPLE_STRCMP, 1)
IMPL (STRCMP, 1)

static int
check_result (impl_t *impl,
	     const CHAR *s1, const CHAR *s2,
	     int exp_result)
{
  int result = CALL (impl, s1, s2);
  if ((exp_result == 0 && result != 0)
      || (exp_result < 0 && result >= 0)
      || (exp_result > 0 && result <= 0))
    {
      error (0, 0, "Wrong result in function %s %d %d", impl->name,
	     result, exp_result);
      ret = 1;
      return -1;
    }

  return 0;
}

static void
do_one_test (impl_t *impl,
	     const CHAR *s1, const CHAR *s2,
	     int exp_result)
{
  if (check_result (impl, s1, s2, exp_result) < 0)
    return;

  if (HP_TIMING_AVAIL)
    {
      hp_timing_t start __attribute ((unused));
      hp_timing_t stop __attribute ((unused));
      hp_timing_t best_time = ~ (hp_timing_t) 0;
      size_t i;

      for (i = 0; i < 32; ++i)
	{
	  HP_TIMING_NOW (start);
	  CALL (impl, s1, s2);
	  HP_TIMING_NOW (stop);
	  HP_TIMING_BEST (best_time, start, stop);
	}

      printf ("\t%zd", (size_t) best_time);
    }
}

static void
do_test (size_t align1, size_t align2, size_t len, int max_char,
	 int exp_result)
{
  size_t i;

  CHAR *s1, *s2;

  if (len == 0)
    return;

  align1 &= 63;
  if (align1 + (len + 1) * CHARBYTES >= page_size)
    return;

  align2 &= 63;
  if (align2 + (len + 1) * CHARBYTES >= page_size)
    return;

  /* Put them close to the end of page.  */
  i = align1 + CHARBYTES * (len + 2);
  s1 = (CHAR *) (buf1 + ((page_size - i) / 16 * 16) + align1);
  i = align2 + CHARBYTES * (len + 2);
  s2 = (CHAR *) (buf2 + ((page_size - i) / 16 * 16)  + align2);

  for (i = 0; i < len; i++)
    s1[i] = s2[i] = 1 + (23 << ((CHARBYTES - 1) * 8)) * i % max_char;

  s1[len] = s2[len] = 0;
  s1[len + 1] = 23;
  s2[len + 1] = 24 + exp_result;
  s2[len - 1] -= exp_result;

  if (HP_TIMING_AVAIL)
    printf ("Length %4zd, alignment %2zd/%2zd:", len, align1, align2);

  FOR_EACH_IMPL (impl, 0)
    do_one_test (impl, s1, s2, exp_result);

  if (HP_TIMING_AVAIL)
    putchar ('\n');
}

static void
do_random_tests (void)
{
  for (size_t a = 0; a < CHARBYTES; a += CHARALIGN)
    for (size_t b = 0; b < CHARBYTES; b += CHARALIGN)
      {
	UCHAR *p1 = (UCHAR *) (buf1 + page_size - 512 * CHARBYTES - a);
	UCHAR *p2 = (UCHAR *) (buf2 + page_size - 512 * CHARBYTES - b);

	for (size_t n = 0; n < ITERATIONS; n++)
	  {
	    size_t align1 = random () & 31;
	    size_t align2;
	    if (random () & 1)
	      align2 = random () & 31;
	    else
	      align2 = align1 + (random () & 24);
	    size_t pos = random () & 511;
	    size_t j = align1 > align2 ? align1 : align2;
	    if (pos + j >= 511)
	      pos = 510 - j - (random () & 7);
	    size_t len1 = random () & 511;
	    if (pos >= len1 && (random () & 1))
	      len1 = pos + (random () & 7);
	    if (len1 + j >= 512)
	      len1 = 511 - j - (random () & 7);
	    size_t len2;
	    if (pos >= len1)
	      len2 = len1;
	    else
	      len2 = len1 + (len1 != 511 - j ? random () % (511 - j - len1) : 0);
	    j = (pos > len2 ? pos : len2) + align1 + 64;
	    if (j > 512)
	      j = 512;
	    for (size_t i = 0; i < j; ++i)
	      {
		p1[i] = random () & 255;
		if (i < len1 + align1 && !p1[i])
		  {
		    p1[i] = random () & 255;
		    if (!p1[i])
		      p1[i] = 1 + (random () & 127);
		  }
	      }
	    for (size_t i = 0; i < j; ++i)
	      {
		p2[i] = random () & 255;
		if (i < len2 + align2 && !p2[i])
		  {
		    p2[i] = random () & 255;
		    if (!p2[i])
		      p2[i] = 1 + (random () & 127);
		  }
	      }

	    int result = 0;
	    MEMCPY ((CHAR *) (p2 + align2), (CHAR *) (p1 + align1), pos);
	    if (pos < len1)
	      {
		if (p2[align2 + pos] == p1[align1 + pos])
		  {
		    p2[align2 + pos] = random () & 255;
		    if (p2[align2 + pos] == p1[align1 + pos])
		      p2[align2 + pos] = p1[align1 + pos] + 3 + (random () & 127);
		  }

		if (p1[align1 + pos] < p2[align2 + pos])
		  result = -1;
		else
		  result = 1;
	      }
	    p1[len1 + align1] = 0;
	    p2[len2 + align2] = 0;

	    FOR_EACH_IMPL (impl, 1)
	      {
		int r = CALL (impl, (CHAR *) (p1 + align1), (CHAR *) (p2 + align2));
		/* Test whether on 64-bit architectures where ABI requires
		   callee to promote has the promotion been done.  */
		asm ("" : "=g" (r) : "0" (r));
		if ((r == 0 && result)
		    || (r < 0 && result >= 0)
		    || (r > 0 && result <= 0))
		  {
		    error (0, 0, "Iteration %zd - wrong result in function %s (%zd, %zd, %zd, %zd, %zd) %d != %d, p1 %p p2 %p",
			   n, impl->name, (size_t) (p1 + align1) & 63, (size_t) (p1 + align2) & 63, len1, len2, pos, r, result, p1, p2);
		    ret = 1;
		  }
	      }
	  }
      }
}

static void
check (void)
{
  CHAR *s1 = (CHAR *) (buf1 + 0xb2c);
  CHAR *s2 = (CHAR *) (buf1 + 0xfd8);

  STRCPY(s1, L("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrs"));
  STRCPY(s2, L("abcdefghijklmnopqrstuvwxyzabcdefghijklmnopqrstuvwxyzabcdefghijkLMNOPQRSTUV"));

  size_t l1 = STRLEN (s1);
  size_t l2 = STRLEN (s2);
  for (size_t i1 = 0; i1 < l1; i1++)
    for (size_t i2 = 0; i2 < l2; i2++)
      {
	int exp_result = SIMPLE_STRCMP (s1 + i1, s2 + i2);
	FOR_EACH_IMPL (impl, 0)
	  check_result (impl, s1 + i1, s2 + i2, exp_result);
      }
}


int
test_main (void)
{
  size_t i;

  test_init ();
  check();

  printf ("%23s", "");
  FOR_EACH_IMPL (impl, 0)
    printf ("\t%s", impl->name);
  putchar ('\n');

  for (i = 1; i < 32; ++i)
    {
      do_test (CHARBYTES * i, CHARBYTES * i, i, MIDCHAR, 0);
      do_test (CHARBYTES * i, CHARBYTES * i, i, MIDCHAR, 1);
      do_test (CHARBYTES * i, CHARBYTES * i, i, MIDCHAR, -1);
    }

  for (i = 1; i < 10 + CHARBYTESLOG; ++i)
    {
      do_test (0, 0, 2 << i, MIDCHAR, 0);
      do_test (0, 0, 2 << i, LARGECHAR, 0);
      do_test (0, 0, 2 << i, MIDCHAR, 1);
      do_test (0, 0, 2 << i, LARGECHAR, 1);
      do_test (0, 0, 2 << i, MIDCHAR, -1);
      do_test (0, 0, 2 << i, LARGECHAR, -1);
      do_test (0, CHARBYTES * i, 2 << i, MIDCHAR, 1);
      do_test (CHARBYTES * i, CHARBYTES * (i + 1), 2 << i, LARGECHAR, 1);
    }

  for (i = 1; i < 8; ++i)
    {
      do_test (CHARBYTES * i, 2 * CHARBYTES * i, 8 << i, MIDCHAR, 0);
      do_test (2 * CHARBYTES * i, CHARBYTES * i, 8 << i, LARGECHAR, 0);
      do_test (CHARBYTES * i, 2 * CHARBYTES * i, 8 << i, MIDCHAR, 1);
      do_test (2 * CHARBYTES * i, CHARBYTES * i, 8 << i, LARGECHAR, 1);
      do_test (CHARBYTES * i, 2 * CHARBYTES * i, 8 << i, MIDCHAR, -1);
      do_test (2 * CHARBYTES * i, CHARBYTES * i, 8 << i, LARGECHAR, -1);
    }

  do_random_tests ();
  return ret;
}

#include "../test-skeleton.c"
