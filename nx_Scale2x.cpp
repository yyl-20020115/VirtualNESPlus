#include "DebugOut.h"
#include "DirectDraw.h"
#include "COM.h"
#include "Render.h"
/*
 * This file is part of the Advance project.
 *
 * Copyright (C) 1999-2002 Andrea Mazzoleni
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * This file contains a C and MMX implentation of the Scale2x effect.
 *
 * You can found an high level description of the effect at :
 *
 * http://scale2x.sourceforge.net/scale2x.html
 *
 * Alternatively at the previous license terms, you are allowed to use this
 * code in your program with these conditions:
 * - the program is not used in commercial activities.
 * - the whole source code of the program is released with the binary.
 * - derivative works of the program are allowed.
 */

// Note
//
// This code has deleted most from the original code.
// An original code is acquirable from the website of Advanced project.
// MS VisualC++ and ProcessorPack are required for compile of this code.
//
static	void	internal_scale2x_16_mmx_single(euI16* dst, const euI16* src0, const euI16* src1, const euI16* src2, unsigned count)
{
	/* always do the first and last run */
	count -= 2*4;
#if   !_WIN64
	__asm {
		mov		eax, src0;
		mov		ebx, src1;
		mov		ecx, src2;
		mov		edx, dst;
		mov		esi, count;
;
		/* first run */;
		/* set the current, current_pre, current_next registers */;
		pxor		mm0, mm0;		/* use a fake black out of screen */;
		movq		mm7, qword ptr [ebx+0];
		movq		mm1, qword ptr [ebx+8];
		psrlq		mm0, 48;
		psllq		mm1, 48;
		movq		mm2, mm7;
		movq		mm3, mm7;
		psllq		mm2, 16;
		psrlq		mm3, 16;
		por		mm0, mm2;
		por		mm1, mm3;

		/* current_upper */;
		movq		mm6, qword ptr [eax];
;
		/* compute the upper-left pixel for dst0 on %%mm2 */;
		/* compute the upper-right pixel for dst0 on %%mm4 */;
		movq		mm2, mm0;
		movq		mm4, mm1;
		movq		mm3, mm0;
		movq		mm5, mm1;
		pcmpeqw		mm2, mm6;
		pcmpeqw		mm4, mm6;
		pcmpeqw		mm3, qword ptr [ecx];
		pcmpeqw		mm5, qword ptr [ecx];
		pandn		mm3, mm2;
		pandn		mm5, mm4;
		movq		mm2, mm0;
		movq		mm4, mm1;
		pcmpeqw		mm2, mm1;
		pcmpeqw		mm4, mm0;
		pandn		mm2, mm3;
		pandn		mm4, mm5;
		movq		mm3, mm2;
		movq		mm5, mm4;
		pand		mm2, mm6;
		pand		mm4, mm6;
		pandn		mm3, mm7;
		pandn		mm5, mm7;
		por		mm2, mm3;
		por		mm4, mm5;
;
		/* set *dst0 */;
		movq		mm3, mm2;
		punpcklwd	mm2, mm4;
		punpckhwd	mm3, mm4;
		movq		qword ptr [edx+0], mm2;
		movq		qword ptr [edx+8], mm3;
;
		/* next */;
		add		eax, 8;
		add		ebx, 8;
		add		ecx, 8;
		add		edx, 16;
;
		/* central runs */;
		shr		esi, 2;
		jz		label1;
		align 4;
label0:
;
		/* set the current, current_pre, current_next registers */;
		movq		mm0, qword ptr [ebx-8];
		movq		mm7, qword ptr [ebx+0];
		movq		mm1, qword ptr [ebx+8];
		psrlq		mm0, 48;
		psllq		mm1, 48;
		movq		mm2, mm7;
		movq		mm3, mm7;
		psllq		mm2, 16;
		psrlq		mm3, 16;
		por		mm0, mm2;
		por		mm1, mm3;
;
		/* current_upper */;
		movq		mm6, qword ptr [eax];
;
		/* compute the upper-left pixel for dst0 on %%mm2 */;
		/* compute the upper-right pixel for dst0 on %%mm4 */;
		movq		mm2, mm0;
		movq		mm4, mm1;
		movq		mm3, mm0;
		movq		mm5, mm1;
		pcmpeqw		mm2, mm6;
		pcmpeqw		mm4, mm6;
		pcmpeqw		mm3, qword ptr [ecx];
		pcmpeqw		mm5, qword ptr [ecx];
		pandn		mm3, mm2;
		pandn		mm5, mm4;
		movq		mm2, mm0;
		movq		mm4, mm1;
		pcmpeqw		mm2, mm1;
		pcmpeqw		mm4, mm0;
		pandn		mm2, mm3;
		pandn		mm4, mm5;
		movq		mm3, mm2;
		movq		mm5, mm4;
		pand		mm2, mm6;
		pand		mm4, mm6;
		pandn		mm3, mm7;
		pandn		mm5, mm7;
		por		mm2, mm3;
		por		mm4, mm5;
;
		/* set *dst0 */;
		movq		mm3, mm2;
		punpcklwd	mm2, mm4;
		punpckhwd	mm3, mm4;
		movq		qword ptr [edx+0], mm2;
		movq		qword ptr [edx+8], mm3;
;
		/* next */;
		add		eax, 8;
		add		ebx, 8;
		add		ecx, 8;
		add		edx, 16;
;
		dec		esi;
		jnz		label0;
label1:
		/* final run */;
		/* set the current, current_pre, current_next registers */;
		movq		mm0, qword ptr [ebx-8];
		movq		mm7, qword ptr [ebx+0];
		pxor		mm1, mm1;		/* use a fake black out of screen */;
		psrlq		mm0, 48;
		psllq		mm1, 48;
		movq		mm2, mm7;
		movq		mm3, mm7;
		psllq		mm2, 16;
		psrlq		mm3, 16;
		por		mm0, mm2;
		por		mm1, mm3;
;
		/* current_upper */;
		movq		mm6, qword ptr [eax];
;
		/* compute the upper-left pixel for dst0 on %%mm2 */;
		/* compute the upper-right pixel for dst0 on %%mm4 */;
		movq		mm2, mm0;
		movq		mm4, mm1;
		movq		mm3, mm0;
		movq		mm5, mm1;
		pcmpeqw		mm2, mm6;
		pcmpeqw		mm4, mm6;
		pcmpeqw		mm3, qword ptr [ecx];
		pcmpeqw		mm5, qword ptr [ecx];
		pandn		mm3, mm2;
		pandn		mm5, mm4;
		movq		mm2, mm0;
		movq		mm4, mm1;
		pcmpeqw		mm2, mm1;
		pcmpeqw		mm4, mm0;
		pandn		mm2, mm3;
		pandn		mm4, mm5;
		movq		mm3, mm2;
		movq		mm5, mm4;
		pand		mm2, mm6;
		pand		mm4, mm6;
		pandn		mm3, mm7;
		pandn		mm5, mm7;
		por		mm2, mm3;
		por		mm4, mm5;
;
		/* set *dst0 */;
		movq		mm3, mm2;
		punpcklwd	mm2, mm4;
		punpckhwd	mm3, mm4;
		movq		qword ptr [edx+0], mm2;
		movq		qword ptr [edx+8], mm3;
;
		mov		src0, eax;
		mov		src1, ebx;
		mov		src2, ecx;
		mov		dst, edx;
		mov		count, esi;
;
		emms;
	}
#endif
}

void	internal_scale2x_32_mmx_single(euI32* dst, const euI32* src0, const euI32* src1, const euI32* src2, unsigned count)
{
	/* always do the first and last run */
	count -= 2*2;
#if   !_WIN64
	__asm {
		mov		eax, src0;
		mov		ebx, src1;
		mov		ecx, src2;
		mov		edx, dst;
		mov		esi, count;
;
		/* first run */;
		/* set the current, current_pre, current_next registers */;
		pxor		mm0, mm0;
		movq		mm7, qword ptr [ebx+0];
		movq		mm1, qword ptr [ebx+8];
		psrlq		mm0, 32;
		psllq		mm1, 32;
		movq		mm2, mm7;
		movq		mm3, mm7;
		psllq		mm2, 32;
		psrlq		mm3, 32;
		por		mm0, mm2;
		por		mm1, mm3;
;
		/* current_upper */;
		movq		mm6, qword ptr [eax];
;
		/* compute the upper-left pixel for dst0 on %%mm2 */;
		/* compute the upper-right pixel for dst0 on %%mm4 */;
		movq		mm2, mm0;
		movq		mm4, mm1;
		movq		mm3, mm0;
		movq		mm5, mm1;
		pcmpeqd		mm2, mm6;
		pcmpeqd		mm4, mm6;
		pcmpeqd		mm3, qword ptr [ecx];
		pcmpeqd		mm5, qword ptr [ecx];
		pandn		mm3, mm2;
		pandn		mm5, mm4;
		movq		mm2, mm0;
		movq		mm4, mm1;
		pcmpeqd		mm2, mm1;
		pcmpeqd		mm4, mm0;
		pandn		mm2, mm3;
		pandn		mm4, mm5;
		movq		mm3, mm2;
		movq		mm5, mm4;
		pand		mm2, mm6;
		pand		mm4, mm6;
		pandn		mm3, mm7;
		pandn		mm5, mm7;
		por		mm2, mm3;
		por		mm4, mm5;
;
		/* set *dst0 */;
		movq		mm3, mm2;
		punpckldq	mm2, mm4;
		punpckhdq	mm3, mm4;
		movq		qword ptr [edx+0], mm2;
		movq		qword ptr [edx+8], mm3;
;
		/* next */;
		add		eax, 8;
		add		ebx, 8;
		add		ecx, 8;
		add		edx, 16;
;
		/* central runs */;
		shr		esi, 1;
		jz		label1;
label0:
;
		/* set the current, current_pre, current_next registers */;
		movq		mm0, qword ptr [ebx-8];
		movq		mm7, qword ptr [ebx+0];
		movq		mm1, qword ptr [ebx+8];
		psrlq		mm0, 32;
		psllq		mm1, 32;
		movq		mm2, mm7;
		movq		mm3, mm7;
		psllq		mm2, 32;
		psrlq		mm3, 32;
		por		mm0, mm2;
		por		mm1, mm3;
;
		/* current_upper */;
		movq		mm6, qword ptr[eax];
;
		/* compute the upper-left pixel for dst0 on %%mm2 */;
		/* compute the upper-right pixel for dst0 on %%mm4 */;
		movq		mm2, mm0;
		movq		mm4, mm1;
		movq		mm3, mm0;
		movq		mm5, mm1;
		pcmpeqd		mm2, mm6;
		pcmpeqd		mm4, mm6;
		pcmpeqd		mm3, qword ptr[ecx];
		pcmpeqd		mm5, qword ptr[ecx];
		pandn		mm3, mm2;
		pandn		mm5, mm4;
		movq		mm2, mm0;
		movq		mm4, mm1;
		pcmpeqd		mm2, mm1;
		pcmpeqd		mm4, mm0;
		pandn		mm2, mm3;
		pandn		mm4, mm5;
		movq		mm3, mm2;
		movq		mm5, mm4;
		pand		mm2, mm6;
		pand		mm4, mm6;
		pandn		mm3, mm7;
		pandn		mm5, mm7;
		por		mm2, mm3;
		por		mm4, mm5;
;
		/* set *dst0 */;
		movq		mm3, mm2;
		punpckldq	mm2, mm4;
		punpckhdq	mm3, mm4;
		movq		qword ptr [edx+0], mm2;
		movq		qword ptr [edx+8], mm3;
;
		/* next */;
		add		eax, 8;
		add		ebx, 8;
		add		ecx, 8;
		add		edx, 16;
;
		dec		esi;
		jnz		label0;
label1:;
		;
		/* final run */;
		/* set the current, current_pre, current_next registers */;
		movq		mm0, qword ptr [ebx-8];
		movq		mm7, qword ptr [ebx+0];
		pxor		mm1, mm1;
		psrlq		mm0, 32;
		psllq		mm1, 32;
		movq		mm2, mm7;
		movq		mm3, mm7;
		psllq		mm2, 32;
		psrlq		mm3, 32;
		por		mm0, mm2;
		por		mm1, mm3;
;
		/* current_upper */;
		movq		mm6, qword ptr [eax];
;
		/* compute the upper-left pixel for dst0 on %%mm2 */;
		/* compute the upper-right pixel for dst0 on %%mm4 */;
		movq		mm2, mm0;
		movq		mm4, mm1;
		movq		mm3, mm0;
		movq		mm5, mm1;
		pcmpeqd		mm2, mm6;
		pcmpeqd		mm4, mm6;
		pcmpeqd		mm3, qword ptr [ecx];
		pcmpeqd		mm5, qword ptr [ecx];
		pandn		mm3, mm2;
		pandn		mm5, mm4;
		movq		mm2, mm0;
		movq		mm4, mm1;
		pcmpeqd		mm2, mm1;
		pcmpeqd		mm4, mm0;
		pandn		mm2, mm3;
		pandn		mm4, mm5;
		movq		mm3, mm2;
		movq		mm5, mm4;
		pand		mm2, mm6;
		pand		mm4, mm6;
		pandn		mm3, mm7;
		pandn		mm5, mm7;
		por		mm2, mm3;
		por		mm4, mm5;
;
		/* set *dst0 */;
		movq		mm3, mm2;
		punpckldq	mm2, mm4;
		punpckhdq	mm3, mm4;
		movq		qword ptr [edx+0], mm2;
		movq		qword ptr [edx+8], mm3;
;
		mov		src0, eax;
		mov		src1, ebx;
		mov		src2, ecx;
		mov		dst, edx;
		mov		count, esi;
;
		emms;
	}
#endif
}

void	internal_scale2x_16_mmx(euI16* dst0, euI16* dst1, const euI16* src0, const euI16* src1, const euI16* src2, unsigned count)
{
	internal_scale2x_16_mmx_single(dst0, src0, src1, src2, count);
	internal_scale2x_16_mmx_single(dst1, src2, src1, src0, count);
}

void	internal_scale2x_32_mmx(euI32* dst0, euI32* dst1, const euI32* src0, const euI32* src1, const euI32* src2, unsigned count)
{
	internal_scale2x_32_mmx_single(dst0, src0, src1, src2, count);
	internal_scale2x_32_mmx_single(dst1, src2, src1, src0, count);
}

