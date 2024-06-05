//////////////////////////////////////////////////////////////////////////
// MapperNSF  NSF:NES Sound Format                                      //
//////////////////////////////////////////////////////////////////////////
#include "math.h"

#define	NSF_PROFILE	0

#if	NSF_PROFILE
static	DWORD	avecount = 0;
static	float	tave = 0.0f;
static	float	pave = 0.0f;
static	float	ptave = 0.0f;
static	DWORD	maxtotalclk = 0;
static	DWORD	maxprofclk = 0;
static	DWORD	maxproftotalclk = 0;
static	DWORD	maxproftotalcnt = 0;
#endif

BYTE	MapperNSF::Font6x8[] = {
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x20,0x20,0x20,0x20,0x00,0x20,0x00,
	0x50,0x50,0x00,0x00,0x00,0x00,0x00,0x00,0x50,0x50,0xF8,0x50,0xF8,0x50,0x50,0x00,
	0x20,0x78,0xA0,0x70,0x28,0xF0,0x20,0x00,0x48,0xB0,0x50,0x20,0x50,0x68,0x90,0x00,
	0x40,0xA0,0xA8,0x68,0x90,0x90,0x68,0x00,0x30,0x20,0x00,0x00,0x00,0x00,0x00,0x00,
	0x10,0x20,0x40,0x40,0x40,0x20,0x10,0x00,0x40,0x20,0x10,0x10,0x10,0x20,0x40,0x00,
	0x00,0x88,0x50,0x20,0x50,0x88,0x00,0x00,0x00,0x20,0x20,0xF8,0x20,0x20,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x20,0x40,0x00,0x00,0x00,0x00,0xF8,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x20,0x00,0x08,0x10,0x10,0x20,0x40,0x40,0x80,0x00,
	0x70,0x88,0x98,0xA8,0xC8,0x88,0x70,0x00,0x20,0x60,0x20,0x20,0x20,0x20,0xF8,0x00,
	0x70,0x88,0x08,0x30,0x40,0x80,0xF8,0x00,0x70,0x88,0x08,0x30,0x08,0x88,0x70,0x00,
	0x30,0x50,0x90,0x90,0xF8,0x10,0x10,0x00,0xF8,0x80,0x80,0xF0,0x08,0x08,0xF0,0x00,
	0x70,0x88,0x80,0xF0,0x88,0x88,0x70,0x00,0xF8,0x08,0x10,0x10,0x20,0x20,0x20,0x00,
	0x70,0x88,0x88,0x70,0x88,0x88,0x70,0x00,0x70,0x88,0x88,0x78,0x08,0x88,0x70,0x00,
	0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x00,0x00,0x00,0x20,0x40,0x00,
	0x10,0x20,0x40,0x80,0x40,0x20,0x10,0x00,0x00,0x00,0xF8,0x00,0xF8,0x00,0x00,0x00,
	0x40,0x20,0x10,0x08,0x10,0x20,0x40,0x00,0x70,0x88,0x08,0x10,0x20,0x00,0x20,0x00,
	0x30,0x48,0x88,0x98,0xA8,0xA8,0x78,0x00,0x20,0x50,0x50,0x88,0xF8,0x88,0x88,0x00,
	0xF0,0x88,0x88,0xF0,0x88,0x88,0xF0,0x00,0x70,0x88,0x80,0x80,0x80,0x88,0x70,0x00,
	0xF0,0x88,0x88,0x88,0x88,0x88,0xF0,0x00,0xF8,0x80,0x80,0xF0,0x80,0x80,0xF8,0x00,
	0xF8,0x80,0x80,0xF0,0x80,0x80,0x80,0x00,0x70,0x88,0x80,0xB8,0x88,0x88,0x70,0x00,
	0x88,0x88,0x88,0xF8,0x88,0x88,0x88,0x00,0xF8,0x20,0x20,0x20,0x20,0x20,0xF8,0x00,
	0x38,0x08,0x08,0x08,0x08,0x88,0x70,0x00,0x88,0x88,0x90,0xE0,0x90,0x88,0x88,0x00,
	0x80,0x80,0x80,0x80,0x80,0x80,0xF8,0x00,0x88,0xD8,0xA8,0xA8,0xA8,0xA8,0xA8,0x00,
	0x88,0xC8,0xA8,0xA8,0xA8,0x98,0x88,0x00,0x70,0x88,0x88,0x88,0x88,0x88,0x70,0x00,
	0xF0,0x88,0x88,0xF0,0x80,0x80,0x80,0x00,0x70,0x88,0x88,0x88,0xA8,0x90,0x68,0x00,
	0xF0,0x88,0x88,0xF0,0x88,0x88,0x88,0x00,0x70,0x88,0x80,0x70,0x08,0x88,0x70,0x00,
	0xF8,0x20,0x20,0x20,0x20,0x20,0x20,0x00,0x88,0x88,0x88,0x88,0x88,0x88,0x70,0x00,
	0x88,0x88,0x88,0x50,0x50,0x50,0x20,0x00,0x88,0xA8,0xA8,0xA8,0xA8,0xD8,0x88,0x00,
	0x88,0x88,0x50,0x20,0x50,0x88,0x88,0x00,0x88,0x88,0x88,0x70,0x20,0x20,0x20,0x00,
	0xF8,0x08,0x10,0x20,0x40,0x80,0xF8,0x00,0x70,0x40,0x40,0x40,0x40,0x40,0x70,0x00,
	0x88,0x50,0xF8,0x20,0xF8,0x20,0x20,0x00,0x70,0x10,0x10,0x10,0x10,0x10,0x70,0x00,
	0x20,0x50,0x88,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xF8,0x00,
	0x80,0xC0,0xE0,0xF0,0xE0,0xC0,0x80,0x00,0x00,0x00,0x70,0x08,0x78,0x88,0xF8,0x00,
	0x80,0x80,0x80,0xF0,0x88,0x88,0xF0,0x00,0x00,0x00,0x78,0x80,0x80,0x80,0x78,0x00,
	0x08,0x08,0x08,0x78,0x88,0x88,0x78,0x00,0x00,0x00,0x70,0x88,0xF8,0x80,0x78,0x00,
	0x18,0x20,0xF8,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x78,0x88,0x78,0x08,0xF0,0x00,
	0x80,0x80,0x80,0xF0,0x88,0x88,0x88,0x00,0x20,0x00,0x20,0x20,0x20,0x20,0x20,0x00,
	0x20,0x00,0x20,0x20,0x20,0x20,0xC0,0x00,0x80,0x80,0x88,0x90,0xE0,0x90,0x88,0x00,
	0x20,0x20,0x20,0x20,0x20,0x20,0x30,0x00,0x00,0x00,0xF0,0xA8,0xA8,0xA8,0xA8,0x00,
	0x00,0x00,0xF0,0x88,0x88,0x88,0x88,0x00,0x00,0x00,0x70,0x88,0x88,0x88,0x70,0x00,
	0x00,0x00,0xF0,0x88,0xF0,0x80,0x80,0x00,0x00,0x00,0x78,0x88,0x78,0x08,0x08,0x00,
	0x00,0x00,0xB8,0xC0,0x80,0x80,0x80,0x00,0x00,0x00,0x78,0x80,0x70,0x08,0xF0,0x00,
	0x20,0x20,0xF8,0x20,0x20,0x20,0x20,0x00,0x00,0x00,0x88,0x88,0x88,0x88,0x70,0x00,
	0x00,0x00,0x88,0x88,0x50,0x50,0x20,0x00,0x00,0x00,0x88,0xA8,0xA8,0xD8,0x88,0x00,
	0x00,0x00,0x88,0x50,0x20,0x50,0x88,0x00,0x00,0x00,0x88,0x88,0x78,0x08,0xF0,0x00,
	0x00,0x00,0xF8,0x08,0x70,0x80,0xF8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
	0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

BYTE	MapperNSF::KeyBoardBitmap[] = {
	48, 26,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x10, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x20,
	0x0F, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20,
	0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x10,
	0x10, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x20,
	0x20, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20,
	0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20,
	0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x0F, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
	0x10, 0x20, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x20,
	0x20, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x20, 0x20, 0x20,
	0x20, 0x10, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10, 0x0F, 0x10, 0x20, 0x20, 0x20, 0x20, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10,
	0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10,
	0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10,
};

// KeyA(6x26)
BYTE	MapperNSF::KeyBitmapA[] = {
	6, 26,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x2A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A, 0x0F, 0x0F, 0x2A, 0x2A, 0x2A, 0x2A, 0x0F, 0x0F,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x1A, 0x2A, 0x2A, 0x2A, 0x2A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
};
// KeyB(6x26)
BYTE	MapperNSF::KeyBitmapB[] = {
	6, 26,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF, 0xFF, 0xFF, 0x1A, 0x2A, 0xFF, 0xFF,
	0x0F, 0x0F, 0x1A, 0x2A, 0x0F, 0x0F, 0x0F, 0x0F, 0x2A, 0x2A, 0x0F, 0x0F,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x1A, 0x2A, 0x2A, 0x2A, 0x2A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
};
// KeyC(6x26)
BYTE	MapperNSF::KeyBitmapC[] = {
	6, 26,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A, 0xFF, 0xFF, 0x1A, 0x2A, 0x2A, 0x2A,
	0x0F, 0x0F, 0x1A, 0x2A, 0x2A, 0x2A, 0x0F, 0x0F, 0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A, 0x2A,
	0x1A, 0x2A, 0x2A, 0x2A, 0x2A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
};
// KeyD(5x16)
BYTE	MapperNSF::KeyBitmapD[] = {
	5, 16,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x2A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x2A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x2A, 0x1A, 0x1A, 0x1A,
	0x1A, 0x2A, 0x2A, 0x2A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A, 0x1A,
};

// #### fluorescent light
BYTE	MapperNSF::NeonBitmapA[] = {
	6, 26,
	0xFF, 0x02, 0x12, 0x12, 0x02, 0xFF, 0x02, 0x12, 0x21, 0x21, 0x12, 0x02,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x12, 0x21, 0x31, 0x31, 0x21, 0x12, 0x12, 0x21, 0x31, 0x31, 0x21, 0x12,
	0x02, 0x12, 0x21, 0x21, 0x12, 0x02, 0xFF, 0x02, 0x12, 0x12, 0x02, 0xFF,
};

BYTE	MapperNSF::NeonBitmapB[] = {
	6, 26,
	0xFF, 0x09, 0x1A, 0x1A, 0x09, 0xFF, 0x09, 0x1A, 0x2A, 0x2A, 0x1A, 0x09,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A, 0x1A, 0x2A, 0x3B, 0x3B, 0x2A, 0x1A,
	0x09, 0x1A, 0x2A, 0x2A, 0x1A, 0x09, 0xFF, 0x09, 0x1A, 0x1A, 0x09, 0xFF,
};

BYTE	MapperNSF::NeonBitmapC[] = {
	6, 26,
	0xFF, 0x1E, 0x1D, 0x1D, 0x1E, 0xFF, 0x1E, 0x1D, 0x2D, 0x2D, 0x1D, 0x1E,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D, 0x1D, 0x2D, 0x00, 0x00, 0x2D, 0x1D,
	0x1E, 0x1D, 0x2D, 0x2D, 0x1D, 0x1E, 0xFF, 0x1E, 0x1D, 0x1D, 0x1E, 0xFF,
};

INT	MapperNSF::KeyBitmapOfs[] = {
	0, 4, 7, 11, 14, 21, 25, 28, 32, 35, 39, 42,
};

LPBYTE	MapperNSF::KeyBitmapTbl[] = {
	MapperNSF::KeyBitmapA,
	MapperNSF::KeyBitmapD,
	MapperNSF::KeyBitmapB,
	MapperNSF::KeyBitmapD,
	MapperNSF::KeyBitmapC,
	MapperNSF::KeyBitmapA,
	MapperNSF::KeyBitmapD,
	MapperNSF::KeyBitmapB,
	MapperNSF::KeyBitmapD,
	MapperNSF::KeyBitmapB,
	MapperNSF::KeyBitmapD,
	MapperNSF::KeyBitmapC,
};

BYTE	MapperNSF::StarBitmapA[] = {
	5, 5,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x2D, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
BYTE	MapperNSF::StarBitmapB[] = {
	5, 5,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x1D, 0xFF, 0xFF,
	0xFF, 0x1D, 0x2D, 0x1D, 0xFF,
	0xFF, 0xFF, 0x1D, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
BYTE	MapperNSF::StarBitmapC[] = {
	5, 5,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0x2D, 0xFF, 0xFF,
	0xFF, 0x2D, 0x10, 0x2D, 0xFF,
	0xFF, 0xFF, 0x2D, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
};
BYTE	MapperNSF::StarBitmapD[] = {
	5, 5,
	0xFF, 0xFF, 0x2D, 0xFF, 0xFF,
	0xFF, 0x2D, 0x10, 0x2D, 0xFF,
	0x2D, 0x10, 0x20, 0x10, 0x2D,
	0xFF, 0x2D, 0x10, 0x2D, 0xFF,
	0xFF, 0xFF, 0x2D, 0xFF, 0xFF,
};

LPBYTE	MapperNSF::StarBitmapTbl[] = {
	MapperNSF::StarBitmapA,
	MapperNSF::StarBitmapB,
	MapperNSF::StarBitmapC,
	MapperNSF::StarBitmapD,
};

void	MapperNSF::Reset()
{
INT	i;

	songno = 0;

	pad = padold = 0;
	repcnt = 0;

	BGPAL[0] = BGPAL[1] = BGPAL[2] = BGPAL[3] = 
	SPPAL[0] = SPPAL[1] = SPPAL[2] = SPPAL[3] = 0x0F;

	exaddr = 0;
	ZEROMEMORY( exram, sizeof(exram) );

	SetPROM_Bank( 2, WRAM+0xA000, BANKTYPE_RAM );	// 4000-5FFF

	SetPROM_Bank( 3, WRAM+0x0000, BANKTYPE_RAM );	// 6000-7FFF
	SetPROM_Bank( 4, WRAM+0x2000, BANKTYPE_RAM );	// 8000-9FFF
	SetPROM_Bank( 5, WRAM+0x4000, BANKTYPE_RAM );	// A000-BFFF
	SetPROM_Bank( 6, WRAM+0x6000, BANKTYPE_RAM );	// C000-DFFF
	SetPROM_Bank( 7, WRAM+0x8000, BANKTYPE_RAM );	// E000-FFFF

	nsfheader = *(nes->rom->GetNsfHeader());
	exchip = nsfheader.ExtraChipSelect;
	banksize = nes->rom->GetNSF_SIZE();

	if( (songno = nsfheader.StartSong-1) >= nsfheader.TotalSong ) {
		songno = 0;
	}

	bankswitch = 0;
	for( i = 0; i < 8; i++ ) {
		bankswitch |= nsfheader.BankSwitch[i];
	}

	if( bankswitch ) {
	// Bankswitch on
		BYTE	start_bank = nsfheader.LoadAddress>>12;
		for( i = 0; (start_bank+i) < 8; i++ ) {
			BankSwitch( start_bank+i, i );
		}
		for( i = 0; i < 8; i++ ) {
			BankSwitch( i+8, nsfheader.BankSwitch[i] );
		}
		if( exchip&0x04 ) {
			BankSwitch( 6, nsfheader.BankSwitch[6] );
			BankSwitch( 7, nsfheader.BankSwitch[7] );
		}
	} else {
	// Bankswitch off
		LPBYTE	pPtr = nes->rom->GetPROM();
		if( banksize < 8 ) {
			for( i = 0; i < 0x1000*banksize; i++ ) {
				if( nsfheader.LoadAddress-0x8000+i < 0x8000 ) {
					WRAM[0x2000+nsfheader.LoadAddress-0x8000+i] = pPtr[i];
				}
			}
		} else {
			for( i = 0; i < 0x8000; i++ ) {
				if( nsfheader.LoadAddress-0x8000+i < 0x8000 ) {
					WRAM[0x2000+nsfheader.LoadAddress-0x8000+i] = pPtr[i];
				}
			}
		}
	}

	// #### $4700 is stuck in an infinite loop.
	WRAM[0xA700] = 0x4c;	//jmp
	WRAM[0xA701] = 0x00;
	WRAM[0xA702] = 0x47;

	// #### $4710 is calling the `init` loop after entering an infinite loop.
	WRAM[0xA710] = 0x20;	// jsr
	WRAM[0xA711] = (BYTE) nsfheader.InitAddress & 0xFF;
	WRAM[0xA712] = (BYTE)(nsfheader.InitAddress>>8);
	WRAM[0xA713] = 0x4c;	// jmp
	WRAM[0xA714] = 0x00;
	WRAM[0xA715] = 0x47;
	
	// #### endless loop
#if	NSF_PROFILE
	WRAM[0xA720] = 0x8D;	// sta
	WRAM[0xA721] = 0x1E;	// $401E
	WRAM[0xA722] = 0x40;	// $401E

	WRAM[0xA723] = 0x20;	// jsr
	WRAM[0xA724] = (BYTE) nsfheader.PlayAddress;
	WRAM[0xA725] = (BYTE)(nsfheader.PlayAddress>>8);

	WRAM[0xA726] = 0x8D;	// sta
	WRAM[0xA727] = 0x1F;	// $401F
	WRAM[0xA728] = 0x40;	// $401F

	WRAM[0xA729] = 0x4c;	//jmp
	WRAM[0xA72A] = 0x00;
	WRAM[0xA72B] = 0x47;
#else
	WRAM[0xA720] = 0x20;	// jsr
	WRAM[0xA721] = (BYTE) nsfheader.PlayAddress;
	WRAM[0xA722] = (BYTE)(nsfheader.PlayAddress>>8);
	WRAM[0xA723] = 0x4c;	//jmp
	WRAM[0xA724] = 0x00;
	WRAM[0xA725] = 0x47;
#endif

	nes->apu->SelectExSound( exchip );
	nes->apu->Write( 0x4015, 0x1F );
	nes->apu->ExWrite( 0x4080, 0x80 );	// FDS Volume 0
	nes->apu->ExWrite( 0x408A, 0xE8 );	// FDS Envelope ON

	// For DQ1/2??
	nes->SetFrameIRQmode( FALSE );

	// NTSC/PAL tune
	nes->SetVideoMode( (nsfheader.NTSC_PALbits&0x01)?TRUE:FALSE );

	// #### Create a frequency table.
	for( i = 0; i < 12*8+1; i++ ) {
		Freq2KeyTbl[i] = (INT)(440.0*256.0*pow(2.0,((double)(i-45)-0.5)/12.0));
	}

	// #### extra
	StarInitial();

	//
#if	NSF_PROFILE
	avecount = 0;
	tave = 0.0f;
	pave = 0.0f;
	ptave = 0.0f;
	maxtotalclk = 0;
	maxprofclk = 0;
	maxproftotalclk = 0;
	maxproftotalcnt = 0;
#endif
}

BYTE	MapperNSF::ExRead( WORD addr )
{
BYTE	data = 0;

	if( addr >= 0x4040 && addr < 0x4100 ) {
		data = nes->apu->ExRead( addr );
	}
	return	data;
}

void	MapperNSF::ExWrite( WORD addr, BYTE data )
{
	if( addr >= 0x4040 && addr < 0x4100 ) {
		nes->apu->ExWrite( addr, data );
	}
}

BYTE	MapperNSF::ReadLow( WORD addr )
{
	if( addr == 0x4800 ) {
		BYTE	data = exram[exaddr&0x7F];
		if( exaddr&0x80 )
			exaddr = (exaddr+1)|0x80;
		nes->apu->ExRead( addr );
		return	data;
	} else if( addr == 0x5205 ) {	// MMC5
		return	(BYTE)multipul[0]*multipul[1];
	} else if( addr == 0x5206 ) {	// MMC5
		return	(BYTE)(((WORD)multipul[0]*(WORD)multipul[1])>>8);
	}
//	if( addr >= 0x47A0 && addr < 0x4800 ) {
//		return	WRAM[addr+0x6000];
//	}
	if( addr >= 0x5C00 && addr < 0x5FFF ) {	// MMC5
		return	DRAM[addr-0x5C00];
	}
	if( addr >= 0x6000 ) {
		return	WRAM[addr-0x6000];
	}

	return	(BYTE)(addr>>8);
}

void	MapperNSF::WriteLow( WORD addr, BYTE data )
{
	if( addr == 0x4800 || (addr >= 0x5000 && addr <= 0x5015) ) {
//DEBUGOUT( "$4800 WR=%02X\n", data );
		exram[exaddr&0x7F] = data;
		if( exaddr&0x80 )
			exaddr = (exaddr+1)|0x80;
		nes->apu->ExWrite( addr, data );
	} else if( addr == 0x5205 ) {	// MMC5
		multipul[0] = data;
	} else if( addr == 0x5206 ) {	// MMC5
		multipul[1] = data;
	} else if( addr >= 0x5C00 && addr < 0x5FF6 ) {	// MMC5
		DRAM[addr-0x5C00] = data;
	}
//	if( addr >= 0x47A0 && addr < 0x4800 ) {
//		WRAM[addr+0x6000] = data;
//	}
	if( addr >= 0x5FF6 && addr <= 0x5FFF ) {
	// Bank switch
		BankSwitch( addr&0x0F, data );
	}
	if( addr >= 0x6000 ) {
		WRAM[addr-0x6000] = data;

		nes->apu->ExWrite( addr, data );
	}
}

void	MapperNSF::Write( WORD addr, BYTE data )
{
	if( (exchip&0x04) ) {
		WRAM[addr-0x6000] = data;
	}
	nes->apu->ExWrite( addr, data );
	if( addr == 0xF800 ) {
		exaddr = data;
	}
}

void	MapperNSF::BankSwitch( INT no, BYTE bank )
{
INT	i;
INT	addr, offs;
LPBYTE	pPtr;

	offs = 0x1000*(INT)bank-(INT)(nsfheader.LoadAddress&0x0FFF);
	pPtr = nes->rom->GetPROM();

	if( no == 6 || no == 7 ) {
		for( i = 0; i < 0x1000; i++ ) {
			addr = offs+i;
			if( addr < 0 || addr > (4096*banksize)-1 ) {
				WRAM[0x1000*(no&1)+i] = 0;
			} else {
				WRAM[0x1000*(no&1)+i] = pPtr[addr];
			}
		}
	} else if( no >= 8 && no <= 15 ) {
		for( i = 0; i < 0x1000; i++ ) {
			addr = offs+i;
			if( addr < 0 || addr > (4096*banksize)-1 ) {
				WRAM[0x2000+0x1000*(no&7)+i] = 0;
			} else {
				WRAM[0x2000+0x1000*(no&7)+i] = pPtr[addr];
			}
		}
	}
}

void	MapperNSF::VSync()
{
	// #### Data Settings
	padold = pad;
	pad = nes->pad->GetNsfController();
	BYTE	padpush = ~padold & pad;

	if( padpush ) {
		repcnt = 20;
	} else if( pad ) {
		if( --repcnt == 0 ) {
			repcnt = 8;
			padpush |= pad & 0xFC;
		}
	}

	// -1
	if( padpush & (1<<2) ) {
		if( --songno < 0 ) {
			songno = nsfheader.TotalSong-1;
		}
	}
	// +1
	if( padpush & (1<<3) ) {
		if( ++songno >= nsfheader.TotalSong ) {
			songno = 0;
		}
	}
	// -16
	if( padpush & (1<<4) ) {
		if( (songno-=16) < 0 ) {
			while( songno < 0 ) {
				songno += nsfheader.TotalSong;
			}
			songno %= nsfheader.TotalSong;
		}
	}
	// +16
	if( padpush & (1<<5) ) {
		if( (songno+=16) >= nsfheader.TotalSong ) {
			songno -= nsfheader.TotalSong;
			songno %= nsfheader.TotalSong;
		}
	}

	// Play
	if( padpush & (1<<0) ) {
		nes->SetNsfPlay( songno, 0 );
	}

	// Stop
	if( padpush & (1<<1) ) {
		nes->SetNsfStop();
	}

	// #### additional gift
	Star();

	// Drawing screen
//	DrawString( 10, 16, "TITLE :", 0x30 );
//	DrawString( 10, 31, "ARTIST:", 0x30 );
//	DrawString( 10, 46, "(C)   :", 0x30 );
//	DrawString( 10, 61, "NO    :", 0x30 );

	DrawString( 10, 16, "Title :", 0x30 );
	DrawString( 10, 31, "Artist:", 0x30 );
	DrawString( 10, 46, "(C)   :", 0x30 );
	DrawString( 10, 61, "No    :", 0x30 );

	DrawString( 28,  76, "Keyboard", 0x30 );
	DrawString( 32, 152, "Wave View", 0x30 );

	// #### Title 
	CHAR	str[64];
	memcpy( str, nsfheader.SongName, 32 );
	str[32] = 0;
	DrawString(  53,  16, str, 0x30 );
	memcpy( str, nsfheader.ArtistName, 32 );
	str[32] = 0;
	DrawString(  53,  31, str, 0x30 );
	memcpy( str, nsfheader.CopyrightName, 32 );
	str[32] = 0;
	DrawString(  53,  46, str, 0x30 );
	// Song no
	sprintf( str, "%02X", songno );
	DrawString(  53,  61, str, 0x30 );

	// Extra sound
	DrawString( 76+6* 0,  61, "VRC6", (exchip&0x01)?0x2B:0x2D );
	DrawString( 76+6* 5,  61, "VRC7", (exchip&0x02)?0x2B:0x2D );
	DrawString( 76+6*10,  61, "FDS",  (exchip&0x04)?0x2B:0x2D );
	DrawString( 76+6*14,  61, "MMC5", (exchip&0x08)?0x2B:0x2D );
	DrawString( 76+6*19,  61, "N106", (exchip&0x10)?0x2B:0x2D );
//	DrawString( 76+6*24,  61, "FME7", (exchip&0x20)?0x2B:0x2D );
	DrawString( 76+6*24,  61, "SN5B", (exchip&0x20)?0x2B:0x2D );

	// #### Title and other lines
	DrawRect(  52, 24, 243-52,   0, 0x30 );
	DrawRect(  52, 39, 243-52,   0, 0x30 );
	DrawRect(  52, 54, 243-52,   0, 0x30 );
	DrawRect(  52, 69, 12,       0, 0x30 );
	DrawRect(  75, 69, 243-75,   0, 0x30 );

	// #### Keyboard frame
	DrawRect(  27,  84, 229-27, 148- 84, 0x30 );

	// #### WAVEVIEW frame
#if	!NSF_PROFILE
	DrawRect(  31, 160, 224-31, 224-160, 0x30 );
	DrawRect(  32, 192, 223-32, 0,       0x27 );

	// Wave
	DrawWave( 28+4, 192, 0x2A );
#endif
	// #### Top Row Keys (O5-O8)
	DrawBitmap( 31+49*0, 88, KeyBoardBitmap );
	DrawBitmap( 31+49*1, 88, KeyBoardBitmap );
	DrawBitmap( 31+49*2, 88, KeyBoardBitmap );
	DrawBitmap( 31+49*3, 88, KeyBoardBitmap );
	// #### Keyboard lower row (O1-O4):  * O1: Number pad * O2: Enter key * O3: Space bar * O4: Page up/down keys
	DrawBitmap( 31+49*0, 119, KeyBoardBitmap );
	DrawBitmap( 31+49*1, 119, KeyBoardBitmap );
	DrawBitmap( 31+49*2, 119, KeyBoardBitmap );
	DrawBitmap( 31+49*3, 119, KeyBoardBitmap );

	// #### The keyboard is being typed on.
	// Internal APU
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0000)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0001)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0002)) );

	// Internal Noise
	if( nes->apu->GetChannelFrequency(0x0003) ) {
		DrawBitmap(  10,  88, NeonBitmapA );
		DrawBitmap( 241,  88, NeonBitmapA );
		DrawBitmap(  10, 119, NeonBitmapA );
		DrawBitmap( 241, 119, NeonBitmapA );
	} else {
		DrawBitmap(  10,  88, NeonBitmapC );
		DrawBitmap( 241,  88, NeonBitmapC );
		DrawBitmap(  10, 119, NeonBitmapC );
		DrawBitmap( 241, 119, NeonBitmapC );
	}

	// Internal DPCM
	if( nes->apu->GetChannelFrequency(0x0004) ) {
		DrawBitmap(  18,  88, NeonBitmapB );
		DrawBitmap( 233,  88, NeonBitmapB );
		DrawBitmap(  18, 119, NeonBitmapB );
		DrawBitmap( 233, 119, NeonBitmapB );
	} else {
		DrawBitmap(  18,  88, NeonBitmapC );
		DrawBitmap( 233,  88, NeonBitmapC );
		DrawBitmap(  18, 119, NeonBitmapC );
		DrawBitmap( 233, 119, NeonBitmapC );
	}

	// VRC6
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0100)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0101)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0102)) );

	// VRC7
	// Nothing...

	// FDS
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0300)) );

	// MMC5
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0400)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0401)) );

	// N106
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0500)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0501)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0502)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0503)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0504)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0505)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0506)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0507)) );

	// FME7
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0600)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0601)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0602)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0610)) );	// Noise
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0611)) );	// Noise
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0612)) );	// Noise

	// VRC7
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0700)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0701)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0702)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0703)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0704)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0705)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0706)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0707)) );
	DrawKey( GetKeyTable(nes->apu->GetChannelFrequency(0x0708)) );

	// TEST
#if	NSF_PROFILE
	if( nes->IsNsfPlaying() ) {
		DWORD	total, profile;

		total = nes->GetFrameTotalCycles();
		profile = nes->GetProfileCycles();

		if( maxtotalclk < total ) {
			maxtotalclk = total;
		}
		if( maxprofclk < profile ) {
			maxprofclk = profile;
		}
		if( maxproftotalclk < nes->GetProfileTotalCycles() ) {
			maxproftotalclk = nes->GetProfileTotalCycles();
		}
		if( maxproftotalcnt < nes->GetProfileTotalCount() ) {
			maxproftotalcnt = nes->GetProfileTotalCount();
		}

		tave += total;
		pave += profile;
		ptave += nes->GetProfileTotalCycles();
		avecount++;

sprintf( str, "TOTAL CLK :%8d", total );
DrawString( 16,  160, str, 0x30 );
sprintf( str, "TOTAL MAX :%8d", maxtotalclk );
DrawString( 134,  160, str, 0x30 );
sprintf( str, "TOTAL AVE :%8.2f", tave / (float)avecount );
DrawString( 16,  168, str, 0x30 );


sprintf( str, "PROFILE   :%8d", profile );
DrawString( 16,  176, str, 0x30 );
sprintf( str, "PROFILEMAX:%8d", maxprofclk );
DrawString( 134,  176, str, 0x30 );
sprintf( str, "POFILE AVE:%8.2f", pave / (float)avecount );
DrawString( 16,  184, str, 0x30 );

sprintf( str, "PRF/TTL  %%:%8.2f", 100.0f * (float)profile / (float)total );
DrawString( 16,  192, str, 0x30 );

sprintf( str, "FRM-P TTL :%8d", nes->GetProfileTotalCycles() );
DrawString( 16,  200, str, 0x30 );
sprintf( str, "F-P TTLMAX:%8d", maxproftotalclk );
DrawString( 134,  200, str, 0x30 );

sprintf( str, "FRM-P CNT :%8d", nes->GetProfileTotalCount() );
DrawString( 16,  208, str, 0x30 );
sprintf( str, "F-P CNTMAX:%8d", maxproftotalcnt );
DrawString( 134,  208, str, 0x30 );

sprintf( str, "F P/T AVE :%8.2f", ptave / (float)avecount );
DrawString( 16,  216, str, 0x30 );

sprintf( str, "F PRF/TTL%%:%8.2f", 100.0f * (float)nes->GetProfileTotalCycles() / (float)total );
DrawString( 16,  224, str, 0x30 );

		if( avecount >= 60 ) {
			avecount = 0;
			tave = 0.0f;
			pave = 0.0f;
			ptave = 0.0f;
			maxtotalclk = 0;
			maxprofclk = 0;
			maxproftotalclk = 0;
			maxproftotalcnt = 0;
		}
	}
#endif
}

INT	MapperNSF::GetKeyTable( INT freq )
{
INT	ret = -1;

	if( freq < Freq2KeyTbl[0] )
		return	-1;

	for( INT i = 0; i < 12*8+1; i++ ) {
		if( freq < Freq2KeyTbl[i] )
			return	ret;
		ret = i;
	}
	return	-1;
}

void	MapperNSF::DrawKey( INT key )
{
	if( key < 0 || key > 12*8-1 )
		return;

	if( key < 12*4 ) {
		DrawBitmap( 31+(key/12)*49+KeyBitmapOfs[key%12], 119, KeyBitmapTbl[key%12] );
	} else {
		DrawBitmap( 31+((key-12*4)/12)*49+KeyBitmapOfs[key%12], 88, KeyBitmapTbl[key%12] );
	}
}

void	MapperNSF::DrawRect( INT x, INT y, INT w, INT h, BYTE col )
{
INT	i;
LPBYTE	pScn = nes->ppu->GetScreenPtr()+8;
LPBYTE	p0, p1;

	p0 = pScn+(256+16)*y+x;
	p1 = p0+(256+16)*h;
	for( i = 0; i < w+1; i++ ) {
		*p0++ = col;
		*p1++ = col;
	}
	p0 = pScn+(256+16)*y+x;
	p1 = p0+w;
	for( i = 0; i < h+1; i++ ) {
		*p0 = col;
		*p1 = col;
		p0 += (256+16);
		p1 += (256+16);
	}
}

void	MapperNSF::DrawFont( INT x, INT y, BYTE chr, BYTE col )
{
INT	i;
LPBYTE	pFnt;
LPBYTE	pPtr;
LPBYTE	pScn = nes->ppu->GetScreenPtr()+8;

	if( chr < 0x20 || chr > 0x7F )
		return;
	chr -= 0x20;
	pFnt = &Font6x8[chr*8];
	pPtr = pScn+(256+16)*y+x;
	for( i = 0; i < 8; i++ ) {
		if( pFnt[i] & 0x80 ) pPtr[0] = col;
		if( pFnt[i] & 0x40 ) pPtr[1] = col;
		if( pFnt[i] & 0x20 ) pPtr[2] = col;
		if( pFnt[i] & 0x10 ) pPtr[3] = col;
		if( pFnt[i] & 0x08 ) pPtr[4] = col;
		if( pFnt[i] & 0x04 ) pPtr[5] = col;
		pPtr += (256+16);
	}
}

void	MapperNSF::DrawString( INT x, INT y, LPSTR str, BYTE col )
{
	while( *str ) {
		DrawFont( x, y, *str, col );
		str++;
		x += 6;
	}
}

void	MapperNSF::DrawWave( INT x, INT y, INT col )
{
INT	i, j;
INT	yp, ypold;
LPBYTE	pScn = nes->ppu->GetScreenPtr()+8+(256+16)*y+x;
LPBYTE	pPtr;

	ypold = 0;
	LPSWORD	pBuffer = (LPSWORD)nes->apu->GetSoundBuffer();
	for( i = 0; i < 192; i++ ) {
		yp = -((INT)*pBuffer)/1024;
		if( yp < -31 ) yp = -31;
		if( yp >  31 ) yp =  31;
		if( abs( yp-ypold ) <= 1 || i == 0 ) {
			pPtr = pScn+yp*(256+16)+i;
			*pPtr = col;
		} else {
		// Line DDA
			INT	xpos, ypos;
			INT	sx, sy;
			xpos = 65536*(i-1)+32768;
			ypos = ypold;
			sy = yp-ypold;
			sx = 65536/abs(sy);
			for( j = 0; j <= abs(sy); j++ ) {
				pPtr = pScn+ypos*(256+16)+(xpos/65536);
				*pPtr = col;
				ypos += (sy>0)?1:-1;
				xpos += sx;
			}
		}
		ypold = yp;
		pBuffer++;
	}
}

void	MapperNSF::DrawBitmap( INT x, INT y, LPBYTE lpBitmap )
{
INT	i, j;
INT	h, v;
LPBYTE	pScn = nes->ppu->GetScreenPtr()+8+(256+16)*y+x;
LPBYTE	pPtr;

	h = (INT)*lpBitmap++;
	v = (INT)*lpBitmap++;

	for( j = 0; j < v; j++ ) {
		pPtr = pScn;
		for( i = 0; i < h; i++ ) {
			if( *lpBitmap != 0xFF ) {
				*pPtr = *lpBitmap;
			}
			lpBitmap++;
			pPtr++;
		}
		pScn += 256+16;
	}
}

void	MapperNSF::StarInitial()
{
	for( INT i = 0; i < STAR_MAX; i++ ) {
		StarBuf[i].x = ((rand()%(256-10))-(256-10)/2)*256;
		StarBuf[i].y = ((rand()%(240-10))-(240-10)/2)*256;
		StarBuf[i].z = (rand()%250)+5;
	}
}

void	MapperNSF::Star()
{
	INT	x, y, z;
	for( INT i = 0; i < STAR_MAX; i++ ) {
		if( --StarBuf[i].z < 5 ) {
		StarBuf[i].x = ((rand()%(256-10))-(256-10)/2)*256;
		StarBuf[i].y = ((rand()%(240-10))-(240-10)/2)*256;
			StarBuf[i].z = 255;
		}

		x = 256/2+(StarBuf[i].x-5/128)/StarBuf[i].z;
		y = 240/2+(StarBuf[i].y-5/128)/StarBuf[i].z;

		if( x < 3 || x >= 256-8 || y < 3 || y >= 240-8 ) {
			StarBuf[i].x = ((rand()%(256-10))-(256-10)/2)*256;
			StarBuf[i].y = ((rand()%(240-10))-(240-10)/2)*256;
			StarBuf[i].z = 255;
			continue;
		}

		if( StarBuf[i].z > 200 )
			z = 0;
		else if( StarBuf[i].z > 100 )
			z = 1;
		else if( StarBuf[i].z > 50 )
			z = 2;
		else
			z = 3;

		DrawBitmap( x, y, StarBitmapTbl[z] );
	}
}

