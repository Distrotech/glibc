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

#include <sysdeps/generic/sysdep.h>

#include <sys/syscall.h>
#define	HAVE_SYSCALLS

#ifndef __ASSEMBLER__
# include <errno.h>

/* Note that using a `PASTE' macro loses.  */
#define	SYSCALL__(name, args)	PSEUDO (__##name, name, args)
#define	SYSCALL(name, args)	PSEUDO (name, name, args)

/* Cancellation macros.  */
#ifndef __SSC
typedef long __syscall_arg_t;
# define __SSC(__x) ((__syscall_arg_t) (__x))
#endif

#define __SYSCALL_CANCEL0(__n) \
  (__syscall_cancel)(__n, 0, 0, 0, 0, 0, 0)
#define __SYSCALL_CANCEL1(__n, __a) \
  (__syscall_cancel)(__n, __SSC(__a), 0, 0, 0, 0, 0)
#define __SYSCALL_CANCEL2(__n, __a, __b) \
  (__syscall_cancel)(__n, __SSC(__a), __SSC(__b), 0, 0, 0, 0)
#define __SYSCALL_CANCEL3(__n, __a, __b, __c) \
  (__syscall_cancel)(__n, __SSC(__a), __SSC(__b), __SSC(__c), 0, 0, 0)
#define __SYSCALL_CANCEL4(__n, __a, __b, __c, __d) \
  (__syscall_cancel)(__n, __SSC(__a), __SSC(__b), __SSC(__c), __SSC(__d), \
		     0, 0)
#define __SYSCALL_CANCEL5(__n, __a, __b, __c, __d, __e) \
  (__syscall_cancel)(__n, __SSC(__a), __SSC(__b), __SSC(__c), __SSC(__d), \
		     __SSC(__e), 0)
#define __SYSCALL_CANCEL6(__n, __a, __b, __c, __d, __e, __f) \
  (__syscall_cancel)(__n, __SSC(__a), __SSC(__b), __SSC(__c), __SSC(__d), \
		     __SSC(__e), __SSC(__f))

#define __SYSCALL_NARGS_X(a,b,c,d,e,f,g,h,n,...) n
#define __SYSCALL_NARGS(...) \
  __SYSCALL_NARGS_X (__VA_ARGS__, 7, 6, 5, 4, 3, 2, 1, 0,)
#define __SYSCALL_CONCAT_X(__a,__b)	__a##__b
#define __SYSCALL_CONCAT(__a,__b)	__SYSCALL_CONCAT_X (__a, __b)
#define __SYSCALL_DISP(__b,...) \
  __SYSCALL_CONCAT (__b,__SYSCALL_NARGS(__VA_ARGS__))(__VA_ARGS__)

#define __SYSCALL_CANCEL(...) __SYSCALL_DISP (__SYSCALL_CANCEL, __VA_ARGS__)

#define SYSCALL_CANCEL_NCS(name, nr, args...)				\
  __SYSCALL_CANCEL (__NR_##name, nr, args)

#define SYSCALL_CANCEL(name, args...)					\
  ({									\
    long int sc_ret = SYSCALL_CANCEL_NCS (name, args);			\
    if (SYSCALL_CANCEL_ERROR (sc_ret))					\
      {									\
        __set_errno (SYSCALL_CANCEL_ERRNO (sc_ret));			\
        sc_ret = -1L;							\
      }									\
    sc_ret;								\
  })

long int __syscall_cancel (__syscall_arg_t nr, __syscall_arg_t arg1,
			   __syscall_arg_t arg2, __syscall_arg_t arg3,
			   __syscall_arg_t arg4, __syscall_arg_t arg5,
			   __syscall_arg_t arg6);
libc_hidden_proto (__syscall_cancel);

#endif

/* Machine-dependent sysdep.h files are expected to define the macro
   PSEUDO (function_name, syscall_name) to emit assembly code to define the
   C-callable function FUNCTION_NAME to do system call SYSCALL_NAME.
   r0 and r1 are the system call outputs.  MOVE(x, y) should be defined as
   an instruction such that "MOVE(r1, r0)" works.  ret should be defined
   as the return instruction.  */

#define SYS_ify(syscall_name) SYS_##syscall_name

/* Terminate a system call named SYM.  This is used on some platforms
   to generate correct debugging information.  */
#ifndef PSEUDO_END
#define PSEUDO_END(sym)
#endif
#ifndef PSEUDO_END_NOERRNO
#define PSEUDO_END_NOERRNO(sym)	PSEUDO_END(sym)
#endif
#ifndef PSEUDO_END_ERRVAL
#define PSEUDO_END_ERRVAL(sym)	PSEUDO_END(sym)
#endif

/* Wrappers around system calls should normally inline the system call code.
   But sometimes it is not possible or implemented and we use this code.  */
#define INLINE_SYSCALL(name, nr, args...) __syscall_##name (args)
