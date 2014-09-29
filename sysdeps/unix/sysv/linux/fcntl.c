/* Copyright (C) 2000-2014 Free Software Foundation, Inc.
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
#include <errno.h>
#include <sysdep-cancel.h>	/* Must come before <fcntl.h>.  */
#include <fcntl.h>
#include <stdarg.h>

#include <sys/syscall.h>

static inline int
__fcntl_common_nocancel (int fd, int cmd, void *arg)
{
  if (cmd != F_GETOWN)
    return INLINE_SYSCALL (fcntl, 3, fd, cmd, arg);

  INTERNAL_SYSCALL_DECL (err);
  struct f_owner_ex fex;
  int res = INTERNAL_SYSCALL (fcntl, err, 3, fd, F_GETOWN_EX, &fex);
  if (!INTERNAL_SYSCALL_ERROR_P (res, err))
    return fex.type == F_OWNER_GID ? -fex.pid : fex.pid;

  __set_errno (INTERNAL_SYSCALL_ERRNO (res, err));
  return -1;
}

static inline int
__fcntl_common_cancel (int fd, int cmd, void *arg)
{
  if (cmd != F_GETOWN)
    return SYSCALL_CANCEL (fcntl, fd, cmd, arg);

  struct f_owner_ex fex;
  int res = SYSCALL_CANCEL_NCS (fcntl, fd, F_GETOWN_EX, &fex);
  if (!SYSCALL_CANCEL_ERROR (res))
    return fex.type == F_OWNER_GID ? -fex.pid : fex.pid;

  __set_errno (SYSCALL_CANCEL_ERRNO (res));
  return -1;
}

#ifndef IS_IN_rtld
int
__fcntl_nocancel (int fd, int cmd, ...)
{
  va_list ap;
  void *arg;

  va_start (ap, cmd);
  arg = va_arg (ap, void *);
  va_end (ap);

  return __fcntl_common_nocancel (fd, cmd, arg);
}
#endif

int
__libc_fcntl (int fd, int cmd, ...)
{
  va_list ap;
  void *arg;

  va_start (ap, cmd);
  arg = va_arg (ap, void *);
  va_end (ap);

  if (cmd != F_SETLKW)
    return __fcntl_common_nocancel (fd, cmd, arg);

  return __fcntl_common_cancel (fd, cmd, arg);
}
libc_hidden_def (__libc_fcntl)

weak_alias (__libc_fcntl, __fcntl)
libc_hidden_weak (__fcntl)
weak_alias (__libc_fcntl, fcntl)
