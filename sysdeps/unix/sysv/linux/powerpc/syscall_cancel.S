/* Cancellable syscall wrapper - powerpc version.
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

	.globl __syscall_cancel_arch_start
	.type  __syscall_cancel_arch_start,@function
__syscall_cancel_arch_start:

	/*mflr	r10
	std	r10,FRAME_LR_SAVE(r1)*/

	lwz     r0,0(r3)
	rldicl. r0,r0,62,63
	beq     1f
	b       __syscall_do_cancel
	nop
1:
	mr      r0,r4
	mr      r3,r5
	mr      r4,r6
	mr      r5,r7
	mr      r6,r8
	mr      r7,r9
	mr      r8,r10
	sc

	.globl __syscall_cancel_arch_end
	.type  __syscall_cancel_arch_end,@function
__syscall_cancel_arch_end:

	/*ld	r10,FRAME_LR_SAVE(r1)
	mtlr	r10*/

	bnslr+
	neg	r3,r3
	blr
END (__syscall_cancel_arch)
libc_hidden_def (__syscall_cancel_arch)
