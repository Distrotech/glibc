/* fmemopen tests.
   Copyright (C) 2014 Free Software Foundation, Inc.
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


#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>

static int
do_test_with_buffer (void)
{
  int result = 0;
  char buf[100];
  const size_t nbuf = sizeof (buf);

  FILE *fp = fmemopen (buf, nbuf, "w");
  if (fp == NULL)
    {
      printf ("%s: fmemopen failed\n", __FUNCTION__);
      return 1;
    }

  static const char str[] = "hello world";
  const size_t nstr = sizeof (str) - 1;
  fputs (str, fp);
  off_t o = ftello (fp);
  if (o != nstr)
    {
      printf ("%s: first ftello returned %ld, expected %zu\n",
	      __FUNCTION__, o, nstr);
      result = 1;
    }

  rewind (fp);
  o = ftello (fp);
  if (o != 0)
    {
      printf ("%s: second ftello returned %ld, expected 0\n",
	      __FUNCTION__, o);
      result = 1;
    }
  if (fseeko (fp, 0, SEEK_END) != 0)
    {
      printf ("%s: fseeko failed\n", __FUNCTION__);
      result = 1;
    }
  o = ftello (fp);
  if (o != nstr)
    {
      printf ("%s: third ftello returned %ld, expected %zu\n",
	      __FUNCTION__, o, nbuf);
      result = 1;
    }

  rewind (fp);
  static const char str2[] = "just hello";
  const size_t nstr2 = sizeof (str2) - 1;
  assert (nstr2 < nstr);
  fputs (str2, fp);
  o = ftello (fp);
  if (o != nstr2)
    {
      printf ("%s: fourth ftello returned %ld, expected %zu\n",
	      __FUNCTION__, o, nstr2);
      result = 1;
    }
  fclose (fp);

  static const char str3[] = "just hellod";
  if (strcmp (buf, str3) != 0)
    {
      printf ("%s: final string is \"%s\", expected \"%s\"\n",
              __FUNCTION__, buf, str3);
      result = 1;
    }
  return result;
}

static int
do_test_without_buffer (void)
{
  int result = 0;
  const size_t nbuf = 100;

  FILE *fp = fmemopen (NULL, nbuf, "w");
  if (fp == NULL)
    {
      printf ("%s: fmemopen failed\n", __FUNCTION__);
      return 1;
    }

  static const char str[] = "hello world";
  const size_t nstr = sizeof (str) - 1;

  fputs (str, fp);
  off_t o = ftello (fp);
  if (o != nstr)
    {
      printf ("%s: first ftello returned %ld, expected %zu\n",
	      __FUNCTION__, o, nstr);
      result = 1;
    }
  if (fseeko (fp, 0, SEEK_END) != 0)
    {
      printf ("%s: fseeko failed\n", __FUNCTION__);
      result = 1;
    }
  o = ftello (fp);
  if (o != nstr)
    {
      printf ("%s: second ftello returned %ld, expected %zu\n",
	      __FUNCTION__, o, nbuf);
      result = 1;
    }
  rewind (fp);
  static const char str2[] = "just hello";
  const size_t nstr2 = sizeof (str2) - 1;
  assert (nstr2 < nstr);
  fputs (str2, fp);
  o = ftello (fp);
  if (o != nstr2)
    {
      printf ("%s: third ftello returned %ld, expected %zu\n",
	      __FUNCTION__, o, nstr2);
      result = 1;
    }
  fclose (fp);

  return result;
}

static int
do_test_length_zero (void)
{
  int result = 0;
  FILE *fp;
#define BUFCONTENTS "testing buffer"
  char buf[100] = BUFCONTENTS;
  const size_t nbuf = 0;
  int r;

  fp = fmemopen (buf, nbuf, "r");
  if (fp == NULL)
    {
      printf ("%s: fmemopen failed\n", __FUNCTION__);
      return 1;
    }

  /* Reading any data on a zero-length buffer should return EOF.  */
  if ((r = fgetc (fp)) != EOF)
    {
      printf ("%s: fgetc on a zero-length returned: %d\n",
	      __FUNCTION__, r);
      result = 1;
    }
  off_t o = ftello (fp);
  if (o != 0)
    {
      printf ("%s: first ftello returned %ld, expected 0\n",
	      __FUNCTION__, o);
      result = 1;
    }
  fclose (fp);

  /* Writing any data shall start at current position and shall not pass
     current buffer size beyond the size in fmemopen call.  */
  fp = fmemopen (buf, nbuf, "w");
  if (fp == NULL)
    {
      printf ("%s: second fmemopen failed\n", __FUNCTION__);
      return 1;
    }

  static const char str[] = "hello world";
  /* Because of buffering, fputs call itself don't fail, however the final
     buffer should be not changed because of length 0 passed in fmemopen
     call.  */
  fputs (str, fp);
  r = 0;
  errno = 0;
  if (fflush (fp) != EOF)
    {
      printf ("%s: fflush did not return EOF\n", __FUNCTION__);
      fclose (fp);
      return 1;
    }
  if (errno != ENOSPC)
    {
      printf ("%s: errno is %i (expected %i - ENOSPC)\n", __FUNCTION__,
	      errno, ENOSPC);
      fclose (fp);
      return 1;
    }

  fclose (fp);

  if (strcmp (buf, BUFCONTENTS) != 0)
    {
      printf ("%s: strcmp (%s, %s) failed\n", __FUNCTION__, buf,
	      BUFCONTENTS);
      return 1;
    }

  /* Different than 'w' mode, 'w+' truncates the buffer.  */
  fp = fmemopen (buf, nbuf, "w+");
  if (fp == NULL)
    {
      printf ("%s: second fmemopen failed\n", __FUNCTION__);
      return 1;
    }

  fclose (fp);

  if (strcmp (buf, "") != 0)
    {
      printf ("%s: strcmp (%s, \"\") failed\n", __FUNCTION__, buf);
      return 1;
    }

  return result;
}

static int
do_test (void)
{
  int ret = 0;

  ret += do_test_with_buffer ();
  ret += do_test_without_buffer ();
  ret += do_test_length_zero ();

  return ret;
}

#define TEST_FUNCTION do_test ()
#include "../test-skeleton.c"
