/* readv supports all Linux kernels >= 2.0.
   Copyright (C) 1997-2014 Free Software Foundation, Inc.
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
#include <stddef.h>
#include <sys/param.h>
#include <sys/uio.h>

#include <sysdep-cancel.h>
#include <sys/syscall.h>

/* Consider moving to syscalls.list.  */

ssize_t
__libc_readv (int fd, const struct iovec *vector, int count)
{
  return SYSCALL_CANCEL (readv, fd, vector, count);
}
strong_alias (__libc_readv, __readv)
weak_alias (__libc_readv, readv)
