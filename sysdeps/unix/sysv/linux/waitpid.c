/* Copyright (C) 1991-2014 Free Software Foundation, Inc.
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
#include <sysdep-cancel.h>
#include <stdlib.h>
#include <sys/wait.h>

__pid_t
__libc_waitpid (__pid_t pid, int *stat_loc, int options)
{
#ifdef __NR_waitpid
  return SYSCALL_CANCEL (waitpid, pid, stat_loc, options);
#else
  return SYSCALL_CANCEL (wait4, pid, stat_loc, options, NULL);
#endif
}
weak_alias (__libc_waitpid, __waitpid)
libc_hidden_weak (__waitpid)
weak_alias (__libc_waitpid, waitpid)
