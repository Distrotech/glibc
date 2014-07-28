/* fmemopen implementation.
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

/* fmemopen() from 2.20 and forward works as defined by POSIX.  It also
   provides an older symbol, version 2.2.5, that behaves different regarding
   SEEK_END (libio/oldfmemopen.c).  */


#include <errno.h>
#include <libio.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include "libioP.h"


typedef struct fmemopen_cookie_struct fmemopen_cookie_t;
struct fmemopen_cookie_struct
{
  char        *buffer;   /* memory buffer.  */
  int         mybuffer;  /* own allocated buffer?  */
  int         append;    /* buffer openened for append?  */
  size_t      size;      /* buffer length in bytes.  */
  _IO_off64_t pos;       /* current position at the buffer.  */
  size_t      maxpos;    /* max position in buffer.  */
};


static ssize_t
fmemopen_read (void *cookie, char *b, size_t s)
{
  fmemopen_cookie_t *c = (fmemopen_cookie_t *) cookie;

  if (c->pos + s > c->maxpos)
    {
      if ((size_t) c->pos == c->maxpos)
	return 0;
      s = c->size - c->pos;
    }

  memcpy (b, &(c->buffer[c->pos]), s);

  c->pos += s;
  if ((size_t) c->pos > c->maxpos)
    c->maxpos = c->pos;

  return s;
}


static ssize_t
fmemopen_write (void *cookie, const char *b, size_t s)
{
  fmemopen_cookie_t *c = (fmemopen_cookie_t *) cookie;;
  _IO_off64_t pos = c->append ? c->maxpos : c->pos;
  int addnullc;

  addnullc = (s == 0 || b[s - 1] != '\0');

  if (pos + s + addnullc > c->size)
    {
      if ((size_t) (pos + addnullc) >= c->size)
	{
	  __set_errno (ENOSPC);
	  return 0;
	}
      s = c->size - pos - addnullc;
    }

  memcpy (&(c->buffer[pos]), b, s);

  pos += s;
  if ((size_t) pos > c->maxpos)
    {
      c->maxpos = pos;
      if (addnullc)
	c->buffer[c->maxpos] = '\0';
    }

  return s;
}


static int
fmemopen_seek (void *cookie, _IO_off64_t *p, int w)
{
  _IO_off64_t np;
  fmemopen_cookie_t *c = (fmemopen_cookie_t *) cookie;

  switch (w)
    {
    case SEEK_SET:
      np = *p;
      break;

    case SEEK_CUR:
      np = c->pos + *p;
      break;

    case SEEK_END:
      np = c->maxpos + *p;
      break;

    default:
      return -1;
    }

  if (np < 0 || (size_t) np > c->size)
    return -1;

  *p = c->pos = np;

  return 0;
}


static int
fmemopen_close (void *cookie)
{
  fmemopen_cookie_t *c = (fmemopen_cookie_t *) cookie;

  if (c->mybuffer)
    free (c->buffer);
  free (c);

  return 0;
}


FILE *
__fmemopen (void *buf, size_t len, const char *mode)
{
  cookie_io_functions_t iof;
  fmemopen_cookie_t *c;

  c = (fmemopen_cookie_t *) malloc (sizeof (fmemopen_cookie_t));
  if (c == NULL)
    return NULL;

  c->mybuffer = (buf == NULL);

  if (c->mybuffer)
    {
      c->buffer = (char *) malloc (len);
      if (c->buffer == NULL)
	{
	  free (c);
	  return NULL;
	}
      c->buffer[0] = '\0';
      c->maxpos = 0;
    }
  else
    {
      if (__glibc_unlikely ((uintptr_t) len > -(uintptr_t) buf))
	{
	  free (c);
	  __set_errno (EINVAL);
	  return NULL;
	}

      c->buffer = buf;

      /* POSIX states that w+ mode should truncate the buffer.  */
      if (mode[0] == 'w' && mode[1] == '+')
	c->buffer[0] = '\0';

      c->maxpos = strnlen (c->buffer, len);
    }


  /* Mode   |  starting position (cookie::pos) |          size (cookie::size)
   * ------ |----------------------------------|-----------------------------
   * read   |          beginning of the buffer |                size argument
   * write  |          beginning of the buffer |                         zero
   * append |    first null or size buffer + 1 |  first null or size argument
   */

  c->size = len;

  if (mode[0] == 'r')
    c->maxpos = len;

  c->append = mode[0] == 'a';
  if (c->append)
    c->pos = c->maxpos;
  else
    c->pos = 0;

  iof.read = fmemopen_read;
  iof.write = fmemopen_write;
  iof.seek = fmemopen_seek;
  iof.close = fmemopen_close;

  return _IO_fopencookie (c, mode, iof);
}
libc_hidden_def (__fmemopen)
versioned_symbol (libc, __fmemopen, fmemopen, GLIBC_2_20);
