#include "DebugOut.h"
#include "DirectDraw.h"
#include "COM.h"
#include "Render.h"

euI64	ONE = 0x0001000100010001;
euI64	cMask;
euI64	qMask;
euI64	lMask;
euI64	ACPixel;
euI64	Mask1;
euI64	Mask2;
euI64	I56Pixel;
euI64	I5556Pixel;
euI64	I5666Pixel;
euI64	I23Pixel;
euI64	I2223Pixel;
euI64	I2333Pixel;
euI64	Mask26;
euI64	Mask35;
euI64	Mask26b;
euI64	Mask35b;
euI64	product1a;
euI64	product1b;
euI64	product2a;
euI64	product2b;
euI64	final1a;
euI64	final1b;
euI64	final2a;
euI64	final2b;


//
// Render
//
void interp_set(unsigned bits_per_pixel);

void nx_2xSaILine_16bpp_mmx(euI8* pSrc, euI8* pDlt, euI32 srcPitch, euI32 width, euI8* pDst, euI32 dstPitch, euI32 bForceWrite);
void nx_2xSaILine_32bpp_mmx(euI8* pSrc, euI8* pDlt, euI32 srcPitch, euI32 width, euI8* pDst, euI32 dstPitch, euI32 bForceWrite);
void internal_scale2x_16_mmx(euI16* dst0, euI16* dst1, const euI16* src0, const euI16* src1, const euI16* src2, unsigned count);
void internal_scale2x_32_mmx(euI32* dst0, euI32* dst1, const euI32* src0, const euI32* src1, const euI32* src2, unsigned count);
void internal_scale2x_32_mmx_single(euI32* dst, const euI32* src0, const euI32* src1, const euI32* src2, unsigned count);

void hq2x_16_def(euI16* dst0, euI16* dst1, const euI16* src0, const euI16* src1, const euI16* src2, unsigned count);
void hq2x_32_def(euI32* dst0, euI32* dst1, const euI32* src0, const euI32* src1, const euI32* src2, unsigned count);
void lq2x_16_def(euI16* dst0, euI16* dst1, const euI16* src0, const euI16* src1, const euI16* src2, unsigned count);
void lq2x_32_def(euI32* dst0, euI32* dst1, const euI32* src0, const euI32* src1, const euI32* src2, unsigned count);

void nx_Super2xSaILine_16bpp_mmx(euI8* pSrc, euI8* pDlt, euI32 srcPitch, euI32 width, euI8* pDst, euI32 dstPitch, euI32 bForceWrite);
void nx_Super2xSaILine_32bpp_mmx(euI8* pSrc, euI8* pDlt, euI32 srcPitch, euI32 width, euI8* pDst, euI32 dstPitch, euI32 bForceWrite);
void nx_SuperEagleLine_16bpp_mmx(euI8* pSrc, euI8* pDlt, euI32 srcPitch, euI32 width, euI8* pDst, euI32 dstPitch, euI32 bForceWrite);
void nx_SuperEagleLine_32bpp_mmx(euI8* pSrc, euI8* pDlt, euI32 srcPitch, euI32 width, euI8* pDst, euI32 dstPitch, euI32 bForceWrite);


// 16bit Render
void	CDirectDraw::Render16bpp( LPBYTE lpSrc, LPBYTE lpDst )
{
	LPBYTE	pPal = 0;
	DWORD	width = 0;
	DWORD	pitch = SCREEN_WIDTH*sizeof(WORD);

	for( INT i = 0; i < SCREEN_HEIGHT; i++ ) {
		if( !(m_LineColormode[i]&0x80) ) {
			pPal = (LPBYTE)m_cnPalette[m_LineColormode[i]&0x07];
		} else {
			pPal = (LPBYTE)m_mnPalette[m_LineColormode[i]&0x07];
		}
		width = SCREEN_WIDTH;

#if _WIN64
		LPBYTE eax = lpSrc;
		LPBYTE esi = pPal;
		LPBYTE edi = lpDst;
		DWORD  edx = 0;
		DWORD  ecx = 0;
		do {
			edx = *(DWORD*)(eax + 0);
			*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));

			edx = *(DWORD*)(eax + 4);
			*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

			eax += 8;
			edi += 16;
			width -= 8;
		} while (width > 0);

		lpSrc += RENDER_WIDTH;
		lpDst += SCREEN_WIDTH * sizeof(DWORD);
#else
		__asm {
			mov		eax, lpSrc;
			mov		esi, pPal;
			mov		edi, lpDst;
_r16b_loop:
			mov		edx, [eax+0];
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 0], cx;
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 2], cx;
			movzx		ecx, dl;
			shr		edx, 8;
			mov		ecx, [esi+ecx*4];
			mov		edx, [esi+edx*4];
			mov		[edi+ 4], cx;
			mov		[edi+ 6], dx;

			mov		edx, [eax+4];
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 8], cx;
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+10], cx;
			movzx		ecx, dl;
			shr		edx, 8;
			mov		ecx, [esi+ecx*4];
			mov		edx, [esi+edx*4];
			mov		[edi+12], cx;
			mov		[edi+14], dx;

			lea		eax, [eax+ 8];
			lea		edi, [edi+16];

			sub		width, 8;
			jg		_r16b_loop;
		}
		lpSrc += RENDER_WIDTH;
		lpDst += SCREEN_WIDTH*sizeof(WORD);
#endif
	}
}

// 16bit Pre-Render
void	CDirectDraw::Render16bppPrefilter( LPBYTE lpSrc, LPBYTE lpDst )
{
	LPBYTE	pPal = 0;
	DWORD	width = 0;
	DWORD	pitch = SCREEN_WIDTH*sizeof(WORD);

	for( INT i = 0; i < SCREEN_HEIGHT; i++ ) {
		if( !(m_LineColormode[i]&0x80) ) {
			pPal = (LPBYTE)m_cfPalette[m_LineColormode[i]&0x07];
		} else {
			pPal = (LPBYTE)m_mfPalette[m_LineColormode[i]&0x07];
		}
		width = SCREEN_WIDTH;

#if _WIN64
		LPBYTE eax = lpSrc;
		LPBYTE esi = pPal;
		LPBYTE edi = lpDst;
		DWORD  edx = 0;
		DWORD  ecx = 0;
		do {
			edx = *(DWORD*)(eax + 0);
			*(WORD*)(edi + 0) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 2) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 4) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 6) = *(WORD*)(esi + 4 * (edx & 0xff));

			edx = *(DWORD*)(eax + 4);
			*(WORD*)(edi + 8) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 10) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 12) = *(WORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(WORD*)(edi + 14) = *(WORD*)(esi + 4 * (edx & 0xff));

			eax += 8;
			edi += 16;
			width -= 8;
		} while (width > 0);

		lpSrc += RENDER_WIDTH;
		lpDst += SCREEN_WIDTH * sizeof(DWORD);

#else
		//ASM_COMMENT_OUT

		__asm {
			mov		eax, lpSrc;
			mov		esi, pPal;
			mov		edi, lpDst;
_r16b_pf_loop:
			mov		edx, [eax+0];
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 0], cx;
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 2], cx;
			movzx		ecx, dl;
			shr		edx, 8;
			mov		ecx, [esi+ecx*4];
			mov		edx, [esi+edx*4];
			mov		[edi+ 4], cx;
			mov		[edi+ 6], dx;

			mov		edx, [eax+4];
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 8], cx;
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+10], cx;
			movzx		ecx, dl;
			shr		edx, 8;
			mov		ecx, [esi+ecx*4];
			mov		edx, [esi+edx*4];
			mov		[edi+12], cx;
			mov		[edi+14], dx;

			lea		eax, [eax+ 8];
			lea		edi, [edi+16];

			sub		width, 8;
			jg		_r16b_pf_loop;
		}
		lpSrc += RENDER_WIDTH;
		lpDst += SCREEN_WIDTH*sizeof(WORD);
#endif
	}
}

// 32bit Render
void	CDirectDraw::Render32bpp( LPBYTE lpSrc, LPBYTE lpDst )
{
	LPBYTE	pPal = 0;
	DWORD	width = 0;
	DWORD	pitch = SCREEN_WIDTH*sizeof(DWORD);

	for( INT i = 0; i < SCREEN_HEIGHT; i++ ) {
		if( !(m_LineColormode[i]&0x80) ) {
			pPal = (LPBYTE)m_cnPalette[m_LineColormode[i]&0x07];
		} else {
			pPal = (LPBYTE)m_mnPalette[m_LineColormode[i]&0x07];
		}
		width = SCREEN_WIDTH;

#if _WIN64
		LPBYTE eax = lpSrc;
		LPBYTE esi = pPal;
		LPBYTE edi = lpDst;
		DWORD  edx = 0;
		DWORD  ecx = 0;
		do {
			edx = *(DWORD*)(eax + 0);
			*(DWORD*)(edi + 0) = *(DWORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(DWORD*)(edi + 4) = *(DWORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(DWORD*)(edi + 8) = *(DWORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(DWORD*)(edi + 12) = *(DWORD*)(esi + 4 * (edx & 0xff));

			edx = *(DWORD*)(eax + 4);
			*(DWORD*)(edi + 16) = *(DWORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(DWORD*)(edi + 20) = *(DWORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(DWORD*)(edi + 24) = *(DWORD*)(esi + 4 * (edx & 0xff));
			edx >>= 8;
			*(DWORD*)(edi + 28) = *(DWORD*)(esi + 4 * (edx & 0xff));

			eax += 8;
			edi += 32;
			width -= 8;
		} while (width > 0);

		lpSrc += RENDER_WIDTH;
		lpDst += SCREEN_WIDTH * sizeof(DWORD);

#else
		__asm {
			mov		eax, lpSrc;
			mov		esi, pPal;
			mov		edi, lpDst;
_r32b_loop:
			mov		edx, [eax+0];
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 0], ecx;
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+ 4], ecx;
			movzx		ecx, dl;
			shr		edx, 8;
			mov		ecx, [esi+ecx*4];
			mov		edx, [esi+edx*4];
			mov		[edi+ 8], ecx;
			mov		[edi+12], edx;

			mov		edx, [eax+4];
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+16], ecx;
			movzx		ecx, dl;
			mov		ecx, [esi+ecx*4];
			shr		edx, 8;
			mov		[edi+20], ecx;
			movzx		ecx, dl;
			shr		edx, 8;
			mov		ecx, [esi+ecx*4];
			mov		edx, [esi+edx*4];
			mov		[edi+24], ecx;
			mov		[edi+28], edx;

			lea		eax, [eax+ 8];
			lea		edi, [edi+32];

			sub		width, 8;
			jg		_r32b_loop;
		}
		lpSrc += RENDER_WIDTH;
		lpDst += SCREEN_WIDTH*sizeof(DWORD);
#endif
	}
}

// Normal/Scanline/Double/DoubleScanlie Render
//#include "Render8bpp.h"
//#include "Render16bpp.h"
//#include "Render24bpp.h"
//#include "Render32bpp.h"
// Filtering Render

void	CDirectDraw::nx_2xSaI_16bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	LPBYTE	pScn = new BYTE[SCREEN_WIDTH*(SCREEN_HEIGHT+6)*sizeof(WORD)];

	// Pre-Rendering
	Render16bpp( lpRdr, &pScn[SCREEN_WIDTH*2*sizeof(WORD)] );

	euI8*	srcPtr   = (euI8*)&pScn[SCREEN_WIDTH*2*sizeof(WORD)];
	euI32	srcPitch = SCREEN_WIDTH*sizeof(WORD);
	euI8*	deltaPtr = (euI8*)lpDlt;
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = (euI32)ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	if( ddsd.ddpfPixelFormat.dwGBitMask == 0x01E0 ) {
	// 555
		cMask = 0x7BDE7BDE7BDE7BDE;
		qMask = 0x739C739C739C739C;
		lMask = 0x0C630C630C630C63;
	} else {
	// 565
		cMask = 0xF7DEF7DEF7DEF7DE;
		qMask = 0xE79CE79CE79CE79C;
		lMask = 0x1863186318631863;
	}

	for( ; height; height-- ) {
		nx_2xSaILine_16bpp_mmx( srcPtr, deltaPtr, srcPitch, width, dstPtr, dstPitch, bForceWrite );
		srcPtr += srcPitch;
		dstPtr += dstPitch * 2;
		deltaPtr += srcPitch;
	}

	delete[] pScn;
}

void	CDirectDraw::nx_2xSaI_32bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	LPBYTE	pScn = new BYTE[SCREEN_WIDTH*(SCREEN_HEIGHT+6)*sizeof(WORD)];

	// Pre-Rendering
	Render16bppPrefilter( lpRdr, &pScn[SCREEN_WIDTH*2*sizeof(WORD)] );

	euI8*	srcPtr   = (euI8*)&pScn[SCREEN_WIDTH*2*sizeof(WORD)];
	euI32	srcPitch = SCREEN_WIDTH*sizeof(WORD);
	euI8*	deltaPtr = (euI8*)lpDlt;
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = (euI32)ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	// 555
	cMask = 0x7BDE7BDE7BDE7BDE;
	qMask = 0x739C739C739C739C;
	lMask = 0x0C630C630C630C63;

	for( ; height; height-- ) {
		nx_2xSaILine_32bpp_mmx( srcPtr, deltaPtr, srcPitch, width, dstPtr, dstPitch, bForceWrite );
		srcPtr += srcPitch;
		dstPtr += dstPitch * 2;
		deltaPtr += srcPitch;
	}

	delete[] pScn;
}

void	CDirectDraw::nx_Super2xSaI_16bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	LPBYTE	pScn = new BYTE[SCREEN_WIDTH*(SCREEN_HEIGHT+6)*sizeof(WORD)];

	// Pre-Rendering
	Render16bpp( lpRdr, &pScn[SCREEN_WIDTH*2*sizeof(WORD)] );

	euI8*	srcPtr   = (euI8*)&pScn[SCREEN_WIDTH*2*sizeof(WORD)];
	euI32	srcPitch = SCREEN_WIDTH*sizeof(WORD);
	euI8*	deltaPtr = (euI8*)lpDlt;
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = (euI32)ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	if( ddsd.ddpfPixelFormat.dwGBitMask == 0x01E0 ) {
	// 555
		cMask = 0x7BDE7BDE7BDE7BDE;
		qMask = 0x739C739C739C739C;
		lMask = 0x0C630C630C630C63;
	} else {
	// 565
		cMask = 0xF7DEF7DEF7DEF7DE;
		qMask = 0xE79CE79CE79CE79C;
		lMask = 0x1863186318631863;
	}

	for( ; height; height-- ) {
		nx_Super2xSaILine_16bpp_mmx( srcPtr, deltaPtr, srcPitch, width, dstPtr, dstPitch, bForceWrite );
		srcPtr += srcPitch;
		dstPtr += dstPitch * 2;
		deltaPtr += srcPitch;
	}

	delete[] pScn;
}

void	CDirectDraw::nx_Super2xSaI_32bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	LPBYTE	pScn = new BYTE[SCREEN_WIDTH*(SCREEN_HEIGHT+6)*sizeof(WORD)];

	// Pre-Rendering
	Render16bppPrefilter( lpRdr, &pScn[SCREEN_WIDTH*2*sizeof(WORD)] );

	euI8*	srcPtr   = (euI8*)&pScn[SCREEN_WIDTH*2*sizeof(WORD)];
	euI32	srcPitch = SCREEN_WIDTH*sizeof(WORD);
	euI8*	deltaPtr = (euI8*)lpDlt;
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = (euI32)ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	// 555
	cMask = 0x7BDE7BDE7BDE7BDE;
	qMask = 0x739C739C739C739C;
	lMask = 0x0C630C630C630C63;

	for( ; height; height-- ) {
		nx_Super2xSaILine_32bpp_mmx( srcPtr, deltaPtr, srcPitch, width, dstPtr, dstPitch, bForceWrite );
		srcPtr += srcPitch;
		dstPtr += dstPitch * 2;
		deltaPtr += srcPitch;
	}

	delete[] pScn;
}

void	CDirectDraw::nx_SuperEagle_16bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	LPBYTE	pScn = new BYTE[SCREEN_WIDTH*(SCREEN_HEIGHT+6)*sizeof(WORD)];

	// Pre-Rendering
	Render16bpp( lpRdr, &pScn[SCREEN_WIDTH*2*sizeof(WORD)] );

	euI8*	srcPtr   = (euI8*)&pScn[SCREEN_WIDTH*2*sizeof(WORD)];
	euI32	srcPitch = SCREEN_WIDTH*sizeof(WORD);
	euI8*	deltaPtr = (euI8*)lpDlt;
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = (euI32)ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	if( ddsd.ddpfPixelFormat.dwGBitMask == 0x01E0 ) {
	// 555
		cMask = 0x7BDE7BDE7BDE7BDE;
		qMask = 0x739C739C739C739C;
		lMask = 0x0C630C630C630C63;
	} else {
	// 565
		cMask = 0xF7DEF7DEF7DEF7DE;
		qMask = 0xE79CE79CE79CE79C;
		lMask = 0x1863186318631863;
	}

	for( ; height; height-- ) {
		nx_SuperEagleLine_16bpp_mmx( srcPtr, deltaPtr, srcPitch, width, dstPtr, dstPitch, bForceWrite );
		srcPtr += srcPitch;
		dstPtr += dstPitch * 2;
		deltaPtr += srcPitch;
	}

	delete[] pScn;
}

void	CDirectDraw::nx_SuperEagle_32bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	LPBYTE	pScn = new BYTE[SCREEN_WIDTH*(SCREEN_HEIGHT+6)*sizeof(WORD)];

	// Pre-Rendering
	Render16bppPrefilter( lpRdr, &pScn[SCREEN_WIDTH*2*sizeof(WORD)] );

	euI8*	srcPtr   = (euI8*)&pScn[SCREEN_WIDTH*2*sizeof(WORD)];
	euI32	srcPitch = SCREEN_WIDTH*sizeof(WORD);
	euI8*	deltaPtr = (euI8*)lpDlt;
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = (euI32)ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	// 555
	cMask = 0x7BDE7BDE7BDE7BDE;
	qMask = 0x739C739C739C739C;
	lMask = 0x0C630C630C630C63;

	for( ; height; height-- ) {
		nx_SuperEagleLine_32bpp_mmx( srcPtr, deltaPtr, srcPitch, width, dstPtr, dstPitch, bForceWrite );
		srcPtr += srcPitch;
		dstPtr += dstPitch * 2;
		deltaPtr += srcPitch;
	}

	delete[] pScn;
}

void	CDirectDraw::nx_Scale2x_16bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	// Pre-Rendering
	Render16bpp( lpRdr, lpDlt );

	euI8*	srcPtr   = (euI8*)lpDlt;
	euI32	srcPitch = SCREEN_WIDTH * sizeof(euI16);
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	euI16	*dst0 = (euI16*)dstPtr;
	euI16	*dst1 = dst0 + (dstPitch/2);
	euI16	*src0 = (euI16*)lpDlt;
	euI16	*src1 = src0 + (srcPitch/2);
	euI16	*src2 = src1 + (srcPitch/2);

	internal_scale2x_16_mmx( dst0, dst1, src0, src0, src1, width );

	int count = height;
	count -= 2;
	while( count ) {
		dst0 += dstPitch;
		dst1 += dstPitch;
		internal_scale2x_16_mmx( dst0, dst1, src0, src1, src2, width );
		src0 = src1;
		src1 = src2;
		src2 += srcPitch/2;
		--count;
	}
	dst0 += dstPitch;
	dst1 += dstPitch;
	internal_scale2x_16_mmx( dst0, dst1, src0, src1, src1, width );
}

void	CDirectDraw::nx_Scale2x_32bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	// Pre-Rendering
	Render32bpp( lpRdr, lpDlt );

	euI8*	srcPtr   = (euI8*)lpDlt;
	euI32	srcPitch = SCREEN_WIDTH * sizeof(euI32);
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	euI32	*dst0 = (euI32*)dstPtr;
	euI32	*dst1 = dst0 + (dstPitch/4);
	euI32	*src0 = (euI32*)lpDlt;
	euI32	*src1 = src0 + (srcPitch/4);
	euI32	*src2 = src1 + (srcPitch/4);

	internal_scale2x_32_mmx( dst0, dst1, src0, src0, src1, width );

	int count = height;
	count -= 2;
	while( count ) {
		dst0 += dstPitch/2;
		dst1 += dstPitch/2;
		internal_scale2x_32_mmx( dst0, dst1, src0, src1, src2, width );
		src0 = src1;
		src1 = src2;
		src2 += srcPitch/4;
		--count;
	}
	dst0 += dstPitch/2;
	dst1 += dstPitch/2;
	internal_scale2x_32_mmx( dst0, dst1, src0, src1, src1, width );
}

void	CDirectDraw::nx_hq2x_16bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	// Pre-Rendering
	Render16bpp( lpRdr, lpDlt );

	euI8*	srcPtr   = (euI8*)lpDlt;
	euI32	srcPitch = SCREEN_WIDTH * sizeof(euI16);
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	euI16	*dst0 = (euI16*)dstPtr;
	euI16	*dst1 = dst0 + (dstPitch/2);
	euI16	*src0 = (euI16*)lpDlt;
	euI16	*src1 = src0 + (srcPitch/2);
	euI16	*src2 = src1 + (srcPitch/2);

	if( ddsd.ddpfPixelFormat.dwGBitMask == 0x01E0 ) {
		interp_set( 15 );
	} else {
		interp_set( 16 );
	}

	hq2x_16_def( dst0, dst1, src0, src0, src1, width );

	int count = height;
	count -= 2;
	while( count ) {
		dst0 += dstPitch;
		dst1 += dstPitch;
		hq2x_16_def( dst0, dst1, src0, src1, src2, width );
		src0 = src1;
		src1 = src2;
		src2 += srcPitch/2;
		--count;
	}
	dst0 += dstPitch;
	dst1 += dstPitch;
	hq2x_16_def( dst0, dst1, src0, src1, src1, width );
}

void	CDirectDraw::nx_hq2x_32bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	// Pre-Rendering
	Render32bpp( lpRdr, lpDlt );

	euI8*	srcPtr   = (euI8*)lpDlt;
	euI32	srcPitch = SCREEN_WIDTH * sizeof(euI32);
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	euI32	*dst0 = (euI32*)dstPtr;
	euI32	*dst1 = dst0 + (dstPitch/4);
	euI32	*src0 = (euI32*)lpDlt;
	euI32	*src1 = src0 + (srcPitch/4);
	euI32	*src2 = src1 + (srcPitch/4);

	interp_set( 32 );

	hq2x_32_def( dst0, dst1, src0, src0, src1, width );

	int count = height;
	count -= 2;
	while( count ) {
		dst0 += dstPitch/2;
		dst1 += dstPitch/2;
		hq2x_32_def( dst0, dst1, src0, src1, src2, width );
		src0 = src1;
		src1 = src2;
		src2 += srcPitch/4;
		--count;
	}
	dst0 += dstPitch/2;
	dst1 += dstPitch/2;
	hq2x_32_def( dst0, dst1, src0, src1, src1, width );
}

void	CDirectDraw::nx_lq2x_16bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	// Pre-Rendering
	Render16bpp( lpRdr, lpDlt );

	euI8*	srcPtr   = (euI8*)lpDlt;
	euI32	srcPitch = SCREEN_WIDTH * sizeof(euI16);
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	euI16	*dst0 = (euI16*)dstPtr;
	euI16	*dst1 = dst0 + (dstPitch/2);
	euI16	*src0 = (euI16*)lpDlt;
	euI16	*src1 = src0 + (srcPitch/2);
	euI16	*src2 = src1 + (srcPitch/2);

	if( ddsd.ddpfPixelFormat.dwGBitMask == 0x01E0 ) {
		interp_set( 15 );
	} else {
		interp_set( 16 );
	}

	lq2x_16_def( dst0, dst1, src0, src0, src1, width );

	int count = height;
	count -= 2;
	while( count ) {
		dst0 += dstPitch;
		dst1 += dstPitch;
		lq2x_16_def( dst0, dst1, src0, src1, src2, width );
		src0 = src1;
		src1 = src2;
		src2 += srcPitch/2;
		--count;
	}
	dst0 += dstPitch;
	dst1 += dstPitch;
	lq2x_16_def( dst0, dst1, src0, src1, src1, width );
}

void	CDirectDraw::nx_lq2x_32bpp( LPBYTE lpRdr, LPBYTE lpDlt, DDSURFACEDESC2& ddsd, BOOL bForceWrite )
{
	// Pre-Rendering
	Render32bpp( lpRdr, lpDlt );

	euI8*	srcPtr   = (euI8*)lpDlt;
	euI32	srcPitch = SCREEN_WIDTH * sizeof(euI32);
	euI8*	dstPtr   = (euI8*)ddsd.lpSurface;
	euI32	dstPitch = ddsd.lPitch;
	int	width    = SCREEN_WIDTH;
	int	height   = SCREEN_HEIGHT;

	euI32	*dst0 = (euI32*)dstPtr;
	euI32	*dst1 = dst0 + (dstPitch/4);
	euI32	*src0 = (euI32*)lpDlt;
	euI32	*src1 = src0 + (srcPitch/4);
	euI32	*src2 = src1 + (srcPitch/4);

	interp_set( 32 );

	lq2x_32_def( dst0, dst1, src0, src0, src1, width );

	int count = height;
	count -= 2;
	while( count ) {
		dst0 += dstPitch/2;
		dst1 += dstPitch/2;
		lq2x_32_def( dst0, dst1, src0, src1, src2, width );
		src0 = src1;
		src1 = src2;
		src2 += srcPitch/4;
		--count;
	}
	dst0 += dstPitch/2;
	dst1 += dstPitch/2;
	lq2x_32_def( dst0, dst1, src0, src1, src1, width );
}
