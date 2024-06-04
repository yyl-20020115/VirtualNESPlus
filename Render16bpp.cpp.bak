#include "DebugOut.h"
#include "DirectDraw.h"
#include "COM.h"
//
// 16bit Normal
// Double: Pal Pal Pal Pal
// DoubleScan: Pal PalScan Pal PalScan
void	CDirectDraw::Render16bpp_Normal(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	LPBYTE	pDlt = (LPBYTE)lpDlt;
	LPBYTE	pPal;
	DWORD	width;
	DWORD	pitch = ddsd.lPitch;

	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		if (!(m_LineColormode[i] & 0x80)) {
			pPal = (LPBYTE)m_cnPalette[m_LineColormode[i] & 0x07];
		}
		else {
			pPal = (LPBYTE)m_mnPalette[m_LineColormode[i] & 0x07];
		}

		BOOL	bFWrite = FALSE;
		if (bForceWrite || *pDlt != m_LineColormode[i]) {
			bFWrite = TRUE;
			*pDlt = m_LineColormode[i];
		}
		pDlt += 4;

		width = SCREEN_WIDTH;

#if _WIN64
		if (bFWrite) {
			LPBYTE eax = pScn;
			LPBYTE ebx = pDlt;
			LPBYTE esi = pPal;
			LPBYTE edi = pDst;
			DWORD  edx = 0;
			DWORD  ecx = 0;
			do {
				*(DWORD*)(ebx + 0) = edx = *(DWORD*)(eax + 0);
				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

				eax += 8;
				ebx += 8;
				edi += 16;
				width -= 8;
			} while (width > 0);

			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
		else {
			LPBYTE	eax = pScn;
			LPBYTE	ebx = pDlt;
			LPBYTE	esi = pPal;
			LPBYTE	edi = pDst;
			DWORD	ecx = 0;
			DWORD	edx = 0;
			do {
				edx = *(DWORD*)eax;
				if (edx == *(DWORD*)ebx)
					goto _r32bn_skip1;
				else
					*(DWORD*)ebx = edx;

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 16;
				width -= 8;
			} while (width > 0);

			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}

#else
		if (bFWrite) {
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bn_loop_fw:
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 4], cx;
				mov[edi + 6], dx;

				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 8], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 10], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], cx;
				mov[edi + 14], dx;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 16];

				sub		width, 8;
				jg		_r16bn_loop_fw;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
		else {
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bn_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r16bn_skip1;
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 4], cx;
				mov[edi + 6], dx;
			_r16bn_skip1:
				// check previous!!;
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r16bn_skip2;
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 8], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 10], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], cx;
				mov[edi + 14], dx;
			_r16bn_skip2:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 16];

				sub		width, 8;
				jg		_r16bn_loop;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
	}
}

//
// 16bit Scanline
//
void	CDirectDraw::Render16bpp_Scanline(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	LPBYTE	pDlt = (LPBYTE)lpDlt;
	LPBYTE	pPal, pPalScan;
	DWORD	width = SCREEN_WIDTH;
	DWORD	pitch = ddsd.lPitch;

	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		if (!(m_LineColormode[i] & 0x80)) {
			pPal = (LPBYTE)m_cnPalette[m_LineColormode[i] & 0x07];
			pPalScan = (LPBYTE)m_csPalette[m_LineColormode[i] & 0x07];
		}
		else {
			pPal = (LPBYTE)m_mnPalette[m_LineColormode[i] & 0x07];
			pPalScan = (LPBYTE)m_msPalette[m_LineColormode[i] & 0x07];
		}

		BOOL	bFWrite = FALSE;
		if (bForceWrite || *pDlt != m_LineColormode[i]) {
			bFWrite = TRUE;
			*pDlt = m_LineColormode[i];
		}
		pDlt += 4;

		width = SCREEN_WIDTH;

#if _WIN64
		if (bFWrite) {
			LPBYTE eax = pScn;
			LPBYTE ebx = pDlt;
			LPBYTE esi = pPal;
			LPBYTE edi = pDst;
			DWORD  edx = 0;
			DWORD  ecx = 0;
			do {
				*(DWORD*)(ebx + 0) = edx = *(DWORD*)(eax + 0);
				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

				eax += 8;
				ebx += 8;
				edi += 16;
				width -= 8;
			} while (width > 0);

			width = SCREEN_WIDTH;
			pDst += pitch;

			eax = pScn;
			ebx = pDlt;
			esi = pPalScan;
			edi = pDst;
			edx = 0;
			ecx = 0;
			do {
				*(DWORD*)(ebx + 0) = edx = *(DWORD*)(eax + 0);
				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

				eax += 8;
				ebx += 8;
				edi += 16;
				width -= 8;
			} while (width > 0);

			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;

		}
		else
		{
			LPBYTE	eax = pScn;
			LPBYTE	ebx = pDlt;
			LPBYTE	esi = pPal;
			LPBYTE	edi = pDst;
			DWORD	ecx = 0;
			DWORD	edx = 0;
			do {
				edx = *(DWORD*)eax;
				if (edx == *(DWORD*)ebx)
					goto _r32bn_skip1;
				else
					*(DWORD*)ebx = edx;

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 16;
				width -= 8;
			} while (width > 0);

			width = SCREEN_WIDTH;
			pDst += pitch;

			eax = pScn;
			ebx = pDlt;
			esi = pPalScan;
			edi = pDst;
			edx = 0;
			ecx = 0;

			do {
				edx = *(DWORD*)eax;
				if (edx == *(DWORD*)ebx)
					goto _r32bn_skip1_;
				else
					*(DWORD*)ebx = edx;

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip1_:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2_;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip2_:
				eax += 8;
				ebx += 8;
				edi += 16;
				width -= 8;
			} while (width > 0);


			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;

		}

#else
		if (bFWrite) {
			//ASM
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bs_loop_fw:
				// check previous!!
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 4], cx;
				mov[edi + 6], dx;

				// check previous!!
				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 8], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 10], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		edx, [esi + edx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], cx;
				mov[edi + 14], dx;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 16];

				sub		width, 8;
				jg		_r16bs_loop_fw;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r16bs_loop2_fw:
				mov		edx, [eax + 0];
				//			mov		[ebx+0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 4], cx;
				mov[edi + 6], dx;

				mov		edx, [eax + 4];
				//			mov		[ebx+4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 8], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 10], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], cx;
				mov[edi + 14], dx;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 16];

				sub		width, 8;
				jg		_r16bs_loop2_fw;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
		else {
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bs_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r16bs_skip1;
				//			mov		[ebx+0], edx		// 1âÒñ⁄ÇÕèëÇ´çûÇ‹Ç»Ç¢;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 4], cx;
				mov[edi + 6], dx;
			_r16bs_skip1:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r16bs_skip2;
				//			mov		[ebx+4], edx		// 1âÒñ⁄ÇÕèëÇ´çûÇ‹Ç»Ç¢;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 8], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 10], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], cx;
				mov[edi + 14], dx;
			_r16bs_skip2:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 16];

				sub		width, 8;
				jg		_r16bs_loop;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r16bs_loop2:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r16bs_skip3;
				mov[ebx + 0], edx;		// 2âÒñ⁄ÇÕèëÇ´çûÇﬁ;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 4], cx;
				mov[edi + 6], dx;
			_r16bs_skip3:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r16bs_skip4;
				mov[ebx + 4], edx;		// 2âÒñ⁄ÇÕèëÇ´çûÇﬁ;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 8], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 10], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], cx;
				mov[edi + 14], dx;
			_r16bs_skip4:;
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 16];

				sub		width, 8;
				jg		_r16bs_loop2;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
	}
}

//
// 16bit Double
//
void	CDirectDraw::Render16bpp_Double(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	LPBYTE	pDlt = (LPBYTE)lpDlt;
	LPBYTE	pPal;
	DWORD	width;
	DWORD	pitch = ddsd.lPitch;

	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		if (!(m_LineColormode[i] & 0x80)) {
			pPal = (LPBYTE)m_cnPalette[m_LineColormode[i] & 0x07];
		}
		else {
			pPal = (LPBYTE)m_mnPalette[m_LineColormode[i] & 0x07];
		}

		BOOL	bFWrite = FALSE;
		if (bForceWrite || *pDlt != m_LineColormode[i]) {
			bFWrite = TRUE;
			*pDlt = m_LineColormode[i];
		}
		pDlt += 4;

		width = SCREEN_WIDTH;

#if _WIN64
		if (bFWrite) {
			LPBYTE eax = pScn;
			LPBYTE ebx = pDlt;
			LPBYTE esi = pPal;
			LPBYTE edi = pDst;
			DWORD  edx = 0;
			DWORD  ecx = 0;
			do {
				*(DWORD*)(ebx + 0) = edx = *(DWORD*)(eax + 0);
				
				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;

				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));

				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);
			width = SCREEN_WIDTH;
			pDst += pitch;
			eax = pScn;
			ebx = pDlt;
			esi = pPal;
			edi = pDst;
			edx = 0;
			ecx = 0;
			do {
				*(DWORD*)(ebx + 0) = edx = *(DWORD*)(eax + 0);

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;

				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));

				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);

			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
		else {
			LPBYTE	eax = pScn;
			LPBYTE	ebx = pDlt;
			LPBYTE	esi = pPal;
			LPBYTE	edi = pDst;
			DWORD	ecx = 0;
			DWORD	edx = 0;
			do {
				edx = *(DWORD*)eax;
				if (edx == *(DWORD*)ebx)
					goto _r32bn_skip1;
				else
					*(DWORD*)ebx = edx;

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);

			width = SCREEN_WIDTH;
			pDst += pitch;

			eax = pScn;
			ebx = pDlt;
			esi = pPal;
			edi = pDst;
			ecx = 0;
			edx = 0;
			do {
				edx = *(DWORD*)eax;
				if (edx == *(DWORD*)ebx)
					goto _r32bn_skip1___;
				else
					*(DWORD*)ebx = edx;

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

			_r32bn_skip1___:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2___;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip2___:
				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);

			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#else
		if (bFWrite) {
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bn_d_loop_fw:
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;

				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bn_d_loop_fw;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bn_d_loop2_fw:
				mov		edx, [eax + 0];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;

				mov		edx, [eax + 4];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bn_d_loop2_fw;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
		else {
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bn_d_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r16bn_d_skip1;
				//			mov		[ebx+0], edx		// 1âÒñ⁄ÇÕèëÇ´çûÇ‹Ç»Ç¢;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;
			_r16bn_d_skip1:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r16bn_d_skip2;
				//			mov		[ebx+4], edx		// 1âÒñ⁄ÇÕèëÇ´çûÇ‹Ç»Ç¢;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;
			_r16bn_d_skip2:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bn_d_loop;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bn_d_loop2:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r16bn_d_skip3;
				mov[ebx + 0], edx;		// 2âÒñ⁄ÇÕèëÇ´çûÇﬁ;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;
			_r16bn_d_skip3:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r16bn_d_skip4;
				mov[ebx + 4], edx;		// 2âÒñ⁄ÇÕèëÇ´çûÇﬁ;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;
			_r16bn_d_skip4:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bn_d_loop2;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
	}
}

//
// 16bit Double Scanline
//
void	CDirectDraw::Render16bpp_DoubleScanline(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	LPBYTE	pDlt = (LPBYTE)lpDlt;
	LPBYTE	pPal, pPalScan;
	DWORD	width;
	DWORD	pitch = ddsd.lPitch;

	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		if (!(m_LineColormode[i] & 0x80)) {
			pPal = (LPBYTE)m_cnPalette[m_LineColormode[i] & 0x07];
			pPalScan = (LPBYTE)m_csPalette[m_LineColormode[i] & 0x07];
		}
		else {
			pPal = (LPBYTE)m_mnPalette[m_LineColormode[i] & 0x07];
			pPalScan = (LPBYTE)m_msPalette[m_LineColormode[i] & 0x07];
		}

		BOOL	bFWrite = FALSE;
		if (bForceWrite || *pDlt != m_LineColormode[i]) {
			bFWrite = TRUE;
			*pDlt = m_LineColormode[i];
		}
		pDlt += 4;

		width = SCREEN_WIDTH;

#if _WIN64
		if (bFWrite) {
			LPBYTE eax = pScn;
			LPBYTE ebx = pDlt;
			LPBYTE esi = pPal;
			LPBYTE edi = pDst;
			DWORD  edx = 0;
			DWORD  ecx = 0;
			do {
				*(DWORD*)(ebx + 0) = edx = *(DWORD*)(eax + 0);

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;

				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));

				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);
			width = SCREEN_WIDTH;
			pDst += pitch;
			eax = pScn;
			ebx = pDlt;
			esi = pPalScan;
			edi = pDst;
			edx = 0;
			ecx = 0;
			do {
				*(DWORD*)(ebx + 0) = edx = *(DWORD*)(eax + 0);

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;

				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));

				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));

				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);

			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
		else {
			LPBYTE	eax = pScn;
			LPBYTE	ebx = pDlt;
			LPBYTE	esi = pPal;
			LPBYTE	edi = pDst;
			DWORD	ecx = 0;
			DWORD	edx = 0;
			do {
				edx = *(DWORD*)eax;
				if (edx == *(DWORD*)ebx)
					goto _r32bn_skip1;
				else
					*(DWORD*)ebx = edx;

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);

			width = SCREEN_WIDTH;
			pDst += pitch;

			eax = pScn;
			ebx = pDlt;
			esi = pPalScan;
			edi = pDst;
			ecx = 0;
			edx = 0;
			do {
				edx = *(DWORD*)eax;
				if (edx == *(DWORD*)ebx)
					goto _r32bn_skip1___;
				else
					*(DWORD*)ebx = edx;

				*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

			_r32bn_skip1___:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2___;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(WORD*)(edi + 16) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 18) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 20) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 22) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 24) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 26) = *(WORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 28) = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 30) = *(WORD*)(esi + 4 * (edx & 0xff));
			_r32bn_skip2___:
				eax += 8;
				ebx += 8;
				edi += 32;
				width -= 8;
			} while (width > 0);

			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}

#else
		if (bFWrite) {
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bs_d_loop_fw:
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;

				// check previous!!
				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bs_d_loop_fw;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r16bs_d_loop2_fw:
				mov		edx, [eax + 0];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;

				mov		edx, [eax + 4];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bs_d_loop2_fw;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
		else {
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r16bs_d_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r16bs_d_skip1;
				//			mov		[ebx+0], edx		// 1âÒñ⁄ÇÕèëÇ´çûÇ‹Ç»Ç¢;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;
			_r16bs_d_skip1:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r16bs_d_skip2;
				//			mov		[ebx+4], edx		// 1âÒñ⁄ÇÕèëÇ´çûÇ‹Ç»Ç¢;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;
			_r16bs_d_skip2:;
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bs_d_loop;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r16bs_d_loop2:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r16bs_d_skip3;
				mov[ebx + 0], edx;		// 2âÒñ⁄ÇÕèëÇ´çûÇﬁ;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], cx;
				mov[edi + 2], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 4], cx;
				mov[edi + 6], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 8], cx;
				mov[edi + 10], cx;
				mov[edi + 12], dx;
				mov[edi + 14], dx;
			_r16bs_d_skip3:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r16bs_d_skip4;
				mov[ebx + 4], edx;		// 2âÒñ⁄ÇÕèëÇ´çûÇﬁ;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 16], cx;
				mov[edi + 18], cx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 20], cx;
				mov[edi + 22], cx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 24], cx;
				mov[edi + 26], cx;
				mov[edi + 28], dx;
				mov[edi + 30], dx;
			_r16bs_d_skip4:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 32];

				sub		width, 8;
				jg		_r16bs_d_loop2;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
	}
}
