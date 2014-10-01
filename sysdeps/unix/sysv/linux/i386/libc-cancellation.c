/* Copyright (C) 2014 Free Software Foundation, Inc.
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

#include <sysdep.h>

#undef INTERNAL_SYSCALL_NCS
#define INTERNAL_SYSCALL_NCS(__name, __err, __nr, __arg1, __arg2, __arg3,    \
			     __arg4, __arg5, __arg6)			     \
  __syscall6_i386 (__name, __arg1, __arg2, __arg3, __arg4, __arg5, __arg6)
 
static inline long
__syscall6_i386 (long __name, long __a1, long __a2, long __a3, long __a4,
		 long __a5, long __a6)
{
  unsigned long __ret;
  long __args[] = { __a5, __a6 };
  asm volatile ("push %6\n"
		"call __vsyscall6_i386\n"
		"add  $4,%%esp"
		 : "=a" (__ret)
		 : "a" (__name), "d"(__a1), "c"(__a2), "D"(__a3), "S"(__a4),
		   "g" (0+__args)
		 : "memory");
  return __ret;
}

#include <nptl/libc-cancellation.c>
