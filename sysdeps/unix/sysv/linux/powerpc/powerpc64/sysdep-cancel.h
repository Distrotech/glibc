/* Cancellable system call stubs.  Linux/PowerPC64 version.
   Copyright (C) 2003-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.
   Contributed by Franz Sirl <Franz.Sirl-kernel@lauterbach.com>, 2003.

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
#include <tls.h>
#ifndef __ASSEMBLER__
# include <nptl/pthreadP.h>
#endif

#if !defined NOT_IN_libc || defined IS_IN_libpthread || defined IS_IN_librt

# ifdef HAVE_ASM_GLOBAL_DOT_NAME
#  define DASHDASHPFX(str) .__##str
# else
#  define DASHDASHPFX(str) __##str
# endif

# ifdef NOT_IN_libc
#  undef HIDDEN_JUMPTARGET
#  define HIDDEN_JUMPTARGET(__symbol) __symbol
# endif

#define CANCEL_FRAMESIZE (FRAME_MIN_SIZE)

# undef PSEUDO
# define PSEUDO(name, syscall_name, args)				\
  .section ".text";							\
  ENTRY (name)								\
    mflr r0;								\
    std  r0,FRAME_LR_SAVE(r1);						\
    cfi_offset (lr, FRAME_LR_SAVE);					\
    stdu r1,-CANCEL_FRAMESIZE(r1);					\
    cfi_adjust_cfa_offset (CANCEL_FRAMESIZE);				\
    mr   r9,r8;								\
    mr   r8,r7;								\
    mr   r7,r6;								\
    mr   r6,r5;								\
    mr   r5,r4;								\
    mr   r4,r3;								\
    li   r3,SYS_ify (syscall_name);					\
    bl   HIDDEN_JUMPTARGET(__syscall_cancel);				\
    nop;								\

# undef PSEUDO_RET
# define PSEUDO_RET							\
    bl   JUMPTARGET(__syscall_cancel_error);				\
    nop;								\
    addi r1,r1,CANCEL_FRAMESIZE;					\
    cfi_adjust_cfa_offset (-CANCEL_FRAMESIZE);				\
    ld   r0,FRAME_LR_SAVE(r1);						\
    mtlr r0;								\
    cfi_restore (lr);							\
    blr

#endif

#ifndef __ASSEMBLER__
# define RTLD_SINGLE_THREAD_P \
  __builtin_expect (THREAD_GETMEM (THREAD_SELF, \
				   header.multiple_threads) == 0, 1)
#endif
