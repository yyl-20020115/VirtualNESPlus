#include "DebugOut.h"
#include "DirectDraw.h"
#include "COM.h"
//
// 24bit Normal
// Double: Pal Pal Pal Pal
// DoubleScan: Pal PalScan Pal PalScan
void	CDirectDraw::Render24bpp_Normal(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
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
				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 9) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 11) = (BYTE)(edx & 0xff);

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(DWORD*)(edi + 12) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 15) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 18) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

				eax += 8;
				ebx += 8;
				edi += 24;
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 9) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 11) = (BYTE)(edx & 0xff);

			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(DWORD*)(edi + 12) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 15) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 18) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 24;
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
			_r24bn_loop_fw:
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 6], ecx;
				mov[edi + 9], dx;
				shr		edx, 16;
				mov[edi + 11], dl;

				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 12], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 15], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 18], ecx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 24];

				sub		width, 8;
				jg		_r24bn_loop_fw;
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
			_r24bn_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r24bn_skip1;
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 6], ecx;
				mov[edi + 9], dx;
				shr		edx, 16;
				mov[edi + 11], dl;
			_r24bn_skip1:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r24bn_skip2;
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 12], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 15], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 18], ecx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;
			_r24bn_skip2:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 24];

				sub		width, 8;
				jg		_r24bn_loop;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
	}
}

//
// 24bit Scanline
//
void	CDirectDraw::Render24bpp_Scanline(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
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
				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 9) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 11) = (BYTE)(edx & 0xff);

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(DWORD*)(edi + 12) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 15) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 18) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

				eax += 8;
				ebx += 8;
				edi += 24;
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
				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 9) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 11) = (BYTE)(edx & 0xff);

				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(DWORD*)(edi + 12) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 15) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 18) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

				eax += 8;
				ebx += 8;
				edi += 24;
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 9) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 11) = (BYTE)(edx & 0xff);

			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(DWORD*)(edi + 12) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 15) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 18) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 24;
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
					goto _r32bn_skip1_;
				else
					*(DWORD*)ebx = edx;

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 9) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 11) = (BYTE)(edx & 0xff);

			_r32bn_skip1_:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2_;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(DWORD*)(edi + 12) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 15) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 18) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				edx = *(WORD*)(esi + 4 * (edx & 0xff));
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

			_r32bn_skip2_:
				eax += 8;
				ebx += 8;
				edi += 24;
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
			_r24bs_loop_fw:
				// check previous!!
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 6], ecx;
				mov[edi + 9], dx;
				shr		edx, 16;
				mov[edi + 11], dl;

				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 12], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 15], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 18], ecx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 24];

				sub		width, 8;
				jg		_r24bs_loop_fw;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r24bs_loop2_fw:
				mov		edx, [eax + 0];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 6], ecx;
				mov[edi + 9], dx;
				shr		edx, 16;
				mov[edi + 11], dl;

				mov		edx, [eax + 4];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 12], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 15], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 18], ecx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 24];

				sub		width, 8;
				jg		_r24bs_loop2_fw;
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
			_r24bs_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r24bs_skip1;
				// #### The assembly code `mov ebx+0, edx` is translating to the following instruction in English:  Move the value of `edx` to the memory location `ebx + 0`.  In other words, this instruction copies the value of `edx` to the memory location `ebx` plus zero.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 6], ecx;
				mov[edi + 9], dx;
				shr		edx, 16;
				mov[edi + 11], dl;
			_r24bs_skip1:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r24bs_skip2;
				// #### Move the contents of the register EDX into the memory location stored in the register EBX plus 4.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 12], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 15], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 18], ecx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;
			_r24bs_skip2:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 24];

				sub		width, 8;
				jg		_r24bs_loop;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r24bs_loop2:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r24bs_skip3;
				mov[ebx + 0], edx;		// #### Write the second time.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 6], ecx;
				mov[edi + 9], dx;
				shr		edx, 16;
				mov[edi + 11], dl;
			_r24bs_skip3:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r24bs_skip4;
				mov[ebx + 4], edx		// #### Write the second one.
					movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 12], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 15], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 18], ecx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;
			_r24bs_skip4:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 24];

				sub		width, 8;
				jg		_r24bs_loop2;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
	}
}

//
// 24bit Double
//
void	CDirectDraw::Render24bpp_Double(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);


				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

				eax += 8;
				ebx += 8;
				edi += 48;
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);


				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

				eax += 8;
				ebx += 8;
				edi += 48;
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 48;
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
					goto _r32bn_skip1__;
				else
					*(DWORD*)ebx = edx;

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

			_r32bn_skip1__:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2__;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

			_r32bn_skip2__:
				eax += 8;
				ebx += 8;
				edi += 48;
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
			_r24bn_d_loop_fw:
				// check previous!!
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;

				// check previous!!
				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], edx;
				shr		edx, 16;
				mov[edi + 47], dl;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bn_d_loop_fw;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r24bn_d_loop2_fw:
				mov		edx, [eax + 0];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;

				mov		edx, [eax + 4];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], dx;
				shr		edx, 16;
				mov[edi + 47], dl;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bn_d_loop2_fw;
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
			_r24bn_d_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r24bn_d_skip1;
				// #### 
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;
			_r24bn_d_skip1:
				// check previous!!;
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r24bn_d_skip2;
				// #### Move the value of EDX to EBX, shifting it 4 bytes to the right.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], edx;
				shr		edx, 16;
				mov[edi + 47], dl;
			_r24bn_d_skip2:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bn_d_loop;
			}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPal;
				mov		edi, pDst;
			_r24bn_d_loop2:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r24bn_d_skip3;
				mov[ebx + 0], edx;		// #### The second time, write it down.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;
			_r24bn_d_skip3:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r24bn_d_skip4;
				mov[ebx + 4], edx;		// #### Write the second one.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], dx;
				shr		edx, 16;
				mov[edi + 47], dl;
			_r24bn_d_skip4:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bn_d_loop2;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
	}
}

//
// 24bit Double Scanline
//
void	CDirectDraw::Render24bpp_DoubleScanline(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);


				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

				eax += 8;
				ebx += 8;
				edi += 48;
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);


				*(DWORD*)(ebx + 4) = edx = *(DWORD*)(eax + 4);

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

				eax += 8;
				ebx += 8;
				edi += 48;
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

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

			_r32bn_skip1:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

			_r32bn_skip2:
				eax += 8;
				ebx += 8;
				edi += 48;
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
					goto _r32bn_skip1_;
				else
					*(DWORD*)ebx = edx;

				*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 3) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 6) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 9) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 12) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 15) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 18) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 21) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 23) = (BYTE)(edx & 0xff);

			_r32bn_skip1_:
				edx = *(DWORD*)(eax + 4);
				if (edx == *(DWORD*)(ebx + 4))
					goto _r32bn_skip2_;
				else
					*(DWORD*)(ebx + 4) = edx;

				*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 27) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(DWORD*)(edi + 30) = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(DWORD*)(edi + 33) = *(DWORD*)(esi + 4 * (edx & 0xff));
				edx >>= 8;
				*(WORD*)(edi + 36) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 39) = (WORD)(edx & 0xffff);
				edx >>= 8;
				*(WORD*)(edi + 42) = (WORD)(edx & 0xffff);
				*(WORD*)(edi + 45) = (WORD)(edx & 0xffff);
				edx >>= 16;
				edx = *(DWORD*)(esi + 4 * (edx & 0xff));
				*(BYTE*)(edi + 47) = (BYTE)(edx & 0xff);

			_r32bn_skip2_:
				eax += 8;
				ebx += 8;
				edi += 48;
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
			_r24bs_d_loop_fw:
				mov		edx, [eax + 0];
				mov[ebx + 0], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;

				mov		edx, [eax + 4];
				mov[ebx + 4], edx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], edx;
				shr		edx, 16;
				mov[edi + 47], dl;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bs_d_loop_fw;
		}
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r24bs_d_loop2_fw:
				mov		edx, [eax + 0];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;

				mov		edx, [eax + 4];
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], dx;
				shr		edx, 16;
				mov[edi + 47], dl;

				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bs_d_loop2_fw;
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
			_r24bs_d_loop:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r24bs_d_skip1;
				// #### The assembly code you provided is written in x86 machine language, and it reads:  `mov ebx, edx`  This instruction moves the value of the EDX register to the EBX register. In other words, it copies the contents of EDX to EBX.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;
			_r24bs_d_skip1:
				// check previous!!;
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r24bs_d_skip2;
				// #### Move the value of EDX into EBX plus 4.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], edx;
				shr		edx, 16;
				mov[edi + 47], dl;
			_r24bs_d_skip2:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bs_d_loop;
			};
			width = SCREEN_WIDTH;
			pDst += pitch;
			__asm {
				mov		eax, pScn;
				mov		ebx, pDlt;
				mov		esi, pPalScan;
				mov		edi, pDst;
			_r24bs_d_loop2:
				// check previous!!
				mov		edx, [eax + 0];
				cmp		edx, [ebx + 0];
				je		_r24bs_d_skip3;
				mov[ebx + 0], edx;		// #### The second time, write it down.
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 0], ecx;
				mov[edi + 3], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 6], ecx;
				mov[edi + 9], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 12], ecx;
				mov[edi + 15], ecx;
				mov[edi + 18], edx;
				mov[edi + 21], dx;
				shr		edx, 16;
				mov[edi + 23], dl;
			_r24bs_d_skip3:
				// check previous!!
				mov		edx, [eax + 4];
				cmp		edx, [ebx + 4];
				je		_r24bs_d_skip4;
				mov[ebx + 4], edx;		// #### Write the second time
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 24], ecx;
				mov[edi + 27], ecx;
				movzx		ecx, dl;
				mov		ecx, [esi + ecx * 4];
				shr		edx, 8;
				mov[edi + 30], ecx;
				mov[edi + 33], ecx;
				movzx		ecx, dl;
				shr		edx, 8;
				mov		ecx, [esi + ecx * 4];
				mov		edx, [esi + edx * 4];
				mov[edi + 36], ecx;
				mov[edi + 39], ecx;
				mov[edi + 42], edx;
				mov[edi + 45], dx;
				shr		edx, 16;
				mov[edi + 47], dl;
			_r24bs_d_skip4:
				lea		eax, [eax + 8];
				lea		ebx, [ebx + 8];
				lea		edi, [edi + 48];

				sub		width, 8;
				jg		_r24bs_d_loop2;
			}
			pScn += RENDER_WIDTH;
			pDlt += SCREEN_WIDTH;
			pDst += pitch;
		}
#endif
}
}
