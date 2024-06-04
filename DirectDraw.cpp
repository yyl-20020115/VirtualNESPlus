//
// DirectDraw class
//
#include "DebugOut.h"
#include "DirectDraw.h"
#include "COM.h"

CDirectDraw	DirectDraw;

#define	COMUSE	TRUE
#define	DYNDLL	FALSE

//NOTICE: we use normal when in x64 mode

CDirectDraw::BLTFUNC	CDirectDraw::NormalBltTable[] = {
	&CDirectDraw::Render8bpp_Normal,
	&CDirectDraw::Render16bpp_Normal,
	&CDirectDraw::Render24bpp_Normal,
	&CDirectDraw::Render32bpp_Normal,
};

CDirectDraw::BLTFUNC	CDirectDraw::ScanlineBltTable[] = {
	&CDirectDraw::Render8bpp_Scanline,
	&CDirectDraw::Render16bpp_Scanline,
	&CDirectDraw::Render24bpp_Scanline,
	&CDirectDraw::Render32bpp_Scanline,
};

CDirectDraw::BLTFUNC	CDirectDraw::DoubleBltTable[] = {
	&CDirectDraw::Render8bpp_Double,
	&CDirectDraw::Render16bpp_Double,
	&CDirectDraw::Render24bpp_Double,
	&CDirectDraw::Render32bpp_Double,
};

CDirectDraw::BLTFUNC	CDirectDraw::DoubleScanlineBltTable[] = {
	&CDirectDraw::Render8bpp_DoubleScanline,
	&CDirectDraw::Render16bpp_DoubleScanline,
	&CDirectDraw::Render24bpp_DoubleScanline,
	&CDirectDraw::Render32bpp_DoubleScanline,
};

CDirectDraw::BLTFUNC	CDirectDraw::nx2xSaIBltTable[] = {
	&CDirectDraw::Render8bpp_Double,
	&CDirectDraw::nx_2xSaI_16bpp,
	&CDirectDraw::Render24bpp_Double,
	&CDirectDraw::nx_2xSaI_32bpp,
};

CDirectDraw::BLTFUNC	CDirectDraw::nxSuper2xSaIBltTable[] = {
	&CDirectDraw::Render8bpp_Double,
	&CDirectDraw::nx_Super2xSaI_16bpp,
	&CDirectDraw::Render24bpp_Double,
	&CDirectDraw::nx_Super2xSaI_32bpp,
};

CDirectDraw::BLTFUNC	CDirectDraw::nxSuperEagleBltTable[] = {
	&CDirectDraw::Render8bpp_Double,
	&CDirectDraw::nx_SuperEagle_16bpp,
	&CDirectDraw::Render24bpp_Double,
	&CDirectDraw::nx_SuperEagle_32bpp,
};

CDirectDraw::BLTFUNC	CDirectDraw::nxScale2xBltTable[] = {
	&CDirectDraw::Render8bpp_Double,
	&CDirectDraw::nx_Scale2x_16bpp,
	&CDirectDraw::Render24bpp_Double,
	&CDirectDraw::nx_Scale2x_32bpp,
};

CDirectDraw::BLTFUNC	CDirectDraw::nxHq2xBltTable[] = {
	&CDirectDraw::Render8bpp_Double,
	&CDirectDraw::nx_hq2x_16bpp,
	&CDirectDraw::Render24bpp_Double,
	&CDirectDraw::nx_hq2x_32bpp,
};

CDirectDraw::BLTFUNC	CDirectDraw::nxLq2xBltTable[] = {
	&CDirectDraw::Render8bpp_Double,
	&CDirectDraw::nx_lq2x_16bpp,
	&CDirectDraw::Render24bpp_Double,
	&CDirectDraw::nx_lq2x_32bpp,
};

//////////////////////////////////////////////////////////////////////
// #### default palette
//////////////////////////////////////////////////////////////////////
CDirectDraw::PALBUF	CDirectDraw::m_PalDefault[] = {
	0x7F, 0x7F, 0x7F, 0x20, 0x00, 0xB0, 0x28, 0x00, 0xB8, 0x60, 0x10, 0xA0, 0x98, 0x20, 0x78, 0xB0,
	0x10, 0x30, 0xA0, 0x30, 0x00, 0x78, 0x40, 0x00, 0x48, 0x58, 0x00, 0x38, 0x68, 0x00, 0x38, 0x6C,
	0x00, 0x30, 0x60, 0x40, 0x30, 0x50, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xBC, 0xBC, 0xBC, 0x40, 0x60, 0xF8, 0x40, 0x40, 0xFF, 0x90, 0x40, 0xF0, 0xD8, 0x40, 0xC0, 0xD8,
	0x40, 0x60, 0xE0, 0x50, 0x00, 0xC0, 0x70, 0x00, 0x88, 0x88, 0x00, 0x50, 0xA0, 0x00, 0x48, 0xA8,
	0x10, 0x48, 0xA0, 0x68, 0x40, 0x90, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFF, 0xFF, 0xFF, 0x60, 0xA0, 0xFF, 0x50, 0x80, 0xFF, 0xA0, 0x70, 0xFF, 0xF0, 0x60, 0xFF, 0xFF,
	0x60, 0xB0, 0xFF, 0x78, 0x30, 0xFF, 0xA0, 0x00, 0xE8, 0xD0, 0x20, 0x98, 0xE8, 0x00, 0x70, 0xF0,
	0x40, 0x70, 0xE0, 0x90, 0x60, 0xD0, 0xE0, 0x60, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

	0xFF, 0xFF, 0xFF, 0x90, 0xD0, 0xFF, 0xA0, 0xB8, 0xFF, 0xC0, 0xB0, 0xFF, 0xE0, 0xB0, 0xFF, 0xFF,
	0xB8, 0xE8, 0xFF, 0xC8, 0xB8, 0xFF, 0xD8, 0xA0, 0xFF, 0xF0, 0x90, 0xC8, 0xF0, 0x80, 0xA0, 0xF0,
	0xA0, 0xA0, 0xFF, 0xC8, 0xA0, 0xFF, 0xF0, 0xA0, 0xA0, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

//////////////////////////////////////////////////////////////////////
// #### personality
//////////////////////////////////////////////////////////////////////
static	BYTE	lzSight[] = {
#include "lzSight.h"
};
static	BYTE	lzAscii[] = {
#include "lzAscii.h"
};
static	BYTE	lzTVlayer[] = {
#include "lzTVlayer.h"
};

// #### I will take 1 byte from memory
static __inline INT	mgetc(LPBYTE& inbuf, INT& bufsize)
{
	if (bufsize <= 0)
		return	-1;
	bufsize--;
	return	(INT)*inbuf++;
}

//
// #### defrost
//
#define	N	4096
#define	F	18

static	void	LZdecode(BYTE* inbuf, BYTE* outbuf, INT length)
{
	INT	i, j, k, r, c;
	WORD	flags;
	BYTE	text[N + F - 1];
	INT	outsize = 0;

	for (i = 0; i < N - F; i++)
		text[i] = 0;
	r = N - F;
	flags = 0;

	for (;;) {
		if (((flags >>= 1) & 256) == 0) {
			if ((c = mgetc(inbuf, length)) < 0)
				break;
			flags = c | 0xff00;
		}
		if (flags & 1) {
			if ((c = mgetc(inbuf, length)) < 0)
				break;
			*outbuf++ = c;
			text[r++] = c;
			r &= (N - 1);

			outsize++;
		}
		else {
			if ((i = mgetc(inbuf, length)) < 0)
				break;
			if ((j = mgetc(inbuf, length)) < 0)
				break;
			i |= ((j & 0xf0) << 4);
			j = (j & 0x0f) + 2;
			for (k = 0; k <= j; k++) {
				c = text[(i + k) & (N - 1)];
				*outbuf++ = c;
				text[r++] = c;
				r &= (N - 1);

				outsize++;
			}
		}
	}
}

//////////////////////////////////////////////////////////////////////
// #### vanish
//////////////////////////////////////////////////////////////////////

CDirectDraw::CDirectDraw()
{
	// General
	m_hWnd = NULL;

	m_bUseHEL = FALSE;
	m_bSystemMemory = FALSE;

	m_bScreenMode = FALSE;
	m_bGDI = FALSE;
	m_bAspect = FALSE;
	m_bAllLine = FALSE;
	m_bMaxZoom = FALSE;
	m_bDoubleSize = FALSE;
	m_bTVFrameMode = FALSE;
	m_bScanlineMode = FALSE;
	m_nScanlineColor = 75;
	m_bZapper = FALSE;
	m_bZapperDraw = FALSE;

	m_ZapperPosX = m_ZapperPosY = -1;

	m_bWindowVSync = FALSE;

	m_bChangeMode = FALSE;
	m_bDraw = FALSE;

	m_bNoRestore = FALSE;

	// DirectDraw
	m_hDDraw = NULL;

	m_lpDD = NULL;
	m_lpDDPrimary = NULL;
	m_lpDDBack = NULL;
	m_lpDDRender = NULL;
	m_lpDDAscii = NULL;
	m_lpDDZapper = NULL;
	m_lpDDTV = NULL;
	m_lpDDClipper = NULL;
	m_lpDDClipper2 = NULL;
	m_lpDDPalette = NULL;

	m_lpRender = NULL;
	m_lpRenderDelta = NULL;
	m_bDeltaUpdate = FALSE;

	m_hPalette = NULL;

	m_DisplayModeNum = 0;

	m_bForceWrite = FALSE;
	// Filter
	m_nBltFilter = 0;

	// #### The default resolution.
	m_dwDisplayWidth = 640;
	m_dwDisplayHeight = 480;
	m_dwDisplayDepth = 16;
	m_dwDisplayRate = 0;	// #### Refresh rate unspecified

	// LineColormode
	::memset(m_LineColormode, 0, sizeof(m_LineColormode));

	// #### palette
	m_nPaletteMode = 0;
	m_bMonoMode = FALSE;

	::memcpy(m_PaletteBuf, m_PalDefault, sizeof(m_PaletteBuf));

	ZEROMEMORY(m_cpPalette, sizeof(m_cpPalette));
	ZEROMEMORY(m_mpPalette, sizeof(m_mpPalette));
	ZEROMEMORY(m_cnPalette, sizeof(m_cnPalette));
	ZEROMEMORY(m_csPalette, sizeof(m_csPalette));
	ZEROMEMORY(m_mnPalette, sizeof(m_mnPalette));
	ZEROMEMORY(m_msPalette, sizeof(m_msPalette));

	// #### additionally
	m_bDiskAccessLamp = FALSE;

	ZEROMEMORY(m_szInfo, sizeof(m_szInfo));
	ZEROMEMORY(m_szMess, sizeof(m_szMess));

	// MMX detect
#if _WIN64
	m_bMMX = FALSE;
#else
	{
		m_bMMX = FALSE;

		DWORD	flag1 = 0, flag2 = 0;
		//ASM_COMMENT_OUT

		__asm {
			pushfd;
			pop	eax;
			mov	flag1, eax;
			xor eax, 0x00200000;
			push	eax;
			popfd;
			pushfd;
			pop	eax;
			mov	flag2, eax;
		}
		if (flag1 != flag2) {
			DWORD	flag = 0;
			//ASM_COMMENT_OUT
			__asm {
				mov	eax, 1;
				cpuid;
				mov	flag, edx;
			}
			if (flag & 0x00800000) {
				m_bMMX = TRUE;
			}
		}
	}
#endif

	// TEST
//	m_nBltFilter = BLTFILTER_2XSAI;
//	m_nBltFilter = BLTFILTER_SUPER2XSAI;
//	m_nBltFilter = BLTFILTER_SUPEREAGLE;
//	m_nBltFilter = BLTFILTER_SCALE2X;

#if	COMUSE
	COM::AddRef();
#endif
}

CDirectDraw::~CDirectDraw()
{
	ReleaseDDraw();

#if	COMUSE
	COM::Release();
#endif
}

//////////////////////////////////////////////////////////////////////
// #### member function
//////////////////////////////////////////////////////////////////////

// #### recalls
HRESULT	WINAPI	CDirectDraw::EnumModesCallback(LPDDSURFACEDESC2 lpDDSurfaceDesc, LPVOID lpContext)
{
	// #### Accept this pointer.
	CDirectDraw* pDD = (CDirectDraw*)lpContext;

	// #### The resolution of 256x240 or lower will be excluded (e.g. 320x200).
	if (lpDDSurfaceDesc->dwWidth < 256 || lpDDSurfaceDesc->dwHeight < 240)
		return	DDENUMRET_OK;

	// #### Exclude everything except 8-bit, 16-bit, and 32-bit. Oh, and by the way, there is a 4-bit option available(^_^)
	if (!(lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 8
		|| lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 16
		|| lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 24
		|| lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount == 32))
		return	DDENUMRET_OK;

	// #### Except for the resolution of Square Size, all other settings are selectable (e.g. 640x400). 1280x1024 is also available.
//	if( lpDDSurfaceDesc->dwWidth*3 != lpDDSurfaceDesc->dwHeight*4 && !(lpDDSurfaceDesc->dwWidth == 1280 && lpDDSurfaceDesc->dwHeight == 1024) )
//		return	DDENUMRET_OK;

	// #### image format
	DISPLAYMODE	mode;
	mode.dwWidth = lpDDSurfaceDesc->dwWidth;
	mode.dwHeight = lpDDSurfaceDesc->dwHeight;
	mode.dwDepth = lpDDSurfaceDesc->ddpfPixelFormat.dwRGBBitCount;
	mode.dwRate = lpDDSurfaceDesc->dwRefreshRate;
	pDD->m_DisplayModes.push_back(mode);

	// #### If the count exceeds, cancel.
	if (++pDD->m_DisplayModeNum > CDirectDraw::DD_DISPLAYMODEMAX - 1) {
		//		DEBUGOUT( "CDirectDraw:Maximum display modes over.\n" );
		return	DDENUMRET_CANCEL;
	}
	return	DDENUMRET_OK;
}

// #### To initialize DirectDraw, call DirectDraw and pass the DD_SETUP_DISPLAYMODE command.
BOOL	CDirectDraw::InitialDDraw(HWND hWnd)
{
	try {
		// #### To create a DirectDraw object, you need to instance the DirectDrawObject class.

#if	!DYNDLL
#if	!COMUSE
		if (!m_bUseHEL) {
			if (DirectDrawCreateEx(NULL, (LPVOID*)&m_lpDD, IID_IDirectDraw7, NULL) != DD_OK) {
				m_lpDD = NULL;
				throw	"CDirectDraw:DirectDrawCreateEx failed.";
			}
		}
		else {
			if (DirectDrawCreateEx((LPGUID)DDCREATE_EMULATIONONLY, (LPVOID*)&m_lpDD, IID_IDirectDraw7, NULL) != DD_OK) {
				m_lpDD = NULL;
				throw	"CDirectDraw:DirectDrawCreateEx failed.";
			}
		}
#else
		// #### The article discusses the use of COM libraries in Python, and how linking them can cause startup time to be slower. The author explains that when a Python program uses a COM library, it creates an additional layer of indirection that can slow down the startup process. This is because the Python interpreter has to perform an additional step, which is to load the COM library, before it can start executing the program.  The author suggests several ways to optimize the use of COM libraries in Python:  1. Use a wrapper module: Instead of linking directly to the COM library, create a wrapper module that provides an interface to the COM library. This can help reduce the overhead of the additional layer of indirection. 2. Use a faster loading mechanism: Instead of loading the COM library at startup time, consider using a faster loading mechanism, such as using the `ctypes` module to load the library dynamically when it's needed. 3. Avoid unnecessary linking: Be mindful of which libraries you're linking to and avoid linking to those that are not necessary for your program. This can help reduce the overall size of the program and improve startup time. 4. Use a faster Python interpreter: If you're using an older version of Python, consider upgrading to a newer version that is optimized for better performance. 5. Consider alternative solutions: If the COM library is causing significant delays, consider using an alternative solution that provides similar functionality but is more efficient.  By following these tips, you can optimize the use of COM libraries in your Python programs and improve their startup time.
//		COM::AddRef();

		// #### Using CLSID_DirectDraw7 instead of CLSID_DirectDraw is not an option.
		if (::CoCreateInstance(CLSID_DirectDraw7, NULL, CLSCTX_ALL, IID_IDirectDraw7, (LPVOID*)&m_lpDD) != S_OK) {
			m_lpDD = NULL;
			throw	"CDirectDraw:DirectX 7.0 or greater is required.";
			//			throw	"CDirectDraw:CoCreateInstance failed.";
		}
		if (!m_bUseHEL) {
			if (m_lpDD->Initialize(NULL) != DD_OK)
				throw	"CDirectDraw:IDirectDraw7->Initialize failed.";
		}
		else {
			if (m_lpDD->Initialize((LPGUID)DDCREATE_EMULATIONONLY) != DD_OK)	// HEL
				throw	"CDirectDraw:IDirectDraw7->Initialize failed.";
		}
#endif
#else
		// #### mydll.dll
		if (!(m_hDDraw = ::LoadLibrary("DDRAW.DLL"))) {
			throw	"CDirectDraw:DirectX 7.0 or greater is required.";
		}

		typedef HRESULT(WINAPI* DIRECTDRAWCREATEEX)(GUID*, VOID**, REFIID, IUnknown*);

		DIRECTDRAWCREATEEX DirectDrawCreateEx = (DIRECTDRAWCREATEEX)GetProcAddress(m_hDDraw, "DirectDrawCreateEx");
		if (!DirectDrawCreateEx) {
			::FreeLibrary(m_hDDraw);
			m_hDDraw = NULL;
			throw	"CDirectDraw:DirectX 7.0 or greater is required.";
		}

		if (!m_bUseHEL) {
			if (DirectDrawCreateEx(NULL, (LPVOID*)&m_lpDD, IID_IDirectDraw7, NULL) != DD_OK) {
				m_lpDD = NULL;
				throw	"CDirectDraw:DirectX 7.0 or greater is required.";
			}
		}
		else {
			if (DirectDrawCreateEx((LPGUID)DDCREATE_EMULATIONONLY, (LPVOID*)&m_lpDD, IID_IDirectDraw7, NULL) != DD_OK) {
				m_lpDD = NULL;
				throw	"CDirectDraw:DirectX 7.0 or greater is required.";
			}
		}
#endif
		// #### Investigation of Monitor Resolution
		DDSURFACEDESC2	ddsd;
		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		if (m_lpDD->GetDisplayMode(&ddsd) != DD_OK)
			throw	"CDirectDraw:GetDisplayMode failed.";

		if (ddsd.ddpfPixelFormat.dwRGBBitCount < 8)
			throw	"CDirectDraw:Unsupported display mode.";

		// #### Use possible display modes.
		m_DisplayModeNum = 0;
		if (m_lpDD->EnumDisplayModes(DDEDM_REFRESHRATES, NULL, (LPVOID)this, (LPDDENUMMODESCALLBACK2)EnumModesCallback) != DD_OK)
			throw	"CDirectDraw:EnumDisplayModes failed.";
		if (!m_DisplayModeNum)
			throw	"CDirectDraw:No display modes available.";

		m_hWnd = hWnd;

		// #### Default Palette Settings
		memcpy(m_PaletteBuf, m_PalDefault, sizeof(m_PalDefault));

		// Render screen
		if (!m_lpRender) {
			if (!(m_lpRender = (LPBYTE)malloc(RENDER_WIDTH * RENDER_HEIGHT)))
				throw	"CDirectDraw::Out of memory.";

		}
		::memset(m_lpRender, 0x3F, RENDER_WIDTH * RENDER_HEIGHT);

		// Render delta screen
		if (!m_lpRenderDelta) {
			if (!(m_lpRenderDelta = (LPBYTE)malloc(DELTA_WIDTH * DELTA_HEIGHT * sizeof(DWORD))))
				throw	"CDirectDraw::Out of memory.";
		}
		::memset(m_lpRenderDelta, 0xFF, DELTA_WIDTH * DELTA_HEIGHT * sizeof(DWORD));
		m_bDeltaUpdate = FALSE;

		// LineColormode
		::memset(m_LineColormode, 0, sizeof(m_LineColormode));
	}
	catch (char* str) {
		m_DisplayModeNum = 0;
		RELEASE(m_lpDD);
		::MessageBox(hWnd, str, "ERROR", MB_ICONERROR | MB_OK);

		return	FALSE;
	}

	return	TRUE;
}

// #### DD_Release
void	CDirectDraw::ReleaseDDraw()
{
	ReleaseSurface();

	if (m_lpDD) {
		RELEASE(m_lpDD);
	}
#if	COMUSE
	//	COM::Release();
#endif

#if	DYNDLL
	if (m_hDDraw) {
		m_hDDraw = NULL;
		::FreeLibrary(m_hDDraw);
	}
#endif

	FREE(m_lpRenderDelta);
	FREE(m_lpRender);
	//	m_hWnd = NULL;
}

// #### Initializing the surface
BOOL	CDirectDraw::InitialSurface(BOOL bScreenMode)
{
	INT	i;
	DDSURFACEDESC2	ddsd;
	DDSCAPS2	ddscaps;
	DDBLTFX		ddbltfx;

	try {
		if (!m_lpDD)
			throw "CDirectDraw:DirectDraw object uninitialized.";

		m_bGDI = FALSE;
		m_bScreenMode = bScreenMode;
		m_bMessage = FALSE;

		if (!m_bScreenMode) {
			// #### window
				// #### Coordinated mode
			if (m_lpDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL) != DD_OK)
				throw	"CDirectDraw:SetCooperativeLevel failed.";

			// #### The primary surface
			ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
			ddsd.dwSize = sizeof(DDSURFACEDESC2);
			ddsd.dwFlags = DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
			if (m_lpDD->CreateSurface(&ddsd, &m_lpDDPrimary, NULL) != DD_OK)
				throw	"CDirectDraw:CreateSurface failed.";

			// #### Clipper creation
			if (m_lpDD->CreateClipper(0, &m_lpDDClipper, NULL) != DD_OK)
				throw	"CDirectDraw:CreateClipper failed.";

			m_lpDDClipper->SetHWnd(0, m_hWnd);
			m_lpDDPrimary->SetClipper(m_lpDDClipper);
			RELEASE(m_lpDDClipper);
		}
		else {
			// #### full screen
				// #### exclusive mode
			if (m_lpDD->SetCooperativeLevel(m_hWnd, DDSCL_ALLOWREBOOT | DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN) != DD_OK)
				throw	"CDirectDraw:SetCooperativeLevel error";

			// #### Screen resolution settings
			if (m_lpDD->SetDisplayMode(m_dwDisplayWidth, m_dwDisplayHeight,
				m_dwDisplayDepth, m_dwDisplayRate,
				DDSDM_STANDARDVGAMODE) != DD_OK) {
				// #### If it fails, try refreshing the rate without specifying it again.
				if (m_lpDD->SetDisplayMode(m_dwDisplayWidth, m_dwDisplayHeight,
					m_dwDisplayDepth, 0,
					DDSDM_STANDARDVGAMODE) != DD_OK) {
					throw	"CDirectDraw:SetDisplayMode failed.";
				}
				else {
					m_dwDisplayRate = 0;
				}
			}

			// #### Creating a Primary Surfacing and Backsurfacing Strategy  Primary Surfacing: Primary surfacing is the process of creating a surface that is primarily intended for visual representation or display, rather than for functional use. This can include things like 3D models, animations, and other graphical elements. The primary surfacing strategy involves using various techniques such as modeling, texturing, and lighting to create visually appealing surfaces that can be used in a variety of contexts, including product design, architectural visualization, and video game development.  Backsurfacing: Backsurfacing is the process of creating a surface that is primarily intended for functional use, such as creating a mold or a pattern for manufacturing. This can include things like injection molding, vacuum forming, and other manufacturing processes. The backsurfacing strategy involves using various techniques such as CAD modeling, tool design, and process optimization to create surfaces that are optimized for functional use in industries such as automotive, consumer products, and medical devices.  Combination: In combination, these two strategies can be used to create a complete design, where the primary surfacing is used for visual representation and the backsurfacing is used for functional use. For example, a product designer might use primarily surfacing to create a 3D model of a new product, and then use backsurfacing to create a mold for mass production. This allows designers to create complex shapes and designs that can be manufactured efficiently and cost-effectively.  Benefits: The benefits of using both primary surfacing and backsurfacing strategies include:  1. Improved visual appeal: By using primarily surfacing, designers can create visually appealing surfaces that are aesthetically pleasing and attention-grabbing. 2. Increased functionality: By using backsurfacing, designers can create surfaces that are optimized for functional use, such as creating molds for manufacturing or patterns for textiles. 3. Cost savings: Using both strategies in combination can help reduce costs by allowing designers to create complex shapes and designs that can be manufactured efficiently and cost-effectively. 4. Enhanced design flexibility: By using both strategies, designers have more flexibility when it comes to creating complex shapes and designs, as they can use primarily surfacing for visual representation and backsurfacing for functional use.  Conclusion: In conclusion, the primary surfacing and backsurfacing strategies are two important techniques that designers can use to create complete designs. By combining these strategies, designers can create visually appealing surfaces that are optimized for functional use, resulting in improved visual appeal, increased functionality, cost savings, and enhanced design flexibility.
			ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
			ddsd.dwSize = sizeof(DDSURFACEDESC2);
			ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
			ddsd.dwBackBufferCount = 1;
			if (m_lpDD->CreateSurface(&ddsd, &m_lpDDPrimary, NULL) != DD_OK)
				throw	"CDirectDraw:CreateSurface failed.";

			ZEROMEMORY(&ddscaps, sizeof(DDSCAPS2));
			ddscaps.dwCaps = DDSCAPS_BACKBUFFER;
			if (m_lpDDPrimary->GetAttachedSurface(&ddscaps, &m_lpDDBack) != DD_OK)
				throw	"CDirectDraw:GetAttachedSurface failed.";
		}

		// offscreen surface
		m_bForceWrite = FALSE;

		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		if (!m_bSystemMemory) {
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		}
		else {
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			m_bForceWrite = TRUE;	// #### Multi-stage rendering without
		}
		ddsd.dwWidth = SCREEN_WIDTH * 2;
		ddsd.dwHeight = SCREEN_HEIGHT * 2;
		if (m_lpDD->CreateSurface(&ddsd, &m_lpDDRender, NULL) != DD_OK) {
			// #### Memory System
			if (ddsd.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
				throw	"CDirectDraw:CreateSurface failed.";

			// #### When the video memory cannot be retrieved, the system memory is used instead.
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			if (m_lpDD->CreateSurface(&ddsd, &m_lpDDRender, NULL) != DD_OK)
				throw	"CDirectDraw:CreateSurface failed.";

			m_bForceWrite = TRUE;	// #### avoid multi-rendering
		}
		if (!m_bSystemMemory) {
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		}
		else {
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		}
		if (m_lpDD->CreateSurface(&ddsd, &m_lpDDRender2, NULL) != DD_OK) {
			// #### Can't be retrieved from the system memory...
			if (ddsd.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
				throw	"CDirectDraw:CreateSurface failed.";

			// #### When the video memory cannot be stored, the system memory is used instead.
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			if (m_lpDD->CreateSurface(&ddsd, &m_lpDDRender2, NULL) != DD_OK)
				throw	"CDirectDraw:CreateSurface failed.";
		}

		typedef	struct tagDDRGNDATA {
			RGNDATAHEADER	rdh;
			RECT		rect[8];
		} DDRGNDATA;

		DDRGNDATA	rgn;
		rgn.rdh.dwSize = sizeof(RGNDATAHEADER);
		rgn.rdh.iType = RDH_RECTANGLES;
		rgn.rdh.nCount = 1;
		rgn.rdh.nRgnSize = sizeof(RECT);
		rgn.rdh.rcBound.left = 0;
		rgn.rdh.rcBound.top = 0;
		rgn.rdh.rcBound.right = ddsd.dwWidth;
		rgn.rdh.rcBound.bottom = ddsd.dwHeight;
		rgn.rect[0].left = 0;
		rgn.rect[0].top = 0;
		rgn.rect[0].right = ddsd.dwWidth;
		rgn.rect[0].bottom = ddsd.dwHeight;

		// Rendering surface clipper
		if (m_lpDD->CreateClipper(0, &m_lpDDClipper2, NULL) != DD_OK)
			throw	"CDirectDraw:CreateClipper failed.";

		m_lpDDClipper2->SetClipList((LPRGNDATA)&rgn, 0);
		m_lpDDRender->SetClipper(m_lpDDClipper2);

		// Ascii surface(6x6x64character)
		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		//		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = 6 * 8;
		ddsd.dwHeight = 6 * 8;
		if (m_lpDD->CreateSurface(&ddsd, &m_lpDDAscii, NULL) != DD_OK)
			throw	"CDirectDraw:CreateSurface failed.";
		// Color key
		DDCOLORKEY	ddck;
		ZEROMEMORY(&ddck, sizeof(DDCOLORKEY));
		ddck.dwColorSpaceLowValue = 0;
		ddck.dwColorSpaceHighValue = 0;
		m_lpDDAscii->SetColorKey(DDCKEY_SRCBLT, &ddck);

		// Zapper surface
		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		//		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = 16;
		ddsd.dwHeight = 16;
		if (m_lpDD->CreateSurface(&ddsd, &m_lpDDZapper, NULL) != DD_OK)
			throw	"CDirectDraw:CreateSurface failed.";
		// Color key
		ZEROMEMORY(&ddck, sizeof(DDCOLORKEY));
		ddck.dwColorSpaceLowValue = 0;
		ddck.dwColorSpaceHighValue = 0;
		m_lpDDZapper->SetColorKey(DDCKEY_SRCBLT, &ddck);

		// TV
		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		ddsd.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		if (!m_bSystemMemory) {
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		}
		else {
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		}
		ddsd.dwWidth = 512;
		ddsd.dwHeight = 480;
		if (m_lpDD->CreateSurface(&ddsd, &m_lpDDTV, NULL) != DD_OK) {
			// #### ...can't be retrieved even by the system memory...
			if (ddsd.ddsCaps.dwCaps & DDSCAPS_SYSTEMMEMORY)
				throw	"CDirectDraw:CreateSurface failed.";

			// #### When the video memory is full or inaccessible, transfer it to system memory.  In Japanese: ビデオメモリに取れない時、しょうがないのでシステムメモリに取る  Explanation: ビデオメモリ (Video Memory) refers to the dedicated memory used for rendering and storing video content. If this memory becomes full or cannot be accessed, the system may transfer some of the video content to a different type of memory called システムメモリ (System Memory). This allows the system to continue functioning properly and accessing the video content as needed.
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
			if (m_lpDD->CreateSurface(&ddsd, &m_lpDDTV, NULL) != DD_OK)
				throw	"CDirectDraw:CreateSurface failed.";
		}

		// Color key
		ZEROMEMORY(&ddck, sizeof(DDCOLORKEY));
		ddck.dwColorSpaceLowValue = 0;
		ddck.dwColorSpaceHighValue = 0;
		m_lpDDTV->SetColorKey(DDCKEY_SRCBLT, &ddck);

		// DirectDraw/GDI Palette
		ZEROMEMORY(&m_logPalette, sizeof(m_logPalette));
		m_logPalette.palVersion = 0x0300;
		m_logPalette.palNumEntries = 256;

		HDC	hdc = ::GetDC(NULL);
		GetSystemPaletteEntries(hdc, 0, 256, m_logPalette.pe);
		ReleaseDC(NULL, hdc);

		for (i = 0; i < 10; i++) {
			m_logPalette.pe[i].peFlags = PC_EXPLICIT;
			m_logPalette.pe[i + 246].peFlags = PC_EXPLICIT;
		}
		for (i = 10; i < 246; i++) {
			m_logPalette.pe[i].peRed = 0;
			m_logPalette.pe[i].peGreen = 0;
			m_logPalette.pe[i].peBlue = 0;
			if (i >= 0x10 && i < 0x20) {
				m_logPalette.pe[i].peRed = (i - 0x10) * 0x10;
				m_logPalette.pe[i].peGreen = (i - 0x10) * 0x10;
				m_logPalette.pe[i].peBlue = (i - 0x10) * 0x10;
				m_logPalette.pe[i].peFlags = PC_RESERVED;
			}
			else if (i >= 0x40 && i < 0xC0) {
				m_logPalette.pe[i].peFlags = PC_RESERVED;
			}
			else {
				m_logPalette.pe[i].peFlags = PC_NOCOLLAPSE;
			}
		}

		// Surface clear
		ddbltfx.dwSize = sizeof(DDBLTFX);
		ddbltfx.dwFillColor = 0;
		if (m_lpDDBack) {
			m_lpDDBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		}
		m_lpDDRender->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);
		m_lpDDRender2->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

		// Palette object
		DDSURFACEDESC2	ddsd;
		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		m_lpDDPrimary->GetSurfaceDesc(&ddsd);

		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8) {
			if (!m_bScreenMode) {
				if ((m_hPalette = CreatePalette((LOGPALETTE*)&m_logPalette)) == NULL)
					throw	"CDirectDraw:CreatePalette failed.";
			}
			else {
				if (m_lpDD->CreatePalette(DDPCAPS_8BIT, m_logPalette.pe, &m_lpDDPalette, NULL) != DD_OK)
					throw	"CDirectDraw:CreatePalette error";
				if (m_lpDDPrimary->SetPalette(m_lpDDPalette) != DD_OK)
					throw	"CDirectDraw:SetPalette failed.";
			}
		}

		// Palette calculate
		CalcPaletteTable();
		// Palette Realize
		RealizePalette();
		m_bPaletteUpdate = TRUE;

		// Character setup
		SetLZSSChar(lzAscii, m_lpDDAscii);
		SetLZSSChar(lzSight, m_lpDDZapper);
		SetLZSSChar(lzTVlayer, m_lpDDTV);

		// #### For redrawing.
		m_bDeltaUpdate = TRUE;
	}
	catch (char* str) {
		ReleaseSurface();

		//		DEBUGOUT( "%s\n", str );
		::MessageBox(m_hWnd, str, "ERROR", MB_ICONERROR | MB_OK);

		return	FALSE;
	}
	//	DEBUGOUT( "CDirectDraw:InitialSurface complete.\n" );

	return	TRUE;
}

// #### Surface opening
BOOL	CDirectDraw::ReleaseSurface(void)
{
	if (!m_lpDD)
		return	FALSE;

	GDIDELETE(m_hPalette);
	RELEASE(m_lpDDPalette);

	RELEASE(m_lpDDClipper2);
	RELEASE(m_lpDDClipper);

	RELEASE(m_lpDDTV);
	RELEASE(m_lpDDZapper);
	RELEASE(m_lpDDAscii);
	RELEASE(m_lpDDRender2);
	RELEASE(m_lpDDRender);
	RELEASE(m_lpDDBack);
	RELEASE(m_lpDDPrimary);

	return	TRUE;
}

// #### Subsurface Listing
BOOL	CDirectDraw::RestoreSurfaceSub(LPDIRECTDRAWSURFACE7 lpSurface)
{
	if (lpSurface) {
		if (lpSurface->IsLost() == DDERR_SURFACELOST) {
			// #### If you lost something, you can try to restore or recover it by following these steps:  1. Retrace your steps: Think about the last time you had the item and try to remember where you were and what you were doing. This can help you figure out where you might have left it. 2. Check your usual places: Look in all the places you normally keep your item, such as your home, car, or workplace. 3. Ask others if they've seen it: If you can't find your item, ask friends, family, or coworkers if they've seen it. They may have picked it up or know where it is. 4. Check lost and found: If you were in a public place, such as a store or restaurant, check the lost and found area to see if anyone has turned in your item. 5. Use technology to track it: If you've misplaced something electronic, such as a phone or laptop, use GPS or other tracking features to try to locate it. 6. Consider hiring a professional: If you're still unable to find your item after trying these steps, you may want to consider hiring a professional investigator or service that specializes in finding lost items.  Remember, patience and persistence are key when trying to recover something that has been lost. Keep looking and don't give up hope!
//			DEBUGOUT( "CDirectDraw:Restore surface...." );
			if (lpSurface->Restore() == DD_OK) {
				//				DEBUGOUT( "Ok.\n" );
			}
			else {
				//				DEBUGOUT( "Failed.\n" );
			}
			return	FALSE;
		}
	}
	return	TRUE;
}

// #### Surface Recovery List
BOOL	CDirectDraw::RestoreSurface()
{
	if (!m_lpDD) return FALSE;

	if (m_bNoRestore)
		return	FALSE;

	BOOL	bRet = TRUE;

	// #### the content will be lost
	if (!RestoreSurfaceSub(m_lpDDRender)) {
		bRet = FALSE;
	}
	if (!RestoreSurfaceSub(m_lpDDRender2)) {
		bRet = FALSE;
	}
	if (!RestoreSurfaceSub(m_lpDDBack)) {
		bRet = FALSE;
	}
	if (!RestoreSurfaceSub(m_lpDDPrimary)) {
		bRet = FALSE;
	}
	if (!RestoreSurfaceSub(m_lpDDAscii)) {
		SetLZSSChar(lzAscii, m_lpDDAscii);
		bRet = FALSE;
	}
	if (!RestoreSurfaceSub(m_lpDDZapper)) {
		SetLZSSChar(lzSight, m_lpDDZapper);
		bRet = FALSE;
	}
	if (!RestoreSurfaceSub(m_lpDDTV)) {
		SetLZSSChar(lzTVlayer, m_lpDDTV);
		bRet = FALSE;
	}

	if (!bRet) {
		// #### redraw
		m_bDeltaUpdate = TRUE;
	}

	return	bRet;
}

//
// #### Display mode change started.
//
BOOL	CDirectDraw::BeginDisplayChange()
{
	if (!m_lpDD)
		return	FALSE;

	// #### Already changed or not?
	if (m_bChangeMode)
		return	FALSE;

	// #### Under construction
	m_bChangeMode = TRUE;

	if (m_bScreenMode) {
		m_lpDD->RestoreDisplayMode();
	}

	// #### Release the surface
	ReleaseSurface();

	// #### Coordinated mode
	m_lpDD->SetCooperativeLevel(NULL, DDSCL_NORMAL);

	return	TRUE;
}

//
// #### The display mode change has been completed, and the surface of the object has been rebuilt.
//
BOOL	CDirectDraw::EndDisplayChange()
{
	if (!m_lpDD || !m_bChangeMode)
		return	FALSE;

	// Rebuild surface
	if (!InitialSurface(m_bScreenMode)) {
		m_bChangeMode = FALSE;
		return	FALSE;
	}

	// Wait
	::Sleep(250);

	// Change mode complete
	m_bChangeMode = FALSE;

	return	TRUE;
}

//
// #### When the display resolution or orientation changes, the following function/handler is called
//
BOOL	CDirectDraw::OnChangeDisplayMode()
{
	// #### When changing oneself, cancel the change.
	if (m_bChangeMode)
		return	TRUE;

	// #### does not exist
	if (!m_lpDD)
		return	FALSE;

	//	DEBUGOUT( "CDirectDraw:OnChangeDisplayMode\n" );

		// #### Coordinated Mode Check
	HRESULT	hr = m_lpDD->TestCooperativeLevel();

	if (!m_bScreenMode) {
		// #### window
		if (hr == DDERR_EXCLUSIVEMODEALREADYSET) {
			// #### There is a jerk who doesn't care about anyone else and only thinks about himself.
//			DEBUGOUT( "CDirectDraw:DDERR_EXCLUSIVEMODEALREADYSET\n" );
			// #### Listen up, let's do this!
			m_bNoRestore = TRUE;
			return	TRUE;
		}
		else if (hr == DDERR_WRONGMODE || hr == DD_OK) {
			// #### and
//			DEBUGOUT( "CDirectDraw:DDERR_WRONGMODE\n" );
			// #### Restore and make it better
			m_bNoRestore = FALSE;

			// #### open
			ReleaseSurface();

			// #### Surface Initialization
			BOOL	bRet = InitialSurface(m_bScreenMode);

			if (bRet) {
				//				DEBUGOUT( "CDirectDraw:InitialSurface ok.\n" );
			}
			else {
				//				DEBUGOUT( "CDirectDraw:InitialSurface failed.\n" );
			}

			return	bRet;
		}
		else if (hr == DDERR_INVALIDOBJECT) {
			//			DEBUGOUT( "CDirectDraw:DDERR_INVALIDOBJECT\n" );
			return	FALSE;
		}
		else if (hr == DDERR_NOEXCLUSIVEMODE) {
			//			DEBUGOUT( "CDirectDraw:DDERR_NOEXCLUSIVEMODE\n" );
		}
		else {
			// #### CDirectDraw:Unknown error. hr=0x[hr]
		}
		// #### for now, we will put it on hold
		m_bNoRestore = FALSE;
		RestoreSurface();

		return	TRUE;
	}
	else {
		// #### full screen
		if (hr == DDERR_NOEXCLUSIVEMODE) {
			// #### After exiting full screen
//			DEBUGOUT( "CDirectDraw:DDERR_NOEXCLUSIVEMODE\n" );
			return	TRUE;
		}

		if (hr == DD_OK) {
			// #### Let's get back to the matter at hand...
			// #### surface
			RestoreSurface();

			return	TRUE;
		}

		// #### Unclear cause
		return	FALSE;
	}

	return	TRUE;
}

void	CDirectDraw::SetDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, DWORD dwRate)
{
	m_dwDisplayWidth = dwWidth;
	m_dwDisplayHeight = dwHeight;
	m_dwDisplayDepth = dwDepth;
	m_dwDisplayRate = dwRate;
}

void	CDirectDraw::GetDisplayMode(DWORD& dwWidth, DWORD& dwHeight, DWORD& dwDepth, DWORD& dwRate)
{
	dwWidth = m_dwDisplayWidth;
	dwHeight = m_dwDisplayHeight;
	dwDepth = m_dwDisplayDepth;
	dwRate = m_dwDisplayRate;
}

BOOL	CDirectDraw::GetDisplayMode(INT no, DWORD& dwWidth, DWORD& dwHeight, DWORD& dwDepth, DWORD& dwRate)
{
	if ((int)m_DisplayModes.size() < no)
		return	FALSE;

	dwWidth = m_DisplayModes[no].dwWidth;
	dwHeight = m_DisplayModes[no].dwHeight;
	dwDepth = m_DisplayModes[no].dwDepth;
	dwRate = m_DisplayModes[no].dwRate;

	return	TRUE;
}

INT	CDirectDraw::GetMatchDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, DWORD dwRate)
{
	for (int i = 0; i < (int)m_DisplayModes.size(); i++) {
		if (m_DisplayModes[i].dwWidth == dwWidth
			|| m_DisplayModes[i].dwHeight == dwHeight
			|| m_DisplayModes[i].dwDepth == dwDepth
			|| m_DisplayModes[i].dwRate == dwRate)
			return	i;
	}

	return	-1;
}

BOOL	CDirectDraw::IsNowDisplayMode(DWORD dwWidth, DWORD dwHeight, DWORD dwDepth, DWORD dwRate)
{
	if (m_dwDisplayWidth == dwWidth && m_dwDisplayHeight == dwHeight
		&& m_dwDisplayDepth == dwDepth && m_dwDisplayRate == dwRate)
		return	TRUE;
	return	FALSE;
}

// #### Acquiring bit position
void	CDirectDraw::GetBitMask(DWORD val, int& shift, int& bits)
{
	shift = 0;
	while (!(val & (1 << shift)) && (shift < 32)) {
		shift++;
	}

	bits = 32;
	while (!(val & (1 << (bits - 1))) && (bits > 0)) {
		bits--;
	}
	bits = bits - shift;
}

static	float	PalConvTbl[][3] = {
	1.00f, 1.00f, 1.00f,
	1.00f, 0.80f, 0.73f,
	0.73f, 1.00f, 0.70f,
	0.76f, 0.78f, 0.58f,
	0.86f, 0.80f, 1.00f,
	0.83f, 0.68f, 0.85f,
	0.67f, 0.77f, 0.83f,
	0.68f, 0.68f, 0.68f,
	//	1.00f, 1.00f, 1.00f,
};

// #### Calculation with a spreadsheet
BOOL	CDirectDraw::CalcPaletteTable()
{
	INT	i, j;

	if (!m_lpDD || !m_lpDDPrimary)
		return	FALSE;

	DDSURFACEDESC2	ddsd;
	ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_PIXELFORMAT;
	if (m_lpDDPrimary->GetSurfaceDesc(&ddsd) != DD_OK)
		throw "CDirectDraw:GetSurfaceDesc error";

	INT	Rbit, Gbit, Bbit;
	INT	Rsft, Gsft, Bsft;

	if (ddsd.ddpfPixelFormat.dwRGBBitCount != 8) {
		GetBitMask(ddsd.ddpfPixelFormat.dwRBitMask, Rsft, Rbit);
		GetBitMask(ddsd.ddpfPixelFormat.dwGBitMask, Gsft, Gbit);
		GetBitMask(ddsd.ddpfPixelFormat.dwBBitMask, Bsft, Bbit);
	}

	for (j = 0; j < 8; j++) {
		for (i = 0; i < 64; i++) {
			DWORD	Rn, Gn, Bn;
			DWORD	Rs, Gs, Bs;

			// Normal
			Rn = (DWORD)(PalConvTbl[j][0] * m_PaletteBuf[i].r);
			Gn = (DWORD)(PalConvTbl[j][1] * m_PaletteBuf[i].g);
			Bn = (DWORD)(PalConvTbl[j][2] * m_PaletteBuf[i].b);
			// Scanline
			Rs = (DWORD)(PalConvTbl[j][0] * m_PaletteBuf[i].r * m_nScanlineColor / 100.0f);
			Gs = (DWORD)(PalConvTbl[j][1] * m_PaletteBuf[i].g * m_nScanlineColor / 100.0f);
			Bs = (DWORD)(PalConvTbl[j][2] * m_PaletteBuf[i].b * m_nScanlineColor / 100.0f);

			m_cpPalette[j][i + 0x00].rgbRed = (BYTE)Rn;
			m_cpPalette[j][i + 0x00].rgbGreen = (BYTE)Gn;
			m_cpPalette[j][i + 0x00].rgbBlue = (BYTE)Bn;
			m_cpPalette[j][i + 0x40].rgbRed = (BYTE)Rs;
			m_cpPalette[j][i + 0x40].rgbGreen = (BYTE)Gs;
			m_cpPalette[j][i + 0x40].rgbBlue = (BYTE)Bs;

			m_cnPalette[j][i] = ((Rn >> (8 - Rbit)) << Rsft) | ((Gn >> (8 - Gbit)) << Gsft) | ((Bn >> (8 - Bbit)) << Bsft);
			m_csPalette[j][i] = ((Rs >> (8 - Rbit)) << Rsft) | ((Gs >> (8 - Gbit)) << Gsft) | ((Bs >> (8 - Bbit)) << Bsft);

			// RGB555
			if (Rsft > Bsft) {
				// #### When converting RGB555 to RGB888, the following happens:  * Each RGB555 value is multiplied by 2 to convert it to an RGB888 value. This means that the minimum value of an RGB888 pixel is 0x000000 (or 0), and the maximum value is 0xFFFFFF (or 65535).  So, if you have an array of RGB555 values, you can convert it to an RGB888 array by simply multiplying each value by 2: ``` RGB888 arr = new RGB888[arr.length]; for (int i = 0; i < arr.length; i++) {     arr[i] = (arr[i] * 2); } ``` Alternatively, you can use the `Arrays.copyOf()` method to create a new RGB888 array with the same length as the original RGB555 array, and then iterate over the elements to perform the conversion: ``` RGB888 arr = Arrays.copyOf(arr, arr.length); for (int i = 0; i < arr.length; i++) {     arr[i] = (arr[i] * 2); } ``` Note that the resulting RGB888 array will have a range of values from 0 to 65535, rather than the original range of -128 to 127.
				m_cfPalette[j][i] = ((Rn >> (8 - 5)) << 10) | ((Gn >> (8 - 5)) << 5) | ((Bn >> (8 - 5)) << 0);
			}
			else {
				// #### When converting BGR555 to BGR888, the following steps can be taken:  1. Shift the red channel (R) by 32 bits to the right: R' = R + 0xFFFFFF00 2. Shift the green channel (G) by 32 bits to the right: G' = G + 0xFFFFFF00 3. Shift the blue channel (B) by 32 bits to the right: B' = B + 0xFFFFFF00 4. Interleave the R', G', and B' channels to produce the final BGR888 pixel value: R8 = (R' >> 16) | (G' >> 8) | B';  Here, the | operator is used for bitwise OR operation.
				m_cfPalette[j][i] = ((Rn >> (8 - 5)) << 0) | ((Gn >> (8 - 5)) << 5) | ((Bn >> (8 - 5)) << 10);
			}

			// Monochrome
			Rn = (DWORD)(m_PaletteBuf[i & 0x30].r);
			Gn = (DWORD)(m_PaletteBuf[i & 0x30].g);
			Bn = (DWORD)(m_PaletteBuf[i & 0x30].b);
			Rn =
				Gn =
				Bn = (DWORD)(0.299f * Rn + 0.587f * Gn + 0.114f * Bn);
			Rn = (DWORD)(PalConvTbl[j][0] * Rn);
			Gn = (DWORD)(PalConvTbl[j][1] * Gn);
			Bn = (DWORD)(PalConvTbl[j][2] * Bn);
			if (Rn > 0xFF) Rs = 0xFF;
			if (Gn > 0xFF) Gs = 0xFF;
			if (Bn > 0xFF) Bs = 0xFF;
			// Scanline
			Rs = (DWORD)(m_PaletteBuf[i & 0x30].r * m_nScanlineColor / 100.0f);
			Gs = (DWORD)(m_PaletteBuf[i & 0x30].g * m_nScanlineColor / 100.0f);
			Bs = (DWORD)(m_PaletteBuf[i & 0x30].b * m_nScanlineColor / 100.0f);
			Rs =
				Gs =
				Bs = (DWORD)(0.299f * Rs + 0.587f * Gs + 0.114f * Bs);
			Rs = (DWORD)(PalConvTbl[j][0] * Rs);
			Gs = (DWORD)(PalConvTbl[j][1] * Gs);
			Bs = (DWORD)(PalConvTbl[j][2] * Bs);
			if (Rs > 0xFF) Rs = 0xFF;
			if (Gs > 0xFF) Gs = 0xFF;
			if (Bs > 0xFF) Bs = 0xFF;

			m_mpPalette[j][i + 0x00].rgbRed = (BYTE)Rn;
			m_mpPalette[j][i + 0x00].rgbGreen = (BYTE)Gn;
			m_mpPalette[j][i + 0x00].rgbBlue = (BYTE)Bn;
			m_mpPalette[j][i + 0x40].rgbRed = (BYTE)Rs;
			m_mpPalette[j][i + 0x40].rgbGreen = (BYTE)Gs;
			m_mpPalette[j][i + 0x40].rgbBlue = (BYTE)Bs;

			m_mnPalette[j][i] = ((Rn >> (8 - Rbit)) << Rsft) | ((Gn >> (8 - Gbit)) << Gsft) | ((Bn >> (8 - Bbit)) << Bsft);
			m_msPalette[j][i] = ((Rs >> (8 - Rbit)) << Rsft) | ((Gs >> (8 - Gbit)) << Gsft) | ((Bs >> (8 - Bbit)) << Bsft);

			// RGB555
			if (Rsft > Bsft) {
				// #### When converting RGB555 to RGB888, the process involves simply multiplying each value in the RGB555 color space by a factor of 1.30625 to shift the color spectrum towards the RGB888 space.  Here's the formula for this conversion:  RGB888 = (RGB555 x 1.30625)  So, if you have a color value in RGB555 represented as (R, G, B), the corresponding value in RGB888 would be:  (R x 1.30625, G x 1.30625, B x 1.30625)  For example, if you have the color value (255, 0, 0) in RGB555, the corresponding value in RGB888 would be (32768, 0, 0).
				m_mfPalette[j][i] = ((Rn >> (8 - 5)) << 10) | ((Gn >> (8 - 5)) << 5) | ((Bn >> (8 - 5)) << 0);
			}
			else {
				// #### sub-pixel interpolation
				m_mfPalette[j][i] = ((Rn >> (8 - 5)) << 0) | ((Gn >> (8 - 5)) << 5) | ((Bn >> (8 - 5)) << 10);
			}
		}
	}

	// #### Redrawing for the sake of...
	m_bDeltaUpdate = TRUE;

	return	TRUE;
}

// #### Setting the parameter table
void	CDirectDraw::SetPaletteTable(LPBYTE pal)
{
	if (pal)
		memcpy(m_PaletteBuf, pal, sizeof(m_PaletteBuf));
	else
		memcpy(m_PaletteBuf, m_PalDefault, sizeof(m_PaletteBuf));

	CalcPaletteTable();

	m_bPaletteUpdate = TRUE;
}

// #### Setting the Pallet Table
void	CDirectDraw::SetPaletteTable(RGBQUAD* rgb)
{
	for (INT i = 0; i < 64; i++) {
		m_PaletteBuf[i].r = rgb[i].rgbRed;
		m_PaletteBuf[i].g = rgb[i].rgbGreen;
		m_PaletteBuf[i].b = rgb[i].rgbBlue;
	}

	CalcPaletteTable();

	m_bPaletteUpdate = TRUE;
}

// #### obtaining a pivot table
void	CDirectDraw::GetPaletteTable(RGBQUAD* rgb)
{
	for (INT i = 0; i < 64; i++) {
		rgb[i].rgbRed = m_PaletteBuf[i].r;
		rgb[i].rgbGreen = m_PaletteBuf[i].g;
		rgb[i].rgbBlue = m_PaletteBuf[i].b;
		rgb[i].rgbReserved = 0;
	}
}

// #### Updating the Pallet File
void	CDirectDraw::SetPaletteFile(LPCTSTR fname)
{
	// #### Update Pallet File
	if (strlen(fname) > 0) {
		FILE* fp;
		if ((fp = ::fopen(fname, "rb"))) {
			BYTE	palbuf[192];
			// #### inner speech
			if (::fread(palbuf, 192, 1, fp) == 1) {
				// #### Pallet Changes and Calculations
				SetPaletteTable(palbuf);
			}
			else {
				// #### When I couldn't read it, I defaulted.
				SetPaletteTable((LPBYTE)NULL);
			}
			FCLOSE(fp);
		}
		else {
			// #### Opened not found
			SetPaletteTable((LPBYTE)NULL);
		}
	}
	else {
		// #### Default
		SetPaletteTable((LPBYTE)NULL);
	}
}

// #### Alt + Enter
BOOL	CDirectDraw::SetFullScreenGDI(BOOL bMode)
{
	// #### Mindfulness Check.
	if (!m_lpDD || !m_lpDDPrimary)
		return	FALSE;

	if (m_bScreenMode) {
		if (!m_bGDI) {
			if (bMode) {
				RELEASE(m_lpDDClipper); // #### in answer to
				if (m_lpDD->CreateClipper(0, &m_lpDDClipper, NULL) == DD_OK) {
					m_lpDDClipper->SetHWnd(0, m_hWnd);
					m_lpDDPrimary->SetClipper(m_lpDDClipper);
					if (m_lpDD->FlipToGDISurface() == DD_OK) {
						m_bGDI = TRUE;
					}
					else {
						RELEASE(m_lpDDClipper);
						return	FALSE;
					}
				}
			}
		}
		else {
			if (!bMode) {
				RELEASE(m_lpDDClipper);
				m_bGDI = FALSE;
			}
		}
	}
	return	TRUE;
}

void	CDirectDraw::RealizePalette()
{
	if (!m_lpDD || !m_lpDDPrimary)
		return;

	if (!m_bScreenMode) {
		DDSURFACEDESC2	ddsd;
		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		m_lpDDPrimary->GetSurfaceDesc(&ddsd);

		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8) {
			HDC	hdc;
			hdc = ::GetDC(m_hWnd);
			::SelectPalette(hdc, m_hPalette, FALSE);
			::RealizePalette(hdc);
			::ReleaseDC(m_hWnd, hdc);

			m_bPaletteUpdate = TRUE;
		}
	}
}

// #### Sketching
void	CDirectDraw::OnScreenDraw()
{
	if (!m_bScreenMode) {
		// Window mode
		Blt();
		Flip();
	}
	else {
		// Fullscreen mode
		if (m_bGDI) {
			Blt();
			Flip();
		}
	}
}

void	CDirectDraw::SetPaletteMode(INT nMode, BOOL bMono)
{
	if ((m_nPaletteMode != nMode) || (m_bMonoMode != bMono)) {
		m_bPaletteUpdate = TRUE;
	}
	m_nPaletteMode = nMode;
	m_bMonoMode = bMono;
}

// #### Obtaining a pivot table while display is on
void	CDirectDraw::GetPaletteData(RGBQUAD* rgb)
{
	if (!m_bMonoMode) {
		for (INT i = 0; i < 64; i++) {
			rgb[i] = m_cpPalette[m_nPaletteMode][i];
			rgb[i + 0x40] = m_mpPalette[m_nPaletteMode][i];
		}
	}
	else {
		for (INT i = 0; i < 64; i++) {
			rgb[i] = m_mpPalette[m_nPaletteMode][i];
			rgb[i + 0x40] = m_mpPalette[m_nPaletteMode][i];
		}
	}
}

void	CDirectDraw::GetPaletteData2(RGBQUAD* rgb)
{
	for (INT j = 0; j < 8; j++) {
		for (INT i = 0; i < 64; i++) {
			rgb[j * 256 + i] = m_cpPalette[j][i];
			rgb[j * 256 + i + 0x40] = m_mpPalette[j][i];
		}
	}
}

void	CDirectDraw::Blt()
{
	INT	i;
	DDSURFACEDESC2	ddsd;

	if (!m_lpDD || !m_lpDDPrimary)
		return;
	// ScreenMode changing?
	if (m_bChangeMode)
		return;
	// Surface lost check & restore
	if (!RestoreSurface())
		return;

	ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	if (m_lpDDPrimary->GetSurfaceDesc(&ddsd) != DD_OK)
		return;

	// Palette copy
	if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8) {
		static int	linecolor = 0;

		if (m_LineColormode[1] != linecolor) {
			linecolor = m_LineColormode[1];
			m_bPaletteUpdate = TRUE;
		}

		if (!(m_LineColormode[1] & 0x80)) {
			for (i = 0; i < 128; i++) {
				m_logPalette.pe[i + 0x40].peRed = m_cpPalette[m_nPaletteMode][i].rgbRed;
				m_logPalette.pe[i + 0x40].peGreen = m_cpPalette[m_nPaletteMode][i].rgbGreen;
				m_logPalette.pe[i + 0x40].peBlue = m_cpPalette[m_nPaletteMode][i].rgbBlue;
			}
		}
		else {
			for (i = 0; i < 128; i++) {
				m_logPalette.pe[i + 0x40].peRed = m_mpPalette[m_nPaletteMode][i].rgbRed;
				m_logPalette.pe[i + 0x40].peGreen = m_mpPalette[m_nPaletteMode][i].rgbGreen;
				m_logPalette.pe[i + 0x40].peBlue = m_mpPalette[m_nPaletteMode][i].rgbBlue;
			}
		}
		if (!m_bScreenMode) {
			::AnimatePalette(m_hPalette, 0, 256, m_logPalette.pe);
		}
	}

	// Size calculate
	BOOL	bDoubleWidth = FALSE;
	BOOL	bDoubleHeight = FALSE;

	RECT	rcW;
	rcW.left = 0;
	rcW.top = 0;
	rcW.right = SCREEN_WIDTH;
	rcW.bottom = SCREEN_HEIGHT;
	if (m_bDoubleSize || (m_nBltFilter && IsMMX())) {
		rcW.right *= 2;
		rcW.bottom *= 2;
		bDoubleWidth = TRUE;
		bDoubleHeight = TRUE;
	}
	else if (m_bScanlineMode) {
		rcW.bottom *= 2;
		bDoubleHeight = TRUE;
	}

	// Render function
	BLTFUNC* bltfunc;
	if (!m_nBltFilter || !IsMMX()) {
		if (!m_bDoubleSize) {
			if (!m_bScanlineMode) {
				bltfunc = NormalBltTable;
			}
			else {
				bltfunc = ScanlineBltTable;
			}
		}
		else {
			if (!m_bScanlineMode) {
				bltfunc = DoubleBltTable;
			}
			else {
				bltfunc = DoubleScanlineBltTable;
			}
		}
	}
	else {
		switch (m_nBltFilter) {
		case	BLTFILTER_2XSAI:
			bltfunc = nx2xSaIBltTable;
			break;
		case	BLTFILTER_SUPER2XSAI:
			bltfunc = nxSuper2xSaIBltTable;
			break;
		case	BLTFILTER_SUPEREAGLE:
			bltfunc = nxSuperEagleBltTable;
			break;
		case	BLTFILTER_SCALE2X:
			bltfunc = nxScale2xBltTable;
			break;
		case	BLTFILTER_HQ2X:
			bltfunc = nxHq2xBltTable;
			break;
		case	BLTFILTER_LQ2X:
			bltfunc = nxLq2xBltTable;
			break;
		default:
			break;
		}
	}

	BOOL	bFilter = FALSE;
	LPBYTE	lpRdr = m_lpRender + 8;

	ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);

	if (m_bForceWrite) {
		// #### Multi-stage rendering without
		if (m_lpDDRender->Lock(NULL, &ddsd, 0, NULL) == DD_OK) {
			switch (ddsd.ddpfPixelFormat.dwRGBBitCount) {
			case	8:
				(this->*bltfunc[0])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, TRUE);
				break;
			case	16:
				(this->*bltfunc[1])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, TRUE);
				break;
			case	24:
				(this->*bltfunc[2])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, TRUE);
				break;
			case	32:
				(this->*bltfunc[3])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, TRUE);
				break;
			default:
				break;
			}

			m_lpDDRender->Unlock(NULL);
			m_bDeltaUpdate = FALSE;
		}
	}
	else {
		if (m_lpDDRender2->Lock(NULL, &ddsd, 0, NULL) == DD_OK) {
			switch (ddsd.ddpfPixelFormat.dwRGBBitCount) {
			case	8:
				(this->*bltfunc[0])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, m_bDeltaUpdate);
				break;
			case	16:
				(this->*bltfunc[1])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, m_bDeltaUpdate);
				break;
			case	24:
				(this->*bltfunc[2])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, m_bDeltaUpdate);
				break;
			case	32:
				(this->*bltfunc[3])(lpRdr, &m_lpRenderDelta[DELTA_WIDTH * 2 * sizeof(DWORD)], ddsd, m_bDeltaUpdate);
				break;
			default:
				break;
			}

			m_lpDDRender2->Unlock(NULL);
			m_bDeltaUpdate = FALSE;

			m_lpDDRender->Blt(&rcW, m_lpDDRender2, &rcW, 0, NULL);
		}
	}

	// #### TV programming.
	if (m_bTVFrameMode) {
		m_lpDDRender->Blt(&rcW, m_lpDDTV, NULL, DDBLT_KEYSRC, NULL);
	}

	// Infomation string
	if (strlen(m_szInfo) > 0) {
		INT	x, y, o;

		x = bDoubleWidth ? 16 : 8;
		o = m_bAllLine ? 0 : 8;
		y = bDoubleHeight ? 8 + o * 2 : 4 + o;

		RenderString(x, y, m_szInfo);
	}
	// Message string
	if (m_bMessage) {
		if (timeGetTime() - m_dwMessageTime > 1500) {
			m_bMessage = FALSE;
		}
		if (strlen(m_szMess) > 0) {
			INT	x, y, o;

			x = bDoubleWidth ? 16 : 8;
			o = m_bAllLine ? 8 : 0;
			y = bDoubleHeight ? (SCREEN_HEIGHT * 2 - 36 + o * 2) : (SCREEN_HEIGHT - 18 + o);

			RenderString(x, y, m_szMess);
		}
	}
	// DiskAccessLamp
	if (m_bDiskAccessLamp) {
		INT	x, y, o;

		x = bDoubleWidth ? SCREEN_WIDTH * 2 - 20 : SCREEN_WIDTH - 10;
		o = m_bAllLine ? 0 : 8;
		y = bDoubleHeight ? 8 + o * 2 : 4 + o;

		RenderString(x, y, "\\");
	}

	if (m_bZapper && m_bZapperDraw) {
		if (m_ZapperPosX >= 0 && m_ZapperPosX < SCREEN_WIDTH && m_ZapperPosY >= 0 && m_ZapperPosY < SCREEN_HEIGHT) {
			RECT	rcS, rcZ;

			SetRect(&rcZ, 0, 0, 16, 16);
			if (!bDoubleWidth) {
				rcS.left = m_ZapperPosX - (rcZ.right - rcZ.left) / 2;
				rcS.right = m_ZapperPosX + (rcZ.right - rcZ.left) / 2;
			}
			else {
				rcS.left = m_ZapperPosX * 2 - (rcZ.right - rcZ.left);
				rcS.right = m_ZapperPosX * 2 + (rcZ.right - rcZ.left);
			}
			if (!bDoubleHeight) {
				rcS.top = m_ZapperPosY - (rcZ.bottom - rcZ.top) / 2;
				rcS.bottom = m_ZapperPosY + (rcZ.bottom - rcZ.top) / 2;
			}
			else {
				rcS.top = m_ZapperPosY * 2 - (rcZ.bottom - rcZ.top);
				rcS.bottom = m_ZapperPosY * 2 + (rcZ.bottom - rcZ.top);
			}

			m_lpDDRender->Blt(&rcS, m_lpDDZapper, &rcZ, DDBLT_KEYSRC, NULL);
		}
	}
}

void	CDirectDraw::Flip()
{
	if (!m_lpDD || !m_lpDDPrimary)
		return;
	// ScreenMode changing?
	if (m_bChangeMode)
		return;
	// Surface lost check & restore
	if (!RestoreSurface())
		return;

	DDSURFACEDESC2	ddsd;
	ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	m_lpDDPrimary->GetSurfaceDesc(&ddsd);

	// Size calculate
	BOOL	bDoubleWidth = FALSE;
	BOOL	bDoubleHeight = FALSE;
	if (m_bDoubleSize || (m_nBltFilter && IsMMX())) {
		bDoubleWidth = TRUE;
		bDoubleHeight = TRUE;
	}
	else if (m_bScanlineMode) {
		bDoubleHeight = TRUE;
	}

	RECT	rcS, rcC;
	if (!bDoubleWidth) {
		rcS.left = 0;
		rcS.right = SCREEN_WIDTH;
	}
	else {
		rcS.left = 0;
		rcS.right = SCREEN_WIDTH * 2;
	}
	if (!m_bAllLine) {
		rcS.top = 8;
		rcS.bottom = SCREEN_HEIGHT - 8;
	}
	else {
		rcS.top = 0;
		rcS.bottom = SCREEN_HEIGHT;
	}
	if (bDoubleHeight) {
		rcS.top *= 2;
		rcS.bottom *= 2;
	}

	if (!m_bScreenMode) {
		// Window mode
		::GetClientRect(m_hWnd, &rcC);
		::ClientToScreen(m_hWnd, (POINT*)&rcC.left);
		::ClientToScreen(m_hWnd, (POINT*)&rcC.right);

		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8) {
			HDC	hdc;
			if (m_lpDDPrimary->GetDC(&hdc) == DD_OK) {
				::SelectPalette(hdc, m_hPalette, FALSE);
				::RealizePalette(hdc);
				m_lpDDPrimary->ReleaseDC(hdc);
			}
		}
	}
	else {
		// Fullscreen mode
		if (!m_bMaxZoom) {
			DDBLTFX	ddbltfx;
			ddbltfx.dwSize = sizeof(DDBLTFX);
			ddbltfx.dwFillColor = 0;
			m_lpDDBack->Blt(NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx);

			// Position offset caluclate
			LONG	swidth, sheight;
			LONG	dwidth, dheight;
			LONG	hmul, vmul;

			if (!m_bAspect)  swidth = SCREEN_WIDTH;
			else		  swidth = 320;
			if (!m_bAllLine) sheight = SCREEN_HEIGHT - 16;
			else		  sheight = SCREEN_HEIGHT;

			dwidth = (LONG)ddsd.dwWidth;
			dheight = (LONG)ddsd.dwHeight;
			hmul = dwidth / swidth;
			vmul = dheight / sheight;

			if (vmul < hmul) hmul = vmul;
			else		  vmul = hmul;

			rcC.left = (dwidth - swidth * hmul) / 2;
			rcC.top = (dheight - sheight * vmul) / 2;
			rcC.right = rcC.left + swidth * hmul;
			rcC.bottom = rcC.top + sheight * vmul;
		}
		else {
			// Maximum zoom
			rcC.left = 0;
			rcC.top = 0;
			rcC.right = (LONG)ddsd.dwWidth;
			rcC.bottom = (LONG)ddsd.dwHeight;
		}
	}

	if (!m_bScreenMode) {
		// Window mode
		if (m_bWindowVSync) {
			HRESULT	hr;
			while (TRUE) {
				hr = m_lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
				if (hr == DD_OK)
					break;
				if (hr == DDERR_SURFACELOST) {
					break;
				}
				if (hr != DDERR_WASSTILLDRAWING) {
					break;
				}
			}
			//			m_lpDDPrimary->Blt( NULL, m_lpDDBack, NULL, DDBLT_WAIT, NULL );
		}

		m_lpDDPrimary->Blt(&rcC, m_lpDDRender, &rcS, DDBLT_WAIT, NULL);

		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8) {
			if (m_bPaletteUpdate) {
				m_bPaletteUpdate = FALSE;
				::AnimatePalette(m_hPalette, 0, 256, m_logPalette.pe);
			}
		}
	}
	else {
		// Fullscreen mode
		m_lpDDBack->Blt(&rcC, m_lpDDRender, &rcS, DDBLT_WAIT, NULL);

		if (ddsd.ddpfPixelFormat.dwRGBBitCount == 8) {
			if (m_bPaletteUpdate) {
				m_bPaletteUpdate = FALSE;
				m_lpDDPalette->SetEntries(0, 0, 256, m_logPalette.pe);
			}
		}

		if (m_bFlip) {
			if (!m_bGDI) {
				HRESULT	hr;
				while (TRUE) {
					hr = m_lpDDPrimary->Flip(NULL, DDFLIP_WAIT);
					if (hr == DD_OK)
						break;
					if (hr == DDERR_SURFACELOST) {
						break;
					}
					if (hr != DDERR_WASSTILLDRAWING) {
						break;
					}
				}
			}
			else {
				HRESULT	hr;
				while (TRUE) {
					hr = m_lpDD->WaitForVerticalBlank(DDWAITVB_BLOCKBEGIN, NULL);
					if (hr == DD_OK)
						break;
					if (hr == DDERR_SURFACELOST) {
						break;
					}
					if (hr != DDERR_WASSTILLDRAWING) {
						break;
					}
				}
				m_lpDDPrimary->Blt(NULL, m_lpDDBack, NULL, DDBLT_WAIT, NULL);
			}
		}
		else {
			m_lpDDPrimary->Blt(NULL, m_lpDDBack, NULL, DDBLT_WAIT, NULL);
		}
	}
}

BYTE	CDirectDraw::GetZapperHit()
{
	if (m_bZapper) {
		if (m_ZapperPosX >= 0 && m_ZapperPosX < SCREEN_WIDTH && m_ZapperPosY >= 0 && m_ZapperPosY < SCREEN_HEIGHT) {
			BYTE	c = m_lpRender[8 + m_ZapperPosX + RENDER_WIDTH * m_ZapperPosY];
			DWORD	Yn = (DWORD)(0.299f * m_cpPalette[0][c].rgbRed + 0.587f * m_cpPalette[0][c].rgbGreen + 0.114f * m_cpPalette[0][c].rgbBlue);
			if (Yn > 0xFF)
				Yn = 0xFF;
			return	(BYTE)Yn;
		}
	}
	return	0x00;
}

void	CDirectDraw::GetZapperPos(LONG& x, LONG& y)
{
	x = y = -1;

	if (!m_lpDD || !m_lpDDPrimary)
		return;
	// ScreenMode changing?
	if (m_bChangeMode)
		return;

	if (!m_bZapper)
		return;

	RECT	rcS, rcC;

	// Size calculate
	rcS.left = 0;
	rcS.right = SCREEN_WIDTH;
	if (!m_bAllLine) {
		rcS.top = 8;
		rcS.bottom = SCREEN_HEIGHT - 8;
	}
	else {
		rcS.top = 0;
		rcS.bottom = SCREEN_HEIGHT;
	}
	if (m_bScanlineMode) {
		rcS.top *= 2;
		rcS.bottom *= 2;
	}

	if (!m_bScreenMode) {
		// Window mode
		::GetClientRect(m_hWnd, &rcC);
		::ClientToScreen(m_hWnd, (POINT*)&rcC.left);
		::ClientToScreen(m_hWnd, (POINT*)&rcC.right);
	}
	else {
		// Fullscreen mode
		DDSURFACEDESC2	ddsd;
		ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize = sizeof(DDSURFACEDESC2);
		if (m_lpDDPrimary->GetSurfaceDesc(&ddsd) != DD_OK)
			return;

		if (!m_bMaxZoom) {
			// Position offset caluclate
			LONG	swidth, sheight;
			LONG	dwidth, dheight;
			LONG	hmul, vmul;

			if (!m_bAspect)  swidth = SCREEN_WIDTH;
			else		  swidth = 320;
			if (!m_bAllLine) sheight = SCREEN_HEIGHT - 16;
			else		  sheight = SCREEN_HEIGHT;

			dwidth = (LONG)ddsd.dwWidth;
			dheight = (LONG)ddsd.dwHeight;
			hmul = dwidth / swidth;
			vmul = dheight / sheight;

			if (vmul < hmul) hmul = vmul;
			else		  vmul = hmul;

			rcC.left = (dwidth - swidth * hmul) / 2;
			rcC.top = (dheight - sheight * vmul) / 2;
			rcC.right = rcC.left + swidth * hmul;
			rcC.bottom = rcC.top + sheight * vmul;
		}
		else {
			// Maximum zoom
			rcC.left = 0;
			rcC.top = 0;
			rcC.right = (LONG)ddsd.dwWidth;
			rcC.bottom = (LONG)ddsd.dwHeight;
		}

	}

	POINT	mp;
	FLOAT	hz, vz;

	::GetCursorPos(&mp);

	if (mp.x >= rcC.left && mp.x < rcC.right
		&& mp.y >= rcC.top && mp.y < rcC.bottom) {
		hz = (FLOAT)(rcS.right - rcS.left) / (rcC.right - rcC.left);
		vz = (FLOAT)(rcS.bottom - rcS.top) / (rcC.bottom - rcC.top);
		x = (LONG)((mp.x - rcC.left) * hz) + rcS.left;
		if (!m_bScanlineMode) {
			y = (LONG)((mp.y - rcC.top) * vz) + rcS.top;
		}
		else {
			y = (LONG)(((mp.y - rcC.top) * vz) + rcS.top) / 2;
		}

		if (x > SCREEN_WIDTH - 1)
			x = SCREEN_WIDTH - 1;
		if (y > SCREEN_HEIGHT - 1)
			y = SCREEN_HEIGHT - 1;
	}
	else {
		x = y = -1;
	}

	m_ZapperPosX = x;
	m_ZapperPosY = y;
}

void	CDirectDraw::SetZapperPos(LONG x, LONG y)
{
	m_ZapperPosX = x;
	m_ZapperPosY = y;
}

// Infomation string
void	CDirectDraw::SetInfoString(LPCSTR str)
{
	if (str) {
		if (strlen(str) > INFOSTR_SIZE) {
			memcpy(m_szInfo, str, INFOSTR_SIZE);
			m_szInfo[INFOSTR_SIZE] = '\0';
		}
		else {
			strcpy(m_szInfo, str);
		}
	}
	else {
		m_szInfo[0] = '\0';
	}
}

// Message string
void	CDirectDraw::SetMessageString(LPCSTR str)
{
	if (str) {
		if (strlen(str) > INFOSTR_SIZE) {
			memcpy(m_szMess, str, INFOSTR_SIZE);
			m_szMess[INFOSTR_SIZE] = '\0';
		}
		else {
			strcpy(m_szMess, str);
		}
		m_bMessage = TRUE;
		m_dwMessageTime = ::timeGetTime();
	}
	else {
		m_bMessage = FALSE;
		m_szMess[0] = '\0';
	}
}

void	CDirectDraw::RenderString(INT x, INT y, LPCSTR str)
{
	if (!m_lpDD || !m_lpDDPrimary)
		return;
	// ScreenMode changing?
	if (m_bChangeMode)
		return;

	if (!str)
		return;

	// Size calculate
	BOOL	bDoubleWidth = FALSE;
	BOOL	bDoubleHeight = FALSE;
	if (m_bDoubleSize || (m_nBltFilter && IsMMX())) {
		bDoubleWidth = TRUE;
		bDoubleHeight = TRUE;
	}
	else if (m_bScanlineMode) {
		bDoubleHeight = TRUE;
	}

	RECT	rcS, rcW;
	INT	ch;
	INT	xadd;

	rcW.left = x;
	rcW.top = y;
	if (!bDoubleWidth) {
		rcW.right = x + 6;
		xadd = 6;
	}
	else {
		rcW.right = x + 12;
		xadd = 12;
	}
	if (!bDoubleHeight) {
		rcW.bottom = y + 6;
	}
	else {
		rcW.bottom = y + 12;
	}

	while (*str) {
		ch = toupper(*str) - 0x20;
		rcS.left = (ch % 8) * 6;
		rcS.right = rcS.left + 6;
		rcS.top = (ch / 8) * 6;
		rcS.bottom = rcS.top + 6;

		m_lpDDRender->Blt(&rcW, m_lpDDAscii, &rcS, DDBLT_KEYSRC, NULL);

		rcW.left += xadd;
		rcW.right += xadd;
		str++;
	}
}

// #### Copy from LZSS BMP to surface.
void	CDirectDraw::SetLZSSChar(LPBYTE lpLZ, LPDIRECTDRAWSURFACE7 lpDDSurface)
{
	LPBYTE	lpBuf = NULL;
	LPBITMAPINFOHEADER pbi;
	LPBYTE	lpPix;
	HDC	hDC;

	if (!(lpBuf = (LPBYTE)malloc(*((LONG*)lpLZ))))
		throw	"Out of memory.";

	LZdecode(&lpLZ[8], lpBuf, *((LONG*)&lpLZ[4]));

	pbi = (LPBITMAPINFOHEADER)lpBuf;
	if (pbi->biBitCount < 16)
		lpPix = ((LPBYTE)pbi) + pbi->biSize + (1LL << pbi->biBitCount) * sizeof(RGBQUAD);
	else
		lpPix = (LPBYTE)pbi + pbi->biSize;

	DDSURFACEDESC2	ddsd;
	ZEROMEMORY(&ddsd, sizeof(DDSURFACEDESC2));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	lpDDSurface->GetSurfaceDesc(&ddsd);

	// #### BMP has a negative height.
	LONG	height = pbi->biHeight;
	if (height < 0)
		height = -height;

	if (lpDDSurface->GetDC(&hDC) == DD_OK) {
		StretchDIBits(hDC, 0, 0, ddsd.dwWidth, ddsd.dwHeight,
			0, 0, pbi->biWidth, height, lpPix, (LPBITMAPINFO)lpBuf, DIB_RGB_COLORS, SRCCOPY);
		lpDDSurface->ReleaseDC(hDC);
	}
	FREE(lpBuf);
}

BYTE	CDirectDraw::GetPixel(INT x, INT y)
{
	if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
		BYTE	c = m_lpRender[8 + (x)+RENDER_WIDTH * y];
		return c;
	}

	return 0x00;
}

//#include "Render.h"
