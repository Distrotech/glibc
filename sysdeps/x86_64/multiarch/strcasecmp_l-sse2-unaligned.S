/* strcasecmp with unaligned loads
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

#ifndef NOT_IN_libc

#include "sysdep.h"

# include "locale-defines.h"


ENTRY (__strcasecmp_sse2_unaligned)
	movq	__libc_tsd_LOCALE@gottpoff(%rip), %rax
	mov	%fs:(%rax), %rdx
        // XXX 5 byte should be before the function
        /* 5-byte NOP.  */
	.byte	0x0f,0x1f,0x44,0x00,0x00

END (__strcasecmp_sse2_unaligned)

ENTRY (__strcasecmp_sse2_unaligned_l)
	movzbl	(%rdi), %eax
	movzbl	(%rsi), %ecx
	mov	LOCALE_TOLOWER(%rdx), %r11
	movl	(%r11,%rax,4), %eax
	subl	(%r11,%rcx,4), %eax
	je	L(next)
L(return):
	ret
L(next):
	test	%ecx, %ecx
	je	L(return)
	leaq	1(%rsi), %rsi
	leaq	1(%rdi), %rdi

	pxor	%xmm7, %xmm7
	movl	%edi, %eax
	andl	$4095, %eax
	cmpl	$4032, %eax
	jg	L(cross_page_start)
	movl	%esi, %eax
	andl	$4095, %eax
	cmpl	$4032, %eax
	jg	L(cross_page_start)
	movdqu	(%rdi), %xmm1
	movdqu	(%rsi), %xmm0
	pcmpeqb	%xmm1, %xmm0
	pminub	%xmm1, %xmm0
	pxor	%xmm1, %xmm1
	pcmpeqb	%xmm7, %xmm0
	pmovmskb %xmm0, %ecx
	test	%ecx, %ecx
	je	L(next_48_bytes)

L(loop1):
	bsf	%ecx, %r8d
	movzbl	(%rdi,%r8), %eax
	movzbl	(%rsi,%r8), %r8d
	movl	(%r11,%rax,4), %eax
	subl	(%r11,%r8,4), %eax
	jne	L(return)
	test	%r8d, %r8d
	je	L(return)
	leaq	-1(%rcx), %rax
	andq	%rax, %rcx
	je	L(next_48_bytes)
 jmp L(loop1)


 .p2align 4
L(next_48_bytes):
	movdqu	16(%rdi), %xmm6
	movdqu	16(%rsi), %xmm3
	movdqu	32(%rdi), %xmm5
	pcmpeqb	%xmm6, %xmm3
	movdqu	32(%rsi), %xmm2
	pminub	%xmm6, %xmm3
	pcmpeqb	%xmm1, %xmm3
	movdqu	48(%rdi), %xmm4
	pcmpeqb	%xmm5, %xmm2
	pmovmskb %xmm3, %edx
	movdqu	48(%rsi), %xmm0
	pminub	%xmm5, %xmm2
	pcmpeqb	%xmm1, %xmm2
	pcmpeqb	%xmm4, %xmm0
	pmovmskb %xmm2, %eax
	sal	$16, %edx
	pminub	%xmm4, %xmm0
	pcmpeqb	%xmm1, %xmm0
	salq	$32, %rax
	orq	%rdx, %rax
	pmovmskb %xmm0, %ecx
	salq	$48, %rcx
	orq	%rax, %rcx
	jne	L(caseloop2)
L(main_loop_header):
	leaq	64(%rdi), %rdx
	movl	$4096, %ecx
	pxor	%xmm9, %xmm9
	andq	$-64, %rdx
	subq	%rdi, %rdx
	leaq	(%rdi, %rdx), %rax
	addq	%rsi, %rdx
	movq	%rdx, %rsi
	andl	$4095, %esi
	subq	%rsi, %rcx
	shrq	$6, %rcx
	movq	%rcx, %rsi
	jmp	L(loop_start)

L(caseloop2):
	bsf	%rcx, %r8
	movzbl	(%rdi,%r8), %eax
	movzbl	(%rsi,%r8), %r8d
	movl	(%r11,%rax,4), %eax
	subl	(%r11,%r8,4), %eax
	jne	L(return)
	test	%r8d, %r8d
	je	L(return)
	leaq	-1(%rcx), %rax
	andq	%rax, %rcx
	je	L(main_loop_header)
	jmp	L(caseloop2)



L(caseloop3):
	bsfq	%rax, %rdx
	leaq	-1(%rax), %r10
	andq	%rax, %r10
	movzbl	(%rdi, %rdx), %eax
	movzbl	(%rsi, %rdx), %edx
	movl	(%r11, %rax, 4), %eax
	movl	(%r11, %rdx, 4), %edx
	testl	%eax, %eax
	je	L(zero3)
	cmpl	%edx, %eax
	je	L(casecnt3)
L(zero3):
	subl	%edx, %eax
	ret
L(casecnt3):
	movq	%rdi, %rax
	movq	%rsi, %rdx
	testq	%r10, %r10
	je	L(back_to_loop)
	movq	%r10, %rax
	jmp	L(caseloop3)



	.p2align 4
L(loop):
	addq	$64, %rax
	addq	$64, %rdx
L(loop_start):
	testq	%rsi, %rsi
	leaq	-1(%rsi), %rsi
	je	L(loop_cross_page)
L(back_to_loop):
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
	pminub	%xmm1, %xmm0
	movdqa	48(%rax), %xmm3
	pcmpeqb	%xmm2, %xmm5
	pcmpeqb	%xmm3, %xmm6
	pminub	%xmm2, %xmm5
	pminub	%xmm3, %xmm6
	pminub	%xmm5, %xmm0
	pminub	%xmm6, %xmm0
	pcmpeqb	%xmm7, %xmm0
	pmovmskb %xmm0, %ecx
	testl	%ecx, %ecx
	je	L(loop)
	pcmpeqb	%xmm7, %xmm5
	movdqu	(%rdx), %xmm0
	pcmpeqb	%xmm7, %xmm1
	movdqa	(%rax), %xmm2
	pcmpeqb	%xmm2, %xmm0
	pminub	%xmm2, %xmm0
	pcmpeqb	%xmm7, %xmm6
	pcmpeqb	%xmm7, %xmm0
	pmovmskb %xmm1, %ecx
	pmovmskb %xmm5, %r8d
	pmovmskb %xmm0, %edi
	salq	$16, %rcx
	salq	$32, %r8
	pmovmskb %xmm6, %esi
	orq	%r8, %rcx
	orq	%rdi, %rcx
	salq	$48, %rsi
	orq	%rsi, %rcx
	movq	%rax, %rdi
	movq	%rdx, %rsi
	jmp	L(caseloop2)

	.p2align 4
L(loop_cross_page):
	xor	%r10, %r10
	movq	%rdx, %r9
	and	$63, %r9
	subq	%r9, %r10

	movdqa	(%rdx, %r10), %xmm0
	movdqa	16(%rdx, %r10), %xmm1
	movdqu	(%rax, %r10), %xmm2
	movdqu	16(%rax, %r10), %xmm3
	pcmpeqb	%xmm2, %xmm0
	movdqa	32(%rdx, %r10), %xmm5
	pcmpeqb	%xmm3, %xmm1
	pminub	%xmm2, %xmm0
	movdqa	48(%rdx, %r10), %xmm6
	pminub	%xmm3, %xmm1
	movdqu	32(%rax, %r10), %xmm2
	movdqu	48(%rax, %r10), %xmm3
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
	salq	$16, %rcx
	salq	$32, %r8
	pmovmskb %xmm6, %esi
	orq	%r8, %rdi
	orq	%rcx, %rdi
	salq	$48, %rsi
	orq	%rsi, %rdi
	movq	%r9, %rcx
	movq	$63, %rsi
	shrq	%cl, %rdi
	test	%rdi, %rdi
	je	L(back_to_loop)
	movq	%rdi, %r10
	movq	%rax, %rdi
	movq	%rdx, %rsi
	movq	%r10, %rax
	jmp	L(caseloop3)

L(cross_page_start):
	xor	%edx, %edx
	jmp	L(cross_page)
	.p2align 4
L(cross_page_loop):
	addq	$1, %rdx
	cmpq	$64, %rdx
	je	L(main_loop_header)
L(cross_page):
	movzbl	(%rdi, %rdx), %eax
	movzbl	(%rsi, %rdx), %ecx
	cmp	%eax, %ecx
	je	L(skip_table)
	movl	(%r11, %rax, 4), %eax
	subl	(%r11, %rcx, 4), %ecx
	jne	L(ret2)
L(skip_table):
	test	%eax, %eax
	jne	L(cross_page_loop)
L(ret2):
	ret


END (__strcasecmp_sse2_unaligned_l)
#endif
