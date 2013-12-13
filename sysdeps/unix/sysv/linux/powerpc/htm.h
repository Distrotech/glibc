/* Shared HTM header.  Emulate transactional execution facility intrinsics for
   compilers and assemblers that do not support the intrinsics and instructions
   yet. 

   Copyright (C) 2013 Free Software Foundation, Inc.
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

#ifndef _HTM_H
#define _HTM_H 1

#ifdef __ASSEMBLER__

/* tbegin.  */
.macro TBEGIN
	.byte 0x7c,0x00,0x05,0x1d
.endm

/* tend. 0  */
.macro TEND
	.byte 0x7c,0x00,0x05,0x5d
.endm

/* tabort. code  */
.macro TABORT code
        .byte 0x7c
        .byte \code
	.byte 0x07
	.byte 0x1d
.endm

/* mfspr %dst,130  */
.macro TEXASR dst
	mfspr \dst,130
.endm

#else

/* Official HTM intrinsics interface matching gcc, but works
   on older gcc compatible compilers and binutils.
   We should somehow detect if the compiler supports it, because
   it may be able to generate slightly better code.  */

#define TBEGIN ".byte 0x7c,0x00,0x05,0x1d"
#define TEND   ".byte 0x7c,0x00,0x05,0x5d"
#define TABORT ".byte 0x7c,%1,0x07,0x1d"

/* Definitions used for TEXASR Failure code (bits 0:6), they need to be even
   because tabort. always sets the first bit.  */
#define _ABORT_LOCK_BUSY       0x3f
#define _ABORT_NESTED_TRYLOCK  0x3e

#define __force_inline        inline __attribute__((__always_inline__))

#ifndef __HTM__

#define _TEXASRU_EXTRACT_BITS(TEXASR,BITNUM,SIZE) \
  (((TEXASR) >> (31-(BITNUM))) & ((1<<(SIZE))-1))
#define _TEXASRU_FAILURE_PERSISTENT(TEXASRU) \
  _TEXASRU_EXTRACT_BITS(TEXASRU, 7, 1)

static __force_inline
unsigned int _tbegin (void)
{
  unsigned int ret;
  asm volatile (
    TBEGIN "\t\n"
    "mfcr   %0,128\t\n"
    "rlwinm %0,%0,3,1\t\n"
    "subfic %0,%0,1\t\n"
    : "=r" (ret) :: "cr0", "memory");
  return ret;
}

static __force_inline
unsigned int _tend (void)
{
  unsigned int ret;
  asm volatile (
    TEND "\t\n"
    "mfcr   %0,128\t\n"
    "rlwinm %0,%0,3,1\t\n"
    "subfic %0,%0,1\t\n"
    : "=r" (ret) :: "cr0", "memory");
  return ret;
}

static __force_inline
unsigned int _tabort (unsigned int code)
{
  unsigned int ret;
  asm volatile (
    TABORT "\t\n"
    "mfcr     %0,128\t\n"
    "rlwinm   %0,%0,3,1\t\n"
    "subfic   %0,%0,1\t\n"
    : "=r" (ret) : "r" (code) : "cr0", "memory");
  return ret;
}

static __force_inline
unsigned long _texasru (void)
{
  unsigned long ret;
  asm volatile (
    "mfspr %0,131\t\n"
    : "=r" (ret));
  return ret;
}

#define __builtin_tbegin(tdb)       _tbegin ()
#define __builtin_tend(nested)      _tend ()
#define __builtin_tabort(abortcode) _tabort (abortcode)
#define __builtin_get_texasru()     _texasru ()

#else
# include <htmintrin.h>
#endif /* ifndef __HTM__  */

#endif

#endif
