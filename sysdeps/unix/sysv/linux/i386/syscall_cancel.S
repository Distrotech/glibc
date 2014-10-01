/* Cancellable syscall wrapper - i386 version.
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

#include <sysdep.h>

ENTRY (__syscall_cancel_arch)

	mov   4(%esp),%ecx
	pushl %ebx
	pushl %esi
	pushl %edi
	pushl %ebp

	.globl __syscall_cancel_arch_start
	.type  __syscall_cancel_arch_start,@function
__syscall_cancel_arch_start:
	movl  (%ecx),%eax
	testb $4, %al
	jne   __syscall_do_cancel

	movl  24(%esp),%eax
	movl  28(%esp),%ebx
	movl  32(%esp),%ecx
	movl  36(%esp),%edx
	movl  40(%esp),%esi
	movl  44(%esp),%edi
	movl  48(%esp),%ebp
	int   $128

	.globl __syscall_cancel_arch_end
	.type  __syscall_cancel_arch_end,@function
__syscall_cancel_arch_end:
	popl  %ebp
	popl  %edi
	popl  %esi
	popl  %ebx
	ret

END (__syscall_cancel_arch)
libc_hidden_def (__syscall_cancel_arch)
