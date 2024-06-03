#include "DebugOut.h"
#include "DirectDraw.h"
#include "COM.h"
#include <immintrin.h>
//
// 8bit Normal
//
void	CDirectDraw::Render8bpp_Normal(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	DWORD	width = SCREEN_WIDTH;
	DWORD	pitch = ddsd.lPitch;

#if _WIN64
	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		PBYTE esi = pScn;
		PBYTE edi = pDst;
		DWORD ecx = width;
		DWORD edx = 0x40404040;
		DWORD eax = 0;
		DWORD ebx = 0;
		do {
			eax = *(DWORD*)(esi + 0);
			ebx = *(DWORD*)(esi + 4);
			eax |= ebx;
			ebx |= edx;
			*(DWORD*)(edi + 0) = eax;
			*(DWORD*)(edi + 4) = ebx;
			esi += 8;
			edi += 8;
			ecx -= 8;
		} while (ecx > 0);

		pScn += RENDER_WIDTH;
		pDst += pitch;
	}

#else
	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		//ASM_COMMENT_OUT
		__asm {
			mov		esi, pScn;
			mov		edi, pDst;
			mov		ecx, width;
			mov		edx, 0x40404040;
		_r8bn_loop:
			mov		eax, [esi + 0];
			mov		ebx, [esi + 4];
			or eax, edx;
			or ebx, edx;
			mov[edi + 0], eax;
			mov[edi + 4], ebx;

			lea		esi, [esi + 8];
			lea		edi, [edi + 8];
			sub		ecx, 8;
			jg		_r8bn_loop;
		}
		pScn += RENDER_WIDTH;
		pDst += pitch;
	}
#endif
}

//
// 8bit Scanline
//
void	CDirectDraw::Render8bpp_Scanline(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	DWORD	width = SCREEN_WIDTH;
	DWORD	pitch = ddsd.lPitch;
#if _WIN64
	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		PBYTE esi = pScn;
		PBYTE edi = pDst;
		DWORD ecx = width;
		DWORD edx = 0x40404040;
		DWORD eax = 0;
		DWORD ebx = 0;
		do {
			eax = *(DWORD*)(esi + 0);
			ebx = *(DWORD*)(esi + 4);
			eax |= ebx;
			ebx |= edx;
			*(DWORD*)(edi + 0) = eax;
			*(DWORD*)(edi + 4) = ebx;
			esi += 8;
			edi += 8;
			ecx -= 8;
		} while (ecx > 0);
		pDst += pitch;

		esi = pScn;
		edi = pDst;
		ecx = width;
		edx = 0x80808080;
		do {
			eax = *(DWORD*)(esi + 0);
			ebx = *(DWORD*)(esi + 4);
			eax |= ebx;
			ebx |= edx;
			*(DWORD*)(edi + 0) = eax;
			*(DWORD*)(edi + 4) = ebx;
			esi += 8;
			edi += 8;
			ecx -= 8;
		} while (ecx > 0);

		pScn += RENDER_WIDTH;
		pDst += pitch;
	}

#else
	for (INT i = 0; i < SCREEN_HEIGHT; i++) {
		__asm {
			mov		esi, pScn;
			mov		edi, pDst;
			mov		ecx, width;
			mov		edx, 0x40404040;
		_r8bs_loop:
			mov		eax, [esi + 0];
			mov		ebx, [esi + 4];
			or eax, edx;
			or ebx, edx;
			mov[edi + 0], eax;
			mov[edi + 4], ebx;

			lea		esi, [esi + 8];
			lea		edi, [edi + 8];
			sub		ecx, 8;
			jg		_r8bs_loop;
		}
		pDst += pitch;
		__asm {
			mov		esi, pScn;
			mov		edi, pDst;
			mov		ecx, width;
			mov		edx, 0x80808080;
		_r8bs_loop2:
			mov		eax, [esi + 0];
			mov		ebx, [esi + 4];
			or eax, edx;
			or ebx, edx;
			mov[edi + 0], eax;
			mov[edi + 4], ebx;

			lea		esi, [esi + 8];
			lea		edi, [edi + 8];
			sub		ecx, 8;
			jg		_r8bs_loop2;
		}
		pScn += RENDER_WIDTH;
		pDst += pitch;
	}
#endif
}

//
// 8bit Double
//
void	CDirectDraw::Render8bpp_Double(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	DWORD	width = SCREEN_WIDTH;
	DWORD	pitch = ddsd.lPitch;

	if (!IsMMX()) {
#if _WIN64
		for (INT i = 0; i < SCREEN_HEIGHT; i++) {
			PBYTE esi = pScn;
			PBYTE edi = pDst;
			DWORD ecx = width;
			DWORD edx = 0x40404040;
			DWORD eax = 0;
			DWORD ebx = 0;
			do {
				eax |= *(BYTE*)(esi + 1);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 1);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 0);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 0);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 0) = eax;

				eax |= *(BYTE*)(esi + 3);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 3);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 2);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 2);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 4) = eax;

				eax |= *(BYTE*)(esi + 5);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 5);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 4);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 4);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 8) = eax;

				eax |= *(BYTE*)(esi + 7);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 7);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 6);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 6);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 12) = eax;
				esi += 8;
				edi += 16;
				ecx -= 8;
			} while (ecx > 0);
			pDst += pitch;
			esi = pScn;
			edi = pDst;
			ecx = width;
			edx = 0x40404040;
			eax = 0;
			ebx = 0;
			do {
				eax |= *(BYTE*)(esi + 1);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 1);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 0);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 0);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 0) = eax;

				eax |= *(BYTE*)(esi + 3);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 3);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 2);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 2);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 4) = eax;

				eax |= *(BYTE*)(esi + 5);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 5);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 4);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 4);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 8) = eax;

				eax |= *(BYTE*)(esi + 7);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 7);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 6);
				eax <<= 8;
				eax |= *(BYTE*)(esi + 6);
				eax <<= 8;
				eax |= edx;
				*(DWORD*)(edi + 12) = eax;
				esi += 8;
				edi += 16;
				ecx -= 8;
			} while (ecx > 0);

			pScn += RENDER_WIDTH;
			pDst += pitch;
		}

#else
		for (INT i = 0; i < SCREEN_HEIGHT; i++) {
			//ASM_COMMENT_OUT
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				mov		edx, 0x40404040;
			_r8bn_d_loop:
				mov		al, [esi + 1];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 0];
				mov		ah, al;
				or eax, edx;
				mov[edi + 0], eax;

				mov		al, [esi + 3];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 2];
				mov		ah, al;
				or eax, edx;
				mov[edi + 4], eax;

				mov		al, [esi + 5];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 4];
				mov		ah, al;
				or eax, edx;
				mov[edi + 8], eax;

				mov		al, [esi + 7];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 6];
				mov		ah, al;
				or eax, edx;
				mov[edi + 12], eax;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bn_d_loop;
			}
			pDst += pitch;
			//ASM_COMMENT_OUT
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				mov		edx, 0x40404040;
			_r8bn_d_loop2:
				mov		al, [esi + 1];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 0];
				mov		ah, al;
				or eax, edx;
				mov[edi + 0], eax;

				mov		al, [esi + 3];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 2];
				mov		ah, al;
				or eax, edx;
				mov[edi + 4], eax;

				mov		al, [esi + 5];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 4];
				mov		ah, al;
				or eax, edx;
				mov[edi + 8], eax;

				mov		al, [esi + 7];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 6];
				mov		ah, al;
				or eax, edx;
				mov[edi + 12], eax;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bn_d_loop2;
			}
			pScn += RENDER_WIDTH;
			pDst += pitch;
		}
#endif
	}
	else {
		QWORD	mask = 0x4040404040404040;
#if _WIN64
		for (INT i = 0; i < SCREEN_HEIGHT; i++) {
			PBYTE esi = pScn;
			PBYTE edi = pDst;
			DWORD ecx = width;
			__m128i mm7 = _mm_loadu_si64(&mask);
			
			do {
				__m128i mm0 = _mm_loadu_si32(esi + 0);
				__m128i mm2 = _mm_loadu_si32(esi + 4);
				__m128i mm1 = mm0;
				__m128i mm3 = mm2;
				_mm_unpacklo_epi8(mm0, mm1);
				_mm_unpacklo_epi8(mm2, mm3);

				_mm_or_si128(mm0, mm7);
				_mm_or_si128(mm2, mm7);

				_mm_storeu_si64(edi + 0, mm0);
				_mm_storeu_si64(edi + 8, mm2);

				esi += 8;
				edi += 16;
				ecx -= 8;
			} while (ecx > 0);
			pDst += pitch;
			esi = pScn;
			edi = pDst;
			ecx = width;
			
			do {
				__m128i mm0 = _mm_loadu_si32(esi + 0);
				__m128i mm2 = _mm_loadu_si32(esi + 4);
				__m128i mm1 = mm0;
				__m128i mm3 = mm2;
				_mm_unpacklo_epi8(mm0, mm1);
				_mm_unpacklo_epi8(mm2, mm3);

				_mm_or_si128(mm0, mm7);
				_mm_or_si128(mm2, mm7);

				_mm_storeu_si64(edi + 0, mm0);
				_mm_storeu_si64(edi + 8, mm2);
				esi += 8;
				edi += 16;
				ecx -= 8;
			} while (ecx > 0);

			pScn += RENDER_WIDTH;
			pDst += pitch;

		}
#else
		for (INT i = 0; i < SCREEN_HEIGHT; i++) {
			//ASM_COMMENT_OUT
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				movq		mm7, mask;
			_r8bn_d_loop3:
				movd		mm0, [esi + 0];
				movd		mm2, [esi + 4];
				movq		mm1, mm0;
				movq		mm3, mm2;
				punpcklbw	mm0, mm1;
				punpcklbw	mm2, mm3;
				por		mm0, mm7;
				por		mm2, mm7;
				movq[edi + 0], mm0;
				movq[edi + 8], mm2;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bn_d_loop3;
			}
			pDst += pitch;
			//ASM_COMMENT_OUT
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				movq		mm7, mask;
			_r8bn_d_loop4:
				movd		mm0, [esi + 0];
				movd		mm2, [esi + 4];
				movq		mm1, mm0;
				movq		mm3, mm2;
				punpcklbw	mm0, mm1;
				punpcklbw	mm2, mm3;
				por		mm0, mm7;
				por		mm2, mm7;
				movq[edi + 0], mm0;
				movq[edi + 8], mm2;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bn_d_loop4;
			}
			pScn += RENDER_WIDTH;
			pDst += pitch;
		}
		_mm_empty();
#endif
	}
}

//
// 8bit Double Scanline
//
void	CDirectDraw::Render8bpp_DoubleScanline(LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite)
{
	LPBYTE	pScn = lpRdr;
	LPBYTE	pDst = (LPBYTE)ddsd.lpSurface;
	DWORD	width = SCREEN_WIDTH;
	DWORD	pitch = ddsd.lPitch;

	if (!IsMMX()) {
#if _WIN64
		PBYTE esi = pScn;
		PBYTE edi = pDst;
		DWORD ecx = width;
		DWORD edx = 0x40404040;
		DWORD eax = 0;
		DWORD ebx = 0;
		do {
			eax |= *(BYTE*)(esi + 1);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 1);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 0);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 0);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 0) = eax;

			eax |= *(BYTE*)(esi + 3);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 3);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 2);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 2);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 4) = eax;

			eax |= *(BYTE*)(esi + 5);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 5);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 4);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 4);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 8) = eax;

			eax |= *(BYTE*)(esi + 7);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 7);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 6);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 6);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 12) = eax;
			esi += 8;
			edi += 16;
			ecx -= 8;
		} while (ecx > 0);
		pDst += pitch;
		esi = pScn;
		edi = pDst;
		ecx = width;
		edx = 0x80808080;
		eax = 0;
		ebx = 0;
		do {
			eax |= *(BYTE*)(esi + 1);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 1);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 0);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 0);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 0) = eax;

			eax |= *(BYTE*)(esi + 3);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 3);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 2);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 2);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 4) = eax;

			eax |= *(BYTE*)(esi + 5);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 5);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 4);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 4);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 8) = eax;

			eax |= *(BYTE*)(esi + 7);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 7);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 6);
			eax <<= 8;
			eax |= *(BYTE*)(esi + 6);
			eax <<= 8;
			eax |= edx;
			*(DWORD*)(edi + 12) = eax;
			esi += 8;
			edi += 16;
			ecx -= 8;
		} while (ecx > 0);
		pScn += RENDER_WIDTH;
		pDst += pitch;

#else
		for (INT i = 0; i < SCREEN_HEIGHT; i++) {
			//ASM_COMMENT_OUT
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				mov		edx, 0x40404040;
			_r8bs_d_loop:
				mov		al, [esi + 1];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 0];
				mov		ah, al;
				or eax, edx;
				mov[edi + 0], eax;

				mov		al, [esi + 3];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 2];
				mov		ah, al;
				or eax, edx;
				mov[edi + 4], eax;

				mov		al, [esi + 5];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 4];
				mov		ah, al;
				or eax, edx;
				mov[edi + 8], eax;

				mov		al, [esi + 7];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 6];
				mov		ah, al;
				or eax, edx;
				mov[edi + 12], eax;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bs_d_loop;
	}
			//ASM_COMMENT_OUT

			pDst += pitch;
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				mov		edx, 0x80808080;
			_r8bs_d_loop2:
				mov		al, [esi + 1];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 0];
				mov		ah, al;
				or eax, edx;
				mov[edi + 0], eax;

				mov		al, [esi + 3];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 2];
				mov		ah, al;
				or eax, edx;
				mov[edi + 4], eax;

				mov		al, [esi + 5];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 4];
				mov		ah, al;
				or eax, edx;
				mov[edi + 8], eax;

				mov		al, [esi + 7];
				mov		ah, al;
				shl		eax, 16;
				mov		al, [esi + 6];
				mov		ah, al;
				or eax, edx;
				mov[edi + 12], eax;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bs_d_loop2;
			}
			pScn += RENDER_WIDTH;
			pDst += pitch;
}
#endif
	}
	else {
		QWORD	maskn = 0x4040404040404040;
		QWORD	masks = 0x8080808080808080;
#if _WIN64
		for (INT i = 0; i < SCREEN_HEIGHT; i++) {
			PBYTE esi = pScn;
			PBYTE edi = pDst;
			DWORD ecx = width;
			__m128i mm7 = _mm_loadu_si64(&maskn);

			do {
				__m128i mm0 = _mm_loadu_si32(esi + 0);
				__m128i mm2 = _mm_loadu_si32(esi + 4);
				__m128i mm1 = mm0;
				__m128i mm3 = mm2;
				_mm_unpacklo_epi8(mm0, mm1);
				_mm_unpacklo_epi8(mm2, mm3);

				_mm_or_si128(mm0, mm7);
				_mm_or_si128(mm2, mm7);

				_mm_storeu_si64(edi + 0, mm0);
				_mm_storeu_si64(edi + 8, mm2);

				esi += 8;
				edi += 16;
				ecx -= 8;
			} while (ecx > 0);
			pDst += pitch;
			esi = pScn;
			edi = pDst;
			ecx = width;
			mm7 = _mm_loadu_si64(&masks);
			do {
				__m128i mm0 = _mm_loadu_si32(esi + 0);
				__m128i mm2 = _mm_loadu_si32(esi + 4);
				__m128i mm1 = mm0;
				__m128i mm3 = mm2;
				_mm_unpacklo_epi8(mm0, mm1);
				_mm_unpacklo_epi8(mm2, mm3);

				_mm_or_si128(mm0, mm7);
				_mm_or_si128(mm2, mm7);

				_mm_storeu_si64(edi + 0, mm0);
				_mm_storeu_si64(edi + 8, mm2);
				esi += 8;
				edi += 16;
				ecx -= 8;
			} while (ecx > 0);

			pScn += RENDER_WIDTH;
			pDst += pitch;

		}

#else
		for (INT i = 0; i < SCREEN_HEIGHT; i++) {
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				movq		mm7, maskn;
			_r8bs_d_loop3:
				movd		mm0, [esi + 0];
				movd		mm2, [esi + 4];
				movq		mm1, mm0;
				movq		mm3, mm2;
				punpcklbw	mm0, mm1;
				punpcklbw	mm2, mm3;
				por		mm0, mm7;
				por		mm2, mm7;
				movq[edi + 0], mm0;
				movq[edi + 8], mm2;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bs_d_loop3;
			}
			pDst += pitch;
			__asm {
				mov		esi, pScn;
				mov		edi, pDst;
				mov		ecx, width;
				movq		mm7, masks;
			_r8bs_d_loop4:
				movd		mm0, [esi + 0];
				movd		mm2, [esi + 4];
				movq		mm1, mm0;
				movq		mm3, mm2;
				punpcklbw	mm0, mm1;
				punpcklbw	mm2, mm3;
				por		mm0, mm7;
				por		mm2, mm7;
				movq[edi + 0], mm0;
				movq[edi + 8], mm2;

				lea		esi, [esi + 8];
				lea		edi, [edi + 16];
				sub		ecx, 8;
				jg		_r8bs_d_loop4;
			}
			pScn += RENDER_WIDTH;
			pDst += pitch;
		}
		_mm_empty();
#endif
	}
}
