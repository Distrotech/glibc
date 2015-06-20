/* strcmp with unaligned loads
   Copyright (C) 2013-2015 Free Software Foundation, Inc.
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

#include "sysdep.h"

ENTRY ( __strcmp_sse2_unaligned)
#ifdef AS_STRNCMP
	lea	-1(%rdx), %r10
	test	%rdx, %rdx
	je	L(ret_zero)
L(back_to_start):
	xor	%rdx, %rdx
#endif

	pxor	%xmm7, %xmm7
	movl	%esi, %eax
	andl	$4095, %eax
	cmpl	$4032, %eax
	jg	L(cross_page)

	movl	%edi, %eax
	andl	$4095, %eax
	cmpl	$4032, %eax
	jg	L(cross_page)
#ifdef AS_STRNCMP
	cmp	$64, %r10
	jae	L(dont_set_mask)
	bts	%r10, %rdx
L(dont_set_mask):
#endif

	movdqu	(%rdi), %xmm1
	movdqu	(%rsi), %xmm0
	pcmpeqb	%xmm1, %xmm0
	pminub	%xmm1, %xmm0
	pcmpeqb	%xmm7, %xmm0
	pmovmskb %xmm0, %eax
#ifdef AS_STRNCMP
	or	%dx, %ax
#else
	test	%eax, %eax
#endif
	je	L(next_48_bytes)
	bsf	%eax, %edx
	movzbl	(%rdi, %rdx), %eax
	movzbl	(%rsi, %rdx), %edx
	subl	%edx, %eax
	ret
#ifdef AS_STRNCMP
	L(ret_zero):
	xor	%eax, %eax
	ret
#endif
	.p2align 4
L(next_48_bytes):
	movdqu	16(%rdi), %xmm6
	movdqu	16(%rsi), %xmm3
	movdqu	32(%rdi), %xmm5
	pcmpeqb	%xmm6, %xmm3
	movdqu	32(%rsi), %xmm2
	pminub	%xmm6, %xmm3
	pcmpeqb	%xmm7, %xmm3
	movdqu	48(%rdi), %xmm4
	pcmpeqb	%xmm5, %xmm2
	movdqu	48(%rsi), %xmm0
	pminub	%xmm5, %xmm2
	pcmpeqb	%xmm7, %xmm2
	pcmpeqb	%xmm4, %xmm0
	pmovmskb %xmm2, %eax
	salq	$32, %rax
#ifdef AS_STRNCMP
	or	%rdx, %rax
#endif
	pmovmskb %xmm3, %edx
	sal	$16, %edx
	pminub	%xmm4, %xmm0
	pcmpeqb	%xmm7, %xmm0
	orq	%rdx, %rax
	pmovmskb %xmm0, %ecx
	salq	$48, %rcx
	orq	%rcx, %rax
	je	L(main_loop_header)
L(return):
	bsf	%rax, %rdx
	movzbl	(%rdi, %rdx), %eax
	movzbl	(%rsi, %rdx), %edx
	subl	%edx, %eax
	ret

L(main_loop_header):
#ifdef USE_AVX2
	vpxor	%xmm7, %xmm7, %xmm7
#endif
	leaq	64(%rdi), %rdx
	andq	$-64, %rdx
# ifdef AS_STRNCMP
	addq	%rdi, %r10
	subq	%rdx, %r10
# endif
	subq	%rdi, %rdx
	leaq	(%rdi, %rdx), %rax
	addq	%rsi, %rdx
	movl	$4096, %esi
	mov	%edx, %ecx
	andl	$4095, %ecx
	sub	%ecx, %esi
	shr	$6, %esi
#ifdef AS_STRNCMP
	mov	%r10, %r9
	addq	%rdx, %r10
	shr	$6, %r9
	cmp	%r9, %rsi
	jb	L(dont_set_page_bound)
	mov	%r9, %rsi
L(dont_set_page_bound):
#endif

	.p2align 4
L(loop):
	add	$-1, %rsi
	ja	L(loop_cross_page)
L(back_to_loop):
#ifdef USE_AVX2
	vmovdqa	(%rax), %ymm4
	vmovdqa	32(%rax), %ymm5
	vmovdqu	(%rdx), %ymm0
	vmovdqu	32(%rdx), %ymm1
	vpcmpeqb %ymm4, %ymm0, %ymm0
	vpminub	%ymm4, %ymm0, %ymm0
	vpcmpeqb %ymm5, %ymm1, %ymm1
	vpminub	%ymm5, %ymm1, %ymm1
	vpminub	%ymm0, %ymm1, %ymm2
	vpcmpeqb %ymm7, %ymm2, %ymm2
	addq	$64, %rax
	addq	$64, %rdx
	vpmovmskb %ymm2, %edi
	test	%edi, %edi
	je	L(loop)
	shl	$32, %rdi
	vpcmpeqb %ymm7, %ymm0, %ymm0
	vpmovmskb %ymm0, %ecx
	or	%rdi, %rcx
	vzeroupper
#else
	movdqu	(%rdx), %xmm0
	movdqu	16(%rdx), %xmm1
	movdqa	(%rax), %xmm2
	movdqa	16(%rax), %xmm3
	pcmpeqb	%xmm2, %xmm0
	movdqu	32(%rdx), %xmm5
	pcmpeqb	%xmm3, %xmm1
	pminub	%xmm2, %xmm0
	movdqu	48(%rdx), %xmm6
	pminub	%xmm3, %xmm1
	movdqa	32(%rax), %xmm2
	movdqa	48(%rax), %xmm3
	pcmpeqb	%xmm2, %xmm5
	pcmpeqb	%xmm3, %xmm6
	addq	$64, %rax
	pminub	%xmm2, %xmm5
	pminub	%xmm3, %xmm6
	addq	$64, %rdx
	pminub	%xmm5, %xmm6
	pminub	%xmm1, %xmm6
	pminub	%xmm0, %xmm6
	pcmpeqb	%xmm7, %xmm6
	pmovmskb %xmm6, %ecx
	testl	%ecx, %ecx
	je	L(loop)
	pcmpeqb	%xmm7, %xmm0
	pcmpeqb	%xmm7, %xmm1
	pcmpeqb	%xmm7, %xmm5
	pmovmskb %xmm0, %edi
	pmovmskb %xmm1, %esi
	pmovmskb %xmm5, %r8d
	salq	$48, %rcx
	salq	$32, %r8
	orq	%r8, %rcx
	orq	%rdi, %rcx
	sal	$16, %esi
	orq	%rsi, %rcx
#endif
	bsfq	%rcx, %rcx
	movzbl	-64(%rax, %rcx), %eax
	movzbl	-64(%rdx, %rcx), %edx
	subl	%edx, %eax
	ret

	.p2align 4
L(loop_cross_page):
#ifdef AS_STRNCMP
	mov	%r10, %r9
	sub	%rdx, %r9
	cmp	$64, %r9
	jb	L(prepare_back_to_start)
#endif


	mov	%edx, %ecx
	and	$63, %ecx
	neg	%rcx
#ifdef USE_AVX2
	vmovdqu	(%rax, %rcx), %ymm4
	vmovdqu	32(%rax, %rcx), %ymm5
	vmovdqa	(%rdx, %rcx), %ymm0
	vmovdqa	32(%rdx, %rcx), %ymm1
	vpcmpeqb %ymm4, %ymm0, %ymm0
	vpminub	%ymm4, %ymm0, %ymm0
	vpcmpeqb %ymm5, %ymm1, %ymm1
	vpminub	%ymm5, %ymm1, %ymm1
	vpminub	%ymm0, %ymm1, %ymm2
	vpcmpeqb %ymm7, %ymm2, %ymm2
	vpmovmskb %ymm2, %esi
	shl 	$32, %rsi
	vpcmpeqb %ymm7, %ymm0, %ymm0
	vpmovmskb %ymm0, %edi
	or    	%rsi, %rdi
#else
	movdqa	(%rdx, %rcx), %xmm0
	movdqa	16(%rdx, %rcx), %xmm1
	movdqu	(%rax, %rcx), %xmm2
	movdqu	16(%rax, %rcx), %xmm3
	pcmpeqb	%xmm2, %xmm0
	movdqa	32(%rdx, %rcx), %xmm5
	pcmpeqb	%xmm3, %xmm1
	pminub	%xmm2, %xmm0
	movdqa	48(%rdx, %rcx), %xmm6
	pminub	%xmm3, %xmm1
	movdqu	32(%rax, %rcx), %xmm2
	movdqu	48(%rax, %rcx), %xmm3
	pcmpeqb	%xmm2, %xmm5
	pcmpeqb	%xmm3, %xmm6
	pminub	%xmm2, %xmm5
	pminub	%xmm3, %xmm6

	pcmpeqb	%xmm7, %xmm0
	pcmpeqb	%xmm7, %xmm1
	pcmpeqb	%xmm7, %xmm5
	pcmpeqb	%xmm7, %xmm6

	pmovmskb %xmm1, %ecx
	pmovmskb %xmm5, %r8d
	pmovmskb %xmm0, %edi
	sal	$16, %ecx
	salq	$32, %r8
	pmovmskb %xmm6, %esi
	orq	%r8, %rdi
	orq	%rcx, %rdi
	salq	$48, %rsi
	orq	%rsi, %rdi
#endif
	mov	%edx, %ecx
	mov	$63, %esi
#ifdef AS_STRNCMP
	shr	$6, %r9
	sub	$1, %r9
	cmp	%r9, %rsi
	jb	L(dont_set_bound2)
	mov	%r9, %rsi
L(dont_set_bound2):
#endif
	shrq	%cl, %rdi
	test	%rdi, %rdi
	je	L(back_to_loop)
#ifdef USE_AVX2
	vzeroupper
#endif
	bsfq	%rdi, %rcx
	movzbl	(%rax, %rcx), %eax
	movzbl	(%rdx, %rcx), %edx
	subl	%edx, %eax
	ret

#ifdef AS_STRNCMP
L(prepare_back_to_start):
# ifdef USE_AVX2
	vzeroupper
# endif
	mov	%r9, %r10
	mov	%rdx, %rsi
	mov	%rax, %rdi
	jmp	L(back_to_start)
#endif


L(cross_page):
	xorl	%edx, %edx
	jmp	L(cross_page_loop_start)
	.p2align 4
L(cross_page_loop):
	add	$1, %edx
	cmp	$64, %edx
	je	L(main_loop_header)
L(cross_page_loop_start):
	movzbl	(%rdi, %rdx), %eax
	movzbl	(%rsi, %rdx), %ecx
	subl	%ecx, %eax
	jne	L(different)
	cmp	%rdx, %r10
	je	L(different)

	test	%ecx, %ecx
	jne	L(cross_page_loop)
L(different):
	ret
END (__strcmp_sse2_unaligned)
