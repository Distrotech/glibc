/* Copyright (C) 1995-2014 Free Software Foundation, Inc.
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

#include <sysdep-cancel.h>
#include <socketcall.h>

#define P(a, b) P2(a, b)
#define P2(a, b) a##b

/* The socket-oriented system calls are handled unusally in Linux.
   They are all gated through the single `socketcall' system call number.
   `socketcall' takes two arguments: the first is the subcode, specifying
   which socket function is being called; and the second is a pointer to
   the arguments to the specific function.

   The .S files for the other calls just #define socket and #include this.
   They also #define a 'number-of-arguments' word in NARGS, which
   defaults to 3.  */

#ifndef NARGS
#ifdef socket
#error NARGS not defined
#endif
#define NARGS 3
#endif

/* 0(r1) and 4(r1) are reserved by the ABI, 8(r1), 12(r1), 16(r1) are used
   for temp saves.  44(r1) is used to save r30.  */
#define STACKBLOCK 20

#ifndef __socket
# ifndef NO_WEAK_ALIAS
#  define __socket P(__,socket)
# else
#  define __socket socket
# endif
#endif

	.text
ENTRY(__socket)
	stwu r1,-48(r1)
	cfi_adjust_cfa_offset(48)
#if NARGS >= 1
	stw  r3,STACKBLOCK(r1)
#endif
#if NARGS >= 2
	stw  r4,4+STACKBLOCK(r1)
#endif
#if NARGS >= 3
	stw  r5,8+STACKBLOCK(r1)
#endif
#if NARGS >= 4
	stw  r6,12+STACKBLOCK(r1)
#endif
#if NARGS >= 5
	stw  r7,16+STACKBLOCK(r1)
#endif
#if NARGS >= 6
	stw  r8,20+STACKBLOCK(r1)
#endif
#if NARGS >= 7
#error too many arguments!
#endif
	mflr	r9
	SETUP_PIC;
	stw	r9,52(r1)
	cfi_offset (lr, 4)

	li	r3,SYS_ify(socketcall)
	li	r4,P(SOCKOP_,socket)
	addi	r5,r1,STACKBLOCK
	CANCEL_JUMPTARGET
	lwz	r9,52(r1)
	addi	r1,r1,48
	cfi_adjust_cfa_offset(-48)
	mtlr	r9
	cfi_restore(lr)
	b	__syscall_cancel_error@local;
END (__socket)

#ifndef NO_WEAK_ALIAS
weak_alias (__socket, socket)
#endif
