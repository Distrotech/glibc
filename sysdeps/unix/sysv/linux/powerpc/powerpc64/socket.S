/* Copyright (C) 1995-2014 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   see <http://www.gnu.org/licenses/>.  */

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

#ifndef __socket
# ifndef NO_WEAK_ALIAS
#  define __socket P(__,socket)
# else
#  define __socket socket
# endif
#endif

#if _CALL_ELF == 2
#define FRAMESIZE (FRAME_MIN_SIZE+16+64)
#define STACKBLOCK (FRAME_MIN_SIZE+16)
#else
#define FRAMESIZE (FRAME_MIN_SIZE+16)
#define STACKBLOCK (FRAMESIZE+FRAME_PARM_SAVE) /* offset to parm save area.  */
#endif

	.text
ENTRY(__socket)
	CALL_MCOUNT NARGS
	stdu r1,-FRAMESIZE(r1)
	cfi_adjust_cfa_offset(FRAMESIZE)

#if NARGS >= 1
	std  r3,STACKBLOCK(r1)
#endif
#if NARGS >= 2
	std  r4,8+STACKBLOCK(r1)
#endif
#if NARGS >= 3
	std  r5,16+STACKBLOCK(r1)
#endif
#if NARGS >= 4
	std  r6,24+STACKBLOCK(r1)
#endif
#if NARGS >= 5
	std  r7,32+STACKBLOCK(r1)
#endif
#if NARGS >= 6
	std  r8,40+STACKBLOCK(r1)
#endif
#if NARGS >= 7
	std  r9,48+STACKBLOCK(r1)
#endif
#if NARGS >= 8
	std  r10,56+STACKBLOCK(r1)
#endif
#if NARGS >= 9
#error too many arguments!
#endif
	mflr	r9
	std	r9,FRAMESIZE+FRAME_LR_SAVE(r1)
	cfi_offset(lr, FRAME_LR_SAVE);

	li	r3,SYS_ify (socketcall)
	li	r4,P(SOCKOP_,socket)
	addi	r5,r1,STACKBLOCK
	bl	HIDDEN_JUMPTARGET(__syscall_cancel)
	nop
	bl	JUMPTARGET(__syscall_cancel_error)
	nop
	ld	r9,FRAMESIZE+FRAME_LR_SAVE(r1)
	mtlr	r9
	cfi_restore(lr)
	addi	r1,r1,FRAMESIZE
	cfi_adjust_cfa_offset(-FRAMESIZE)
	blr
END (__socket)

#ifndef NO_WEAK_ALIAS
weak_alias (__socket, socket)
#endif
