/* Call to terminate the current thread.  NaCl/ARM version.
   Copyright (C) 2015 Free Software Foundation, Inc.
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

/* This bit is machine-specific: Switch stacks to SP and call FUNC (ARG).  */
static void __attribute__ ((noreturn))
call_on_stack (void *sp, void *func, void *arg)
{
  register void *r0 asm ("r0") = arg;
  asm volatile ("bic sp, %[sp], %[dmask]\n"
                "sfi_blx %[func]"
                :
                : [sp] "r" (sp), [func] "r" (func), "r" (r0),
                  [dmask] "i" (0xc0000000));
  __builtin_trap ();
}

#include <sysdeps/nacl/exit-thread.c>
