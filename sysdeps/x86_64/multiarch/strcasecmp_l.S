/* Multiple versions of strcasecmp
   Copyright (C) 2009-2015 Free Software Foundation, Inc.
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

#define STRCMP __strcasecmp_l
#define USE_AS_STRCASECMP_L

#include <sysdep.h>
#include <init-arch.h>

# include "locale-defines.h"


# define STRCMP_SSSE3	__strcasecmp_l_ssse3
# define STRCMP_SSE2	__strcasecmp_l_sse2
# define __GI_STRCMP	__GI___strcasecmp_l

/* Define multiple versions only for the definition in libc.  Don't
   define multiple versions for strncmp in static library since we
   need strncmp before the initialization happened.  */
#if (defined SHARED || !defined USE_AS_STRNCMP) && IS_IN (libc)
	.text
ENTRY(STRCMP)
	.type	STRCMP, @gnu_indirect_function
	/* Manually inlined call to __get_cpu_features.  */
	cmpl	$0, __cpu_features+KIND_OFFSET(%rip)
	jne	1f
	call	__init_cpu_features
1:
	leaq	__strcasecmp_sse2_unaligned_l(%rip), %rax
	testl   $bit_Fast_Unaligned_Load, __cpu_features+FEATURE_OFFSET+index_Fast_Unaligned_Load(%rip)
	jnz     3f
2:	leaq	STRCMP_SSSE3(%rip), %rax
	testl	$bit_SSSE3, __cpu_features+CPUID_OFFSET+index_SSSE3(%rip)
	jnz	3f
	leaq	STRCMP_SSE2(%rip), %rax
3:	ret
END(STRCMP)

ENTRY(__strcasecmp)
	.type	__strcasecmp, @gnu_indirect_function
	/* Manually inlined call to __get_cpu_features.  */
	cmpl	$0, __cpu_features+KIND_OFFSET(%rip)
	jne	1f
	call	__init_cpu_features
1:
	leaq	__strcasecmp_sse2_unaligned(%rip), %rax
	testl   $bit_Fast_Unaligned_Load, __cpu_features+FEATURE_OFFSET+index_Fast_Unaligned_Load(%rip)
	jnz     3f
2:	leaq	__strcasecmp_ssse3(%rip), %rax
	testl	$bit_SSSE3, __cpu_features+CPUID_OFFSET+index_SSSE3(%rip)
	jnz	3f
	leaq	__strcasecmp_sse2(%rip), %rax
3:	ret

END(__strcasecmp)
weak_alias (__strcasecmp, strcasecmp)

# undef ENTRY
# define ENTRY(name) \
	.type STRCMP_SSE2, @function; \
	.align 16; \
	.globl STRCMP_SSE2; \
	.hidden STRCMP_SSE2; \
	STRCMP_SSE2: cfi_startproc; \
	CALL_MCOUNT
# undef END
# define END(name) \
	cfi_endproc; .size STRCMP_SSE2, .-STRCMP_SSE2

#  define ENTRY2(name) \
	.type __strcasecmp_sse2, @function; \
	.align 16; \
	.globl __strcasecmp_sse2; \
	.hidden __strcasecmp_sse2; \
	__strcasecmp_sse2: cfi_startproc; \
	CALL_MCOUNT
#  define END2(name) \
	cfi_endproc; .size __strcasecmp_sse2, .-__strcasecmp_sse2

# undef libc_hidden_builtin_def
/* It doesn't make sense to send libc-internal strcmp calls through a PLT.
   The speedup we get from using SSE4.2 instruction is likely eaten away
   by the indirect call in the PLT.  */
# define libc_hidden_builtin_def(name) \
	.globl __GI_STRCMP; __GI_STRCMP = STRCMP_SSE2
#endif

#include "../strcmp.S"

weak_alias (__strcasecmp_l, strcasecmp_l)
libc_hidden_def (strcasecmp_l)
