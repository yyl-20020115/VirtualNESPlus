//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Emulation core                                              //
//                                                           Norix      //
//                                               written     2001/02/22 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "typedef.h"
#include "macro.h"

#include "VirtualNESres.h"

#include "DebugOut.h"
#include "App.h"
#include "Pathlib.h"
#include "Config.h"
#include "Crclib.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "pad.h"
#include "rom.h"
#include "mapper.h"

#include "DirectDraw.h"
#include "DirectSound.h"
#include "DirectInput.h"

#include "pngwrite.h"

#define VIRTUALNES_MV_SIGN "VirtuaNES MV"
#define VIRTUALNES_ST_SIGN "VirtuaNES ST"
#define VIRTUALNES_DI_SIGN "VirtuaNES DI"


NESCONFIG NESCONFIG_NTSC = {
	21477270.0f,		// Base clock
	1789772.5f,		// Cpu clock

	262,			// Total scanlines

	1364,			// Scanline total cycles(15.75KHz)

	1024,			// H-Draw cycles
	340,			// H-Blank cycles
	4,			// End cycles

	1364*262,		// Frame cycles
	29830,			// FrameIRQ cycles

	60,			// Frame rate(Be originally 59.94Hz)
	1000.0f/60.0f		// Frame period(ms)
};

NESCONFIG NESCONFIG_PAL = {
//	21281364.0f,		// Base clock
	26601714.0f,		// Base clock
//	1773447.0f,		// Cpu clock
	1662607.125f,		// Cpu clock

	312,			// Total scanlines

//	1362,			// Scanline total cycles(15.625KHz)
	1278,			// Scanline total cycles(15.625KHz)

//	1024,			// H-Draw cycles
	960,			// H-Draw cycles
//	338,			// H-Blank cycles
	318,			// H-Blank cycles
	2,			// End cycles

//	1362*312,		// Frame cycles
	1278*312,		// Frame cycles
//	35469,			// FrameIRQ cycles
	33252,			// FrameIRQ cycles

	50,			// Frame rate(Hz)
	1000.0f/50.0f		// Frame period(ms)
};

// Pad disp
BYTE	NES::m_PadImg[]  = {
	28, 8,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
	0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x00,
	0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x0F,
	0x0F, 0x0F, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x00, 0x0F, 0x0F, 0x0F, 0x0F, 0x00,
	0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x0F, 0x00, 0x0F,
	0x0F, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00, 0x00, 0x0F, 0x0F, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};
BYTE	NES::m_KeyImg0[] = {
	2, 2,
	0x2A, 0x2A,
	0x2A, 0x2A,
};
BYTE	NES::m_KeyImg1[] = {
	3, 3,
	0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A,
};
BYTE	NES::m_KeyImg2[] = {
	4, 4,
	0xFF, 0x2A, 0x2A, 0xFF,
	0x2A, 0x2A, 0x2A, 0x2A,
	0x2A, 0x2A, 0x2A, 0x2A,
	0xFF, 0x2A, 0x2A, 0xFF,
};

//----------
#define	ASMVER	0

#if	ASMVER
static	INT	cache = 0;
static	UINT	cache_hiRem = 0;
#endif
//
//
//
NES::NES( const char* fname )
{
	DEBUGOUT( "VirtualNES - NES Emulator for Win32 by Norix (C)2001\n" );

	m_bDiskThrottle = FALSE;
	m_CommandRequest = 0;

	m_nSnapNo = 0;

	m_bNsfPlaying = FALSE;

	m_bMoviePlay = m_bMovieRec = FALSE;
	m_fpMovie = NULL;

	m_bTapePlay = m_bTapeRec = FALSE;
	m_fpTape = NULL;
	m_TapeCycles = 0.0;
	m_TapeIn = m_TapeOut = 0;

	m_bBarcode = FALSE;
	m_BarcodeOut = 0;
	m_BarcodePtr = 0;
	m_BarcodeCycles = 0;

	m_bBarcode2 = FALSE;

	m_TurboFileBank = 0;

	cpu = NULL;
	ppu = NULL;
	apu = NULL;
	rom = NULL;
	pad = NULL;
	mapper = NULL;

	SAVERAM_SIZE = 8*1024;	// 8K byte

	// IRQ type
	nIRQtype = 0;

	// FrameIRQ mode
	bFrameIRQ = TRUE;

	// NTSC/PAL VideoMode
	bVideoMode = FALSE;

	// Default config
	nescfg = &NESCONFIG_NTSC;

	// Cheat
	CheatInitial();

	// TEST
#if	NES_PROFILER
	m_dwTotalCycle = 0;
	m_dwTotalTempCycle = 0;
	m_dwProfileTotalCycle = 0;
	m_dwProfileTotalCount = 0;
	m_dwProfileCycle = 0;
	m_dwProfileTempCycle = 0;
	m_dwProfileAveCycle = 0;
	m_dwProfileMaxCycle = 0;
#endif

	//-------
#if	ASMVER
	cache = 0;
	cache_hiRem = 0;
#endif

	try {
		DEBUGOUT( "Allocating CPU..." );
		if( !(cpu = new CPU(this)) )
			throw	"Allocating CPU failed.";
		DEBUGOUT( "Ok.\n" );

		DEBUGOUT( "Allocating PPU..." );
		if( !(ppu = new PPU(this)) )
			throw	"Allocating PPU failed.";
		DEBUGOUT( "Ok.\n" );

		DEBUGOUT( "Allocating APU..." );
		if( !(apu = new APU(this)) )
			throw	"Allocating APU failed.";
		DEBUGOUT( "Ok.\n" );

		DEBUGOUT( "Allocating PAD..." );
		if( !(pad = new PAD(this)) )
			throw	"Allocating PAD failed.";
		DEBUGOUT( "Ok.\n" );

		DEBUGOUT( "Loading ROM Image...\n" );

		if( !(rom = new ROM(fname)) )
			throw	"Allocating ROM failed.";

		if( !(mapper = CreateMapper(this, rom->GetMapperNo())) ) {
			// #### Unsupported Mapper
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_UNSUPPORTMAPPER );
			sprintf( szErrorString, szErrStr, rom->GetMapperNo() );
			throw	szErrorString;
		}

		DEBUGOUT( "Ok.\n" );

		DEBUGOUT( "ROM status\n" );
		DEBUGOUT( " %s\n", rom->GetRomName() );
		DEBUGOUT( " Mapper       : %03d\n", rom->GetMapperNo() );
		DEBUGOUT( " PRG SIZE     : %4dK\n", 16*(INT)rom->GetPROM_SIZE() );
		DEBUGOUT( " CHR SIZE     : %4dK\n",  8*(INT)rom->GetVROM_SIZE() );

		DEBUGOUT( " V MIRROR     : " );
		if( rom->IsVMIRROR() ) DEBUGOUT( "Yes\n" );
		else		       DEBUGOUT( "No\n" );
		DEBUGOUT( " 4 SCREEN     : " );
		if( rom->Is4SCREEN() ) DEBUGOUT( "Yes\n" );
		else		       DEBUGOUT( "No\n" );
		DEBUGOUT( " SAVE RAM     : " );
		if( rom->IsSAVERAM() ) DEBUGOUT( "Yes\n" );
		else		       DEBUGOUT( "No\n" );
		DEBUGOUT( " TRAINER      : " );
		if( rom->IsTRAINER() ) DEBUGOUT( "Yes\n" );
		else		       DEBUGOUT( "No\n" );
		DEBUGOUT( " VS-Unisystem : " );
		if( rom->IsVSUNISYSTEM() ) DEBUGOUT( "Yes\n" );
		else			   DEBUGOUT( "No\n" );

		NesSub_MemoryInitial();
		LoadSRAM();
		LoadDISK();

		{
		// #### Initialization timing
		DWORD	crc = rom->GetPROM_CRC();
		if( crc == 0xe792de94		// Best Play - Pro Yakyuu (New) (J)
		 || crc == 0xf79d684a		// Best Play - Pro Yakyuu (Old) (J)
		 || crc == 0xc2ef3422		// Best Play - Pro Yakyuu 2 (J)
		 || crc == 0x974e8840		// Best Play - Pro Yakyuu '90 (J)
		 || crc == 0xb8747abf		// Best Play - Pro Yakyuu Special (J)
		 || crc == 0x9fa1c11f		// Castle Excellent (J)
		 || crc == 0x0b0d4d1b		// Derby Stallion - Zenkoku Ban (J)
		 || crc == 0x728c3d98		// Downtown - Nekketsu Monogatari (J)
		 || crc == 0xd68a6f33		// Dungeon Kid (J)
		 || crc == 0x3a51eb04		// Fleet Commander (J)
		 || crc == 0x7c46998b		// Haja no Fuuin (J)
		 || crc == 0x7e5d2f1a		// Itadaki Street - Watashi no Mise ni Yottette (J)
		 || crc == 0xcee5857b		// Ninjara Hoi! (J)
		 || crc == 0x50ec5e8b		// Wizardry - Legacy of Llylgamyn (J)
		 || crc == 0x343e9146		// Wizardry - Proving Grounds of the Mad Overlord (J)
		 || crc == 0x33d07e45 ) {	// Wizardry - The Knight of Diamonds (J)
			pad->SetExController( PAD::EXCONTROLLER_TURBOFILE );
		}
		}

		LoadTurboFile();

		// #### Windows 7
		if( rom->IsVSUNISYSTEM() ) {
		DWORD	crc = rom->GetPROM_CRC();

			m_VSDipValue = GetVSDefaultDipSwitchValue( crc );
			m_VSDipTable = FindVSDipSwitchTable( crc );

			//#include "VS_Setting.h"
			{
				switch (rom->GetPROM_CRC()) {
				case	0xeb2dba63:	// VS TKO Boxing
				case	0x9818f656:
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xed588f00:	// VS Duck Hunt
					pad->SetVSType(PAD::VS_TYPEZ);
					goto	vsexit;
				case	0x8c0c2df5:	// VS Top Gun
					ppu->SetVSSecurity(0x1B);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0x16d3f469:	// VS Ninja Jajamaru Kun (J)
					ppu->SetVSSecurity(0x1B);
					pad->SetVSType(PAD::VS_TYPE3);
					goto	vsexit;
				case	0x8850924b:	// VS Tetris
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0xcf36261e:	// VS Sky Kid
					pad->SetVSType(PAD::VS_TYPE3);
					goto	vsexit;
				case	0xe1aa8214:	// VS Star Luster
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xec461db9:	// VS Pinball
					ppu->SetVSColorMap(0);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xe528f651:	// VS Pinball (alt)
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0x17ae56be:	// VS Freedom Force
					//			ppu->SetVSColorMap( 0 );
					ppu->SetVSColorMap(4);
					pad->SetVSType(PAD::VS_TYPEZ);
					goto	vsexit;
				case	0xe2c0a2be:	// VS Platoon
					ppu->SetVSColorMap(0);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xff5135a3:	// VS Hogan's Alley
					ppu->SetVSColorMap(0);
					pad->SetVSType(PAD::VS_TYPEZ);
					goto	vsexit;
				case	0x70901b25:	// VS Slalom
					ppu->SetVSColorMap(1);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0x0b65a917:	// VS Mach Rider(Endurance Course)
				case	0x8a6a9848:
					ppu->SetVSColorMap(1);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xae8063ef:	// VS Mach Rider(Japan, Fighting Course)
					ppu->SetVSColorMap(0);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xcc2c4b5d:	// VS Golf
					ppu->SetVSColorMap(1);
					//			pad->SetVSType( PAD::VS_TYPE0 );
					pad->SetVSType(PAD::VS_TYPE6);
					goto	vsexit;
				case	0xa93a5aee:	// VS Stroke and Match Golf
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0x86167220:	// VS Lady Golf
					ppu->SetVSColorMap(1);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0xffbef374:	// VS Castlevania
					ppu->SetVSColorMap(1);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0x135adf7c:	// VS Atari RBI Baseball
					ppu->SetVSColorMap(2);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0xd5d7eac4:	// VS Dr. Mario
					ppu->SetVSColorMap(2);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0x46914e3e:	// VS Soccer
					ppu->SetVSColorMap(2);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0x70433f2c:	// VS Battle City
				case	0x8d15a6e6:	// VS bad .nes
					ppu->SetVSColorMap(2);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0x1e438d52:	// VS Goonies
					ppu->SetVSColorMap(2);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xcbe85490:	// VS Excitebike
					ppu->SetVSColorMap(2);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0x29155e0c:	// VS Excitebike (alt)
					ppu->SetVSColorMap(3);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0x07138c06:	// VS Clu Clu Land
					ppu->SetVSColorMap(3);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0x43a357ef:	// VS Ice Climber
					ppu->SetVSColorMap(3);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0x737dd1bf:	// VS Super Mario Bros
				case	0x4bf3972d:	// VS Super Mario Bros
				case	0x8b60cc58:	// VS Super Mario Bros
				case	0x8192c804:	// VS Super Mario Bros
					ppu->SetVSColorMap(3);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;
				case	0xd99a2087:	// VS Gradius
					ppu->SetVSColorMap(4);
					pad->SetVSType(PAD::VS_TYPE1);
					goto	vsexit;
				case	0xf9d3b0a3:	// VS Super Xevious
				case	0x9924980a:	// VS Super Xevious
				case	0x66bb838f:	// VS Super Xevious
					ppu->SetVSColorMap(4);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;

				case	0xc99ec059:	// VS Raid on Bungeling Bay(J)
					ppu->SetVSColorMap(1);
					pad->SetVSType(PAD::VS_TYPE5);
					goto	vsexit;
				case	0xca85e56d:	// VS Mighty Bomb Jack(J)
					ppu->SetVSSecurity(0x3D);
					pad->SetVSType(PAD::VS_TYPE0);
					goto	vsexit;

				vsexit:
					ppu->SetVSMode(TRUE);
					break;
				}
			}
		} else {
			m_VSDipValue = 0;
			m_VSDipTable = vsdip_default;
		}

		Reset();

		// #### Set game-specific default options (restore them when changing settings).
		GameOption.defRenderMethod = (INT)GetRenderMethod();
		GameOption.defIRQtype      = (INT)GetIrqType();
		GameOption.defFrameIRQ     = GetFrameIRQmode();
		GameOption.defVideoMode    = GetVideoMode();

		// #### Load settings and set them (if there are no entries, the default will be entered)
		if( rom->GetMapperNo() != 20 ) {
			GameOption.Load( rom->GetPROM_CRC() );
		} else {
			GameOption.Load( rom->GetGameID(), rom->GetMakerID() );
		}
		SetRenderMethod( (RENDERMETHOD)GameOption.nRenderMethod );
		SetIrqType     ( GameOption.nIRQtype );
		SetFrameIRQmode( GameOption.bFrameIRQ );
		SetVideoMode   ( GameOption.bVideoMode );
	} catch( CHAR* str ) {
		DELETEPTR( cpu );
		DELETEPTR( ppu );
		DELETEPTR( apu );
		DELETEPTR( pad );
		DELETEPTR( rom );
		DELETEPTR( mapper );
		throw	str;
#ifndef	_DEBUG
	} catch( ... ) {
		DELETEPTR( cpu );
		DELETEPTR( ppu );
		DELETEPTR( apu );
		DELETEPTR( pad );
		DELETEPTR( rom );
		DELETEPTR( mapper );

		// #### An uncertain error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}

	DEBUGOUT( "Starting emulation!\n" );
}

NES::~NES()
{
	MovieStop();

	SaveSRAM();
	SaveDISK();
	SaveTurboFile();

	DEBUGOUT( "Free NES..." );

	DELETEPTR( cpu );
	DELETEPTR( ppu );
	DELETEPTR( apu );
	DELETEPTR( pad );
	DELETEPTR( rom );
	DELETEPTR( mapper );

	DEBUGOUT( "Ok.\n" );
}

void	NES::SetVideoMode( BOOL bMode )
{	
	bVideoMode = bMode;
	if( !bVideoMode ) {
		nescfg = &NESCONFIG_NTSC;
	} else {
		nescfg = &NESCONFIG_PAL;
	}
	apu->SoundSetup();
}

void	NES::Reset()
{
	SaveSRAM();
	SaveDISK();
	SaveTurboFile();

	// RAM Clear
	ZEROMEMORY( RAM, sizeof(RAM) );
	if( rom->GetPROM_CRC() == 0x29401686 ) {	// Minna no Taabou no Nakayoshi Dai Sakusen(J)
		::memset( RAM, 0xFF, sizeof(RAM) );
	}

	// RAM set
	if( !rom->IsSAVERAM() && rom->GetMapperNo() != 20 ) {
		::memset( WRAM, 0xFF, sizeof(WRAM) );
	}

	ZEROMEMORY( CRAM, sizeof(CRAM) );
	ZEROMEMORY( VRAM, sizeof(VRAM) );

	ZEROMEMORY( SPRAM, sizeof(SPRAM) );
	ZEROMEMORY( BGPAL, sizeof(BGPAL) );
	ZEROMEMORY( SPPAL, sizeof(SPPAL) );

	ZEROMEMORY( CPUREG, sizeof(CPUREG) );
	ZEROMEMORY( PPUREG, sizeof(PPUREG) );

	m_bDiskThrottle = FALSE;

	SetRenderMethod( PRE_RENDER );

	if( rom->IsPAL() ) {
		SetVideoMode( TRUE );
	}

	PROM = rom->GetPROM();
	VROM = rom->GetVROM();

	PROM_8K_SIZE  = rom->GetPROM_SIZE()*2;
	PROM_16K_SIZE = rom->GetPROM_SIZE();
	PROM_32K_SIZE = rom->GetPROM_SIZE()/2;

	VROM_1K_SIZE = rom->GetVROM_SIZE()*8;
	VROM_2K_SIZE = rom->GetVROM_SIZE()*4;
	VROM_4K_SIZE = rom->GetVROM_SIZE()*2;
	VROM_8K_SIZE = rom->GetVROM_SIZE();

	// #### Default Bank
	if( VROM_8K_SIZE ) {
		SetVROM_8K_Bank( 0 );
	} else {
		SetCRAM_8K_Bank( 0 );
	}

	// #### Mirror
	if( rom->Is4SCREEN() ) {
		SetVRAM_Mirror( VRAM_MIRROR4 );
	} else if( rom->IsVMIRROR() ) {
		SetVRAM_Mirror( VRAM_VMIRROR );
	} else {
		SetVRAM_Mirror( VRAM_HMIRROR );
	}

	apu->SelectExSound( 0 );

	ppu->Reset();
	mapper->Reset();

	// Trainer
	if( rom->IsTRAINER() ) {
		::memcpy( WRAM+0x1000, rom->GetTRAINER(), 512 );
	}

	pad->Reset();
	cpu->Reset();
	apu->Reset();

	if( rom->IsNSF() ) {
		mapper->Reset();
	}

	base_cycles = emul_cycles = 0;
}

void	NES::SoftReset()
{
	pad->Reset();
	cpu->Reset();
	apu->Reset();

	if( rom->IsNSF() ) {
		mapper->Reset();
	}

	m_bDiskThrottle = FALSE;

	base_cycles = emul_cycles = 0;
}

void	NES::EmulationCPU( INT basecycles )
{
INT	cycles;

	base_cycles += basecycles;
#if	!ASMVER
	cycles = (INT)((base_cycles/12)-emul_cycles);
#else
	SQWORD&	rBase = base_cycles;
	SQWORD&	rEmul = emul_cycles;

	__asm {
		// check if high DWORD changed
		mov	edi, 12;
		mov	eax, cache;
		mov	ebx, DWORD PTR [rBase];
		mov	ecx, DWORD PTR [rEmul];
		cmp	eax, [ebx+4];
		jz	low_dword;

//	high_dword:
		// temp = (base_cycles/12)
		// Calculate upper DWORD and remainder
		xor	edx, edx;
		mov	eax, [ebx+4];
		mov	cache, eax;
		div	edi;
		mov	cache_hiRem, edx;

	low_dword:
		// Calculate bottom DWORD with upper remainder
		mov edx, cache_hiRem;
		mov	eax, [ebx];
		div	edi;//	diff:
		// cycles = temp - emul_cycles
		// eax carried from DIV above
		xor	edx, edx;
		sub	edx, [ecx];
		add	eax, edx;
		mov	cycles, eax;
	}
#endif

	if( cycles > 0 ) {
		emul_cycles += cpu->EXEC( cycles );
	}
}

void	NES::EmulationCPU_BeforeNMI( INT cycles )
{
	base_cycles += cycles;
	emul_cycles += cpu->EXEC( cycles/12 );
}

/*
	Drawing sequence
	0		Dummy scan line(Do not draw)
	1 - 239 Draw
	240		Dummy scan line,VBLANK Flag ON
	241		VINT period,NMI occurs
	242-261		VINT period
	261		VINT period,VBLANK Flag OFF
*/
void	NES::EmulateFrame( BOOL bDraw )
{
INT	scanline = 0;

	// #### Non-standard Format (NSF) Player's Time
	if( rom->IsNSF() ) {
		EmulateNSF();
		return;
	}

	// Cheat
	CheatCodeProcess();
	//
	NES_scanline = scanline;

	if( RenderMethod != TILE_RENDER ) {
		bZapper = FALSE;
		while( TRUE ) {
			ppu->SetRenderScanline( scanline );

			if( scanline == 0 ) {
			// #### dummy scan line
				if( RenderMethod < POST_RENDER ) {
					EmulationCPU( nescfg->ScanlineCycles );
					ppu->FrameStart();
					ppu->ScanlineNext();
					mapper->HSync( scanline );
					ppu->ScanlineStart();
				} else {
					EmulationCPU( nescfg->HDrawCycles );
					ppu->FrameStart();
					ppu->ScanlineNext();
					mapper->HSync( scanline );
					EmulationCPU( FETCH_CYCLES*32 );
					ppu->ScanlineStart();
					EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
				}
			} else if( scanline < 240 ) {
				if( RenderMethod < POST_RENDER ) {
					if( RenderMethod == POST_ALL_RENDER )
						EmulationCPU( nescfg->ScanlineCycles );
					if( bDraw ) {
						ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
					} else {
						if( pad->IsZapperMode() && scanline == ZapperY ) {
							ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
						} else {
							if( !ppu->IsSprite0( scanline ) ) {
								ppu->DummyScanline( scanline );
							} else {
								ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
							}
						}
					}
					ppu->ScanlineNext();				// #### the screen is different
					if( RenderMethod == PRE_ALL_RENDER )
						EmulationCPU( nescfg->ScanlineCycles );
// #### The statement `ppu->ScanlineNext();` is a part of the Sega Master System (SMS) programming, specifically the Korean version. The `ppu` variable refers to the Programmable Processor Unit (PPU), which is a custom chip used in the SMS for handling graphics and sound processing.  The `ScanlineNext()` function is called inside a loop that processes each scanline of the screen, and it moves on to the next scanline. In other words, it advances the position of the program counter (PC) by one scanline.  In the context of the SMS, this statement is used to process each line of the screen individually, allowing for smooth and efficient rendering of the game graphics. The `ppu` variable contains the address of the PPU chip, and the `ScanlineNext()` function calls are used to iterate through each scanline of the screen, performing various operations such as drawing graphics, updating sprites, and playing sound effects.  In summary, `ppu->ScanlineNext();` is a part of the SMS programming that allows for efficient rendering of game graphics by processing each scanline individually.
					mapper->HSync( scanline );
					ppu->ScanlineStart();
				} else {
					if( RenderMethod == POST_RENDER )
						EmulationCPU( nescfg->HDrawCycles );
					if( bDraw ) {
						ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
					} else {
						if( pad->IsZapperMode() && scanline == ZapperY ) {
							ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
						} else {
							if( !ppu->IsSprite0( scanline ) ) {
								ppu->DummyScanline( scanline );
							} else {
								ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
							}
						}
					}
					if( RenderMethod == PRE_RENDER )
						EmulationCPU( nescfg->HDrawCycles );
					ppu->ScanlineNext();
					mapper->HSync( scanline );
					EmulationCPU( FETCH_CYCLES*32 );
					ppu->ScanlineStart();
					EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
				}
			} else if( scanline == 240 ) {
				mapper->VSync();
				if( RenderMethod < POST_RENDER ) {
					EmulationCPU( nescfg->ScanlineCycles );
					mapper->HSync( scanline );
				} else {
					EmulationCPU( nescfg->HDrawCycles );
					mapper->HSync( scanline );
					EmulationCPU( nescfg->HBlankCycles );
				}
			} else if( scanline <= nescfg->TotalScanlines-1 ) {
				pad->VSync();

				// #### Waiting Period
				if( scanline == nescfg->TotalScanlines-1 ) {
					ppu->VBlankEnd();
				}
				if( RenderMethod < POST_RENDER ) {
					if( scanline == 241 ) {
						ppu->VBlankStart();
						if( PPUREG[0] & PPU_VBLANK_BIT ) {
							cpu->NMI();
						}
					}
					EmulationCPU( nescfg->ScanlineCycles );
					mapper->HSync( scanline );
				} else {
					if( scanline == 241 ) {
						ppu->VBlankStart();
						if( PPUREG[0] & PPU_VBLANK_BIT ) {
							cpu->NMI();
						}
					}
					EmulationCPU( nescfg->HDrawCycles );
					mapper->HSync( scanline );
					EmulationCPU( nescfg->HBlankCycles );
				}

				if( scanline == nescfg->TotalScanlines-1 ) {
					break;
				}
			}
			if( pad->IsZapperMode() ) {
				if( scanline == ZapperY )
					bZapper = TRUE;
				else
					bZapper = FALSE;
			}

			scanline++;
			NES_scanline = scanline;
		}
	} else {
		bZapper = FALSE;
		while( TRUE ) {
			ppu->SetRenderScanline( scanline );

			if( scanline == 0 ) {
			// #### Deep Scan Line
				// H-Draw (4fetches*32)
				EmulationCPU( FETCH_CYCLES*128 );
				ppu->FrameStart();
				ppu->ScanlineNext();
				EmulationCPU( FETCH_CYCLES*10 );
				mapper->HSync( scanline );
				EmulationCPU( FETCH_CYCLES*22 );
				ppu->ScanlineStart();
				EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
			} else if( scanline < 240 ) {
			// #### Screen drawing (Scanline 1-239) in English.
				if( bDraw ) {
					ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
					ppu->ScanlineNext();
					EmulationCPU( FETCH_CYCLES*10 );
					mapper->HSync( scanline );
					EmulationCPU( FETCH_CYCLES*22 );
					ppu->ScanlineStart();
					EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
				} else {
					if( pad->IsZapperMode() && scanline == ZapperY ) {
						ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
						ppu->ScanlineNext();
						EmulationCPU( FETCH_CYCLES*10 );
						mapper->HSync( scanline );
						EmulationCPU( FETCH_CYCLES*22 );
						ppu->ScanlineStart();
						EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
					} else {
						if( !ppu->IsSprite0( scanline ) ) {
							// H-Draw (4fetches*32)
							EmulationCPU( FETCH_CYCLES*128 );
							ppu->DummyScanline( scanline );
							ppu->ScanlineNext();
							EmulationCPU( FETCH_CYCLES*10 );
							mapper->HSync( scanline );
							EmulationCPU( FETCH_CYCLES*22 );
							ppu->ScanlineStart();
							EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
						} else {
							ppu->Scanline( scanline, Config.graphics.bAllSprite, Config.graphics.bLeftClip );
							ppu->ScanlineNext();
							EmulationCPU( FETCH_CYCLES*10 );
							mapper->HSync( scanline );
							EmulationCPU( FETCH_CYCLES*22 );
							ppu->ScanlineStart();
							EmulationCPU( FETCH_CYCLES*10+nescfg->ScanlineEndCycles );
						}
					}
				}
			} else if( scanline == 240 ) {
			// #### Scanline 240
				mapper->VSync();

				EmulationCPU( nescfg->HDrawCycles );
				// H-Sync
				mapper->HSync( scanline );

				EmulationCPU( nescfg->HBlankCycles );
			} else if( scanline <= nescfg->TotalScanlines-1 ) {
				pad->VSync();

			// #### VBlank period
				if( scanline == nescfg->TotalScanlines-1 ) {
					ppu->VBlankEnd();
				}
				if( scanline == 241 ) {
					ppu->VBlankStart();
					if( PPUREG[0]&PPU_VBLANK_BIT ) {
						cpu->NMI();
					}
				}
				EmulationCPU( nescfg->HDrawCycles );

				// H-Sync
				mapper->HSync( scanline );

				EmulationCPU( nescfg->HBlankCycles );

				if( scanline == nescfg->TotalScanlines-1 ) {
					break;
				}
			}
			if( pad->IsZapperMode() ) {
				if( scanline == ZapperY )
					bZapper = TRUE;
				else
					bZapper = FALSE;
			}

			scanline++;
			NES_scanline = scanline;
		}
	}

	// Movie pad
//	if( Config.movie.bPadDisplay && bDraw ) {
//		DrawPad();
//	}
	// Movie pad
	if( bDraw ) {
		DrawPad();
	}
#if	NES_PROFILER
	{
	CHAR	str[256];
	::wsprintf( str, "Cyc:%10d", m_dwProfileCycle );
	DrawString( 9, 240-32, str, 0x1F );
	DrawString( 8, 240-33, str, 0x30 );
	::wsprintf( str, "Ave:%10d", m_dwProfileAveCycle );
	DrawString( 9, 240-23, str, 0x1F );
	DrawString( 8, 240-24, str, 0x30 );
	::wsprintf( str, "Max:%10d", m_dwProfileMaxCycle );
	DrawString( 9, 240-14, str, 0x1F );
	DrawString( 8, 240-15, str, 0x30 );
	}
#endif
}

void	NES::EmulateNSF()
{
R6502	reg;

	ppu->Reset();
	mapper->VSync();

//DEBUGOUT( "Frame\n" );

	if( m_bNsfPlaying ) {
		if( m_bNsfInit ) {
			ZEROMEMORY( RAM, sizeof(RAM) );
			if( !(rom->GetNsfHeader()->ExtraChipSelect&0x04) ) {
				ZEROMEMORY( WRAM, 0x2000 );
			}

			apu->Reset();
			apu->Write( 0x4015, 0x0F );
			apu->Write( 0x4017, 0xC0 );
			apu->ExWrite( 0x4080, 0x80 );	// FDS Volume 0
			apu->ExWrite( 0x408A, 0xE8 );	// FDS Envelope Speed

			cpu->GetContext( reg );
			reg.PC = 0x4710;	// Init Address
			reg.A  = (BYTE)m_nNsfSongNo;
			reg.X  = (BYTE)m_nNsfSongMode;
			reg.Y  = 0;
			reg.S  = 0xFF;
			reg.P  = Z_FLAG|R_FLAG|I_FLAG;
			cpu->SetContext( reg );

			// #### for 1 second
			for( INT i = 0; i < nescfg->TotalScanlines*60; i++ ) {
				EmulationCPU( nescfg->ScanlineCycles );
				cpu->GetContext( reg );

				// #### got out of
				if( reg.PC == 0x4700 ) {
					break;
				}
			}

			m_bNsfInit = FALSE;
		}

		cpu->GetContext( reg );
		// #### If you enter an infinite loop, please reset it.
		if( reg.PC == 0x4700 ) {
			reg.PC = 0x4720;	// Play Address
			reg.A  = 0;
			reg.S  = 0xFF;
			cpu->SetContext( reg );
		}

		for( INT i = 0; i < nescfg->TotalScanlines; i++ ) {
			EmulationCPU( nescfg->ScanlineCycles );
		}
	} else {
		cpu->GetContext( reg );
		reg.PC = 0x4700;	// #### Infinite loop
		reg.S  = 0xFF;
		cpu->SetContext( reg );

		EmulationCPU( nescfg->ScanlineCycles*nescfg->TotalScanlines );
	}
}

void	NES::SetNsfPlay( INT songno, INT songmode )
{
	m_bNsfPlaying  = TRUE;
	m_bNsfInit     = TRUE;
	m_nNsfSongNo   = songno;
	m_nNsfSongMode = songmode;
}

void	NES::SetNsfStop()
{
	m_bNsfPlaying = FALSE;
	apu->Reset();
}

void	NES::Clock( INT cycles )
{
	Tape( cycles );
	Barcode( cycles );
}

BYTE	NES::Read( WORD addr )
{
	switch( addr>>13 ) {
		case	0x00:	// $0000-$1FFF
			return	RAM[addr&0x07FF];
		case	0x01:	// $2000-$3FFF
			return	ppu->Read( addr&0xE007 );
		case	0x02:	// $4000-$5FFF
			if( addr < 0x4100 ) {
				return	ReadReg( addr );
			} else {
				return	mapper->ReadLow( addr );
			}
			break;
		case	0x03:	// $6000-$7FFF
			return	mapper->ReadLow( addr );
		case	0x04:	// $8000-$9FFF
		case	0x05:	// $A000-$BFFF
		case	0x06:	// $C000-$DFFF
		case	0x07:	// $E000-$FFFF
			return	CPU_MEM_BANK[addr>>13][addr&0x1FFF];
	}

	return	0x00;	// #### Prepare for warning.
}

void	NES::Write( WORD addr, BYTE data )
{
	switch( addr>>13 ) {
		case	0x00:	// $0000-$1FFF
			RAM[addr&0x07FF] = data;
			break;
		case	0x01:	// $2000-$3FFF
			if( !rom->IsNSF() ) {
				ppu->Write( addr&0xE007, data );
			}
			break;
		case	0x02:	// $4000-$5FFF
			if( addr < 0x4100 ) {
				WriteReg( addr, data );
			} else {
				mapper->WriteLow( addr, data );
			}
			break;
		case	0x03:	// $6000-$7FFF
			mapper->WriteLow( addr, data );
			break;
		case	0x04:	// $8000-$9FFF
		case	0x05:	// $A000-$BFFF
		case	0x06:	// $C000-$DFFF
		case	0x07:	// $E000-$FFFF
			mapper->Write( addr, data );

			GenieCodeProcess();
			break;
	}
}

BYTE	NES::ReadReg( WORD addr )
{
	switch( addr & 0xFF ) {
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x04: case 0x05: case 0x06: case 0x07:
		case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F:
		case 0x10: case 0x11: case 0x12: case 0x13:
			return	apu->Read( addr );
			break;
		case	0x15:
			return	apu->Read( addr );
			break;

		case	0x14:
			return	addr&0xFF;
			break;

		case	0x16:
			if( rom->IsVSUNISYSTEM() ) {
				return	pad->Read( addr );
			} else {
				return	pad->Read( addr ) | 0x40 | m_TapeOut;
			}
			break;
		case	0x17:
			if( rom->IsVSUNISYSTEM() ) {
				return	pad->Read( addr );
			} else {
				return	pad->Read( addr ) | apu->Read( addr );
			}
			break;
		default:
			return	mapper->ExRead( addr );
			break;
	}
}

void	NES::WriteReg( WORD addr, BYTE data )
{
	switch( addr & 0xFF ) {
		case 0x00: case 0x01: case 0x02: case 0x03:
		case 0x04: case 0x05: case 0x06: case 0x07:
		case 0x08: case 0x09: case 0x0A: case 0x0B:
		case 0x0C: case 0x0D: case 0x0E: case 0x0F:
		case 0x10: case 0x11: case 0x12: case 0x13:
		case 0x15:
			apu->Write( addr, data );
			CPUREG[addr & 0xFF] = data;
			break;

		case	0x14:
			ppu->DMA( data );
			cpu->DMA( 514 ); // DMA Pending cycle
			CPUREG[addr & 0xFF] = data;
			break;

		case	0x16:
			mapper->ExWrite( addr, data );	// For VS-Unisystem
			pad->Write( addr, data );
			CPUREG[addr & 0xFF] = data;
			m_TapeIn = data;
			break;
		case	0x17:
			CPUREG[addr & 0xFF] = data;
			pad->Write( addr, data );
			apu->Write( addr, data );
			break;
		// #### Fixed Port
		case	0x18:
			apu->Write( addr, data );
			break;

#if	NES_PROFILER
		case	0x1D:
			m_dwProfileAveCycle = 0;
			m_dwProfileMaxCycle = 0;
			break;
		case	0x1E:
			m_dwProfileTempCycle = cpu->GetTotalCycles();
			break;
		case	0x1F:
			m_dwProfileCycle = cpu->GetTotalCycles()-m_dwProfileTempCycle-4;
			if( !m_dwProfileAveCycle ) {
				m_dwProfileAveCycle += m_dwProfileCycle;
			} else {
				m_dwProfileAveCycle += m_dwProfileCycle;
				m_dwProfileAveCycle /= 2;
			}
			if( m_dwProfileMaxCycle < m_dwProfileCycle ) {
				m_dwProfileMaxCycle = m_dwProfileCycle;
			}
			break;
#endif
#if	0
		case	0x1C:
			m_dwProfileTempCycle = cpu->GetTotalCycles();
			break;
		case	0x1D:
			m_dwProfileCycle = cpu->GetTotalCycles()-m_dwProfileTempCycle-4;
			m_dwProfileTotalCycle += m_dwProfileCycle;
			m_dwProfileTotalCount++;
			break;
		case	0x1E:
			m_dwProfileTotalCount = 0;
			m_dwProfileTotalCycle = 0;
			m_dwTotalTempCycle = cpu->GetTotalCycles();
			break;
		case	0x1F:
			m_dwTotalCycle = cpu->GetTotalCycles()-m_dwTotalTempCycle-4;
			break;
#endif
		default:
			mapper->ExWrite( addr, data );
			break;
	}
}

void	NES::LoadSRAM()
{
	if( rom->IsNSF() )
		return;

	ZEROMEMORY( WRAM, sizeof(WRAM) );

	if( !rom->IsSAVERAM() )
		return;

	string	pathstr, tempstr;
	if( Config.path.bSavePath ) {
		pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSavePath );
	} else {
		pathstr = rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt( pathstr.c_str(), rom->GetRomName(), "sav" );
	DEBUGOUT( "Path: %s\n", tempstr.c_str() );

	FILE*	fp = NULL;
	try
	{
		if( !(fp = ::fopen( tempstr.c_str(), "rb" )) ) {
			// #### I can't open the xxx file.
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			sprintf( szErrorString, szErrStr, tempstr.c_str() );
			throw	szErrorString;
		}

		DEBUGOUT( "Loading SAVERAM..." );

		LONG	size;
		// #### File size retrieval
		::fseek( fp, 0, SEEK_END );
		size = ftell( fp );
		::fseek( fp, 0, SEEK_SET );
		if( size <= 128*1024 && WRAM!=0) {
			if( ::fread( WRAM, size, 1, fp ) != 1 )
				throw	"File Read error.";
		}

		DEBUGOUT( "Ok.\n" );
		FCLOSE( fp );
	} catch( CHAR* str ) {
		FCLOSE( fp );
		DEBUGOUT( "Loading SAVERAM Error.\n" );
		DEBUGOUT( "%s\n", str );
//		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		FCLOSE( fp );
		DEBUGOUT( "Loading SAVERAM Error.\n" );
		// #### An unknown error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}
}

void	NES::SaveSRAM()
{
INT	i;

	if( rom->IsNSF() )
		return;

	if( !rom->IsSAVERAM() )
		return;

	for( i = 0; i < SAVERAM_SIZE; i++ ) {
		if( WRAM[i] != 0x00 )
			break;
	}

	if( i < SAVERAM_SIZE ) {
		DEBUGOUT( "Saving SAVERAM...\n" );

		string	pathstr, tempstr;
		if( Config.path.bSavePath ) {
			pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSavePath );
			::CreateDirectory( pathstr.c_str(), NULL );
		} else {
			pathstr = rom->GetRomPath();
		}
		tempstr = CPathlib::MakePathExt( pathstr.c_str(), rom->GetRomName(), "sav" );
		DEBUGOUT( "Path: %s\n", tempstr.c_str() );

		FILE*	fp = NULL;
		try
		{
			if( !(fp = ::fopen( tempstr.c_str(), "wb" )) ) {
				// #### I can't open the xxx file.
				LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
				sprintf( szErrorString, szErrStr, tempstr.c_str() );
				throw	szErrorString;
			}

			if( ::fwrite( WRAM, SAVERAM_SIZE, 1, fp ) != 1 ) {
				// #### File write failure.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}

			DEBUGOUT( "Ok.\n" );
			FCLOSE( fp );
		} catch( CHAR* str ) {
			DEBUGOUT( "Writing SAVERAM Error.\n" );
			FCLOSE( fp );
			throw	str;
	#ifndef	_DEBUG
		} catch(...) {
			DEBUGOUT( "Writing SAVERAM Error.\n" );
			FCLOSE( fp );
			// #### An uncertain error has occurred.
			throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
	#endif
		}
	}
}

void	NES::LoadDISK()
{
	if( rom->GetMapperNo() != 20 )
		return;

	BOOL	bExit = FALSE;

	DWORD	i, j, diskno;
	FILE*	fp = NULL;
	DISKIMGFILEHDR	ifh;
	DISKIMGHDR	hdr;
	LPBYTE		disk;

	WORD	Version;

	string	pathstr, tempstr;
	if( Config.path.bSavePath ) {
		pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSavePath );
	} else {
		pathstr = rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt( pathstr.c_str(), rom->GetRomName(), "dsv" );
	DEBUGOUT( "Path: %s\n", tempstr.c_str() );

	try
	{
		if( !(fp = ::fopen( tempstr.c_str(), "rb" )) ) {
			// #### I cannot open the xxx file.
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			sprintf( szErrorString, szErrStr, tempstr.c_str() );
			throw	szErrorString;
		}

		if( ::fread( &ifh, sizeof(DISKIMGFILEHDR), 1, fp ) != 1 ) {
			// #### File loading failed.
			throw	CApp::GetErrorString( IDS_ERROR_READ );
		}

		if( ::memcmp( ifh.ID, VIRTUALNES_DI_SIGN, sizeof(ifh.ID) ) == 0 ) {
			if( ifh.BlockVersion < 0x0100 && ifh.BlockVersion > 0x200 ) {
				// #### Not responding or unresponsive form.
				throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
			}
			Version = ifh.BlockVersion;
		} else {
			// #### Not responding format.
			throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
		}

		if( Version == 0x0100 ) {
		// #### Before version 0.24
			if( ifh.DiskNumber > 4 ) {
				// #### Not in compliance with the format.
				throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
			}

			for( i = 0; i < (INT)ifh.DiskNumber; i++ ) {
				if( ::fread( &hdr, sizeof(DISKIMGHDR), 1, fp ) != 1 ) {
					if( i == 0 ) {
						// #### File read failure.
						throw	CApp::GetErrorString( IDS_ERROR_READ );
					} else {
						break;
					}
				}

				if( ::memcmp( hdr.ID, "SIDE0A", sizeof(hdr.ID) ) == 0 ) {
					diskno = 0;
				} else if( ::memcmp( hdr.ID, "SIDE0B", sizeof(hdr.ID) ) == 0 ) {
					diskno = 1;
				} else if( ::memcmp( hdr.ID, "SIDE1A", sizeof(hdr.ID) ) == 0 ) {
					diskno = 2;
				} else if( ::memcmp( hdr.ID, "SIDE1B", sizeof(hdr.ID) ) == 0 ) {
					diskno = 3;
				} else {
					// #### Not responded form
					throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
				}

				for( j = 0; j < 16; j++ ) {
					if( hdr.DiskTouch[j] ) {
						disk = rom->GetPROM()+16+65500*diskno+(4*1024)*j;
						if( j < 15 ) {
							if( ::fread( disk, 4*1024, 1, fp ) != 1 ) {
								bExit = TRUE;
								// #### File read failed.
								throw	CApp::GetErrorString( IDS_ERROR_READ );
							}
						} else {
							if( ::fread( disk, 4*1024-36, 1, fp ) != 1 ) {
								bExit = TRUE;
								// #### File loading failed.
								throw	CApp::GetErrorString( IDS_ERROR_READ );
							}
						}
					}
				}
			}
		} else 
		if( Version == 0x0200 || Version == 0x0210 ) {
			// #### Version 0.30 and later:  In versions after 0.30, the translation will be in English.
			DISKFILEHDR	dfh;
			LPBYTE	lpDisk = rom->GetPROM();
			LPBYTE	lpWrite = rom->GetDISK();
			LONG	DiskSize = 16+65500*rom->GetDiskNo();
			DWORD	pos;
			BYTE	data;

			// #### to delete
			::ZeroMemory( lpWrite, 16+65500*rom->GetDiskNo() );

			// #### Headline Revision
			if( ::fseek( fp, 0, SEEK_SET ) ) {
				// #### File reading failure.
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}
			if( ::fread( &dfh, sizeof(DISKFILEHDR), 1, fp ) != 1 ) {
				// #### File loading failed.
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}

			if( Config.emulator.bCrcCheck ) {
				// #### Is the title of the current road different?
				if( dfh.ProgID  !=       rom->GetGameID()
				 || dfh.MakerID != (WORD)rom->GetMakerID()
				 || dfh.DiskNo  != (WORD)rom->GetDiskNo() ) {
					// #### File read failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}
			}

			for( i = 0; i < dfh.DifferentSize; i++ ) {
				if( ::fread( &pos, sizeof(DWORD), 1, fp ) != 1 ) {
					// #### File loading failed.
					bExit = TRUE;
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}
				data = (BYTE)(pos>>24);
				pos &= 0x00FFFFFF;
				if( pos >= 16 && pos < (DWORD)DiskSize ) {
					lpDisk[pos] = data;
					lpWrite[pos] = 0xFF;
				}
			}
		}
		FCLOSE( fp );
	} catch( CHAR* str ) {
		FCLOSE( fp );
		DEBUGOUT( "%s\n", str );
		if( bExit )
			throw	str;
#ifndef	_DEBUG
	} catch(...) {
		FCLOSE( fp );
		DEBUGOUT( "Loading DISKIMAGE Error.\n" );
		// #### An ambiguous error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}
}

void	NES::SaveDISK()
{
	if( rom->GetMapperNo() != 20 )
		return;

	INT	i;
	FILE*	fp = NULL;
	DISKFILEHDR ifh;
	LPBYTE	lpDisk  = rom->GetPROM();
	LPBYTE	lpWrite = rom->GetDISK();
	LONG	DiskSize = 16+65500*rom->GetDiskNo();
	DWORD	data;

	try
	{
		::ZeroMemory( &ifh, sizeof(ifh) );
		::memcpy( ifh.ID, VIRTUALNES_DI_SIGN, sizeof(ifh.ID) );
		ifh.BlockVersion = 0x0210;
		ifh.ProgID  = rom->GetGameID();
		ifh.MakerID = (WORD)rom->GetMakerID();
		ifh.DiskNo  = (WORD)rom->GetDiskNo();

		// #### number
		for( i = 16; i < DiskSize; i++ ) {
			if( lpWrite[i] )
				ifh.DifferentSize++;
		}

		if( !ifh.DifferentSize )
			return;

		string	pathstr, tempstr;
		if( Config.path.bSavePath ) {
			pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSavePath );
			::CreateDirectory( pathstr.c_str(), NULL );
		} else {
			pathstr = rom->GetRomPath();
		}
		tempstr = CPathlib::MakePathExt( pathstr.c_str(), rom->GetRomName(), "dsv" );
		DEBUGOUT( "Path: %s\n", tempstr.c_str() );

		if( !(fp = ::fopen( tempstr.c_str(), "wb" )) ) {
			// #### #### xxx File not opened
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			::wsprintf( szErrorString, szErrStr, tempstr.c_str() );
			throw	szErrorString;
		}

		if( ::fwrite( &ifh, sizeof(DISKFILEHDR), 1, fp ) != 1 ) {
			// #### File write failure.
			throw	CApp::GetErrorString( IDS_ERROR_WRITE );
		}

		for( i = 16; i < DiskSize; i++ ) {
			if( lpWrite[i] ) {
				data = i & 0x00FFFFFF;
				data |= ((DWORD)lpDisk[i]&0xFF)<<24;

				// Write File
				if( ::fwrite( &data, sizeof(DWORD), 1, fp ) != 1 ) {
					// #### File write failure.
					throw	CApp::GetErrorString( IDS_ERROR_WRITE );
				}
			}
		}
		FCLOSE( fp );
	} catch( CHAR* str ) {
		FCLOSE( fp );
		DEBUGOUT( "%s\n", str );
#ifndef	_DEBUG
	} catch(...) {
		FCLOSE( fp );
		DEBUGOUT( "Saving DISKIMAGE Error.\n" );
		// #### Unclear error occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}
}

void	NES::LoadTurboFile()
{
	ZEROMEMORY( ERAM, sizeof(ERAM) );

	if( pad->GetExController() != PAD::EXCONTROLLER_TURBOFILE )
		return;

	string	pathstr, tempstr;
	if( Config.path.bSavePath ) {
		pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSavePath );
	} else {
		pathstr = rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt( pathstr.c_str(), "TurboFile", "vtf" );
	DEBUGOUT( "Path: %s\n", tempstr.c_str() );

	FILE*	fp = NULL;
	try
	{
		if( !(fp = ::fopen( tempstr.c_str(), "rb" )) ) {
			// #### I cannot open the xxx file.
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			sprintf( szErrorString, szErrStr, tempstr.c_str() );
			throw	szErrorString;
		}

		DEBUGOUT( "Loading TURBOFILE..." );

		LONG	size;
		// #### File size retrieval
		::fseek( fp, 0, SEEK_END );
		size = ftell( fp );
		::fseek( fp, 0, SEEK_SET );
		if( size > 32*1024 ) {
			size = 32*1024;
		}
		if (ERAM != 0) {
			if (::fread(ERAM, size, 1, fp) != 1)
				throw	"File Read error.";
		}
		DEBUGOUT( "Ok.\n" );
		FCLOSE( fp );
	} catch( CHAR* str ) {
		FCLOSE( fp );
		DEBUGOUT( "Loading TurboFile Error.\n" );
		DEBUGOUT( "%s\n", str );
//		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		FCLOSE( fp );
		DEBUGOUT( "Loading TurboFile Error.\n" );
		// #### An unspecified error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}
}

void	NES::SaveTurboFile()
{
INT	i;

	if( pad->GetExController() != PAD::EXCONTROLLER_TURBOFILE )
		return;

	for( i = 0; i < sizeof(ERAM); i++ ) {
		if( ERAM[i] != 0x00 )
			break;
	}

	if( i < sizeof(ERAM) ) {
		DEBUGOUT( "Saving TURBOFILE...\n" );

		string	pathstr, tempstr;
		if( Config.path.bSavePath ) {
			pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSavePath );
			::CreateDirectory( pathstr.c_str(), NULL );
		} else {
			pathstr = rom->GetRomPath();
		}
		tempstr = CPathlib::MakePathExt( pathstr.c_str(), "TurboFile", "vtf" );
		DEBUGOUT( "Path: %s\n", tempstr.c_str() );

		FILE*	fp = NULL;
		try
		{
			if( !(fp = ::fopen( tempstr.c_str(), "wb" )) ) {
				// #### I cannot open the xxx file.
				LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
				sprintf( szErrorString, szErrStr, tempstr.c_str() );
				throw	szErrorString;
			}

			if( ::fwrite( ERAM, sizeof(ERAM), 1, fp ) != 1 ) {
				// #### File write failure.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}

			DEBUGOUT( "Ok.\n" );
			FCLOSE( fp );
		} catch( CHAR* str ) {
			DEBUGOUT( "Writing TurboFile Error.\n" );
			FCLOSE( fp );
			throw	str;
	#ifndef	_DEBUG
		} catch(...) {
			DEBUGOUT( "Writing TurboFile Error.\n" );
			FCLOSE( fp );
			// #### An unknown error has occurred.
			throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
	#endif
		}
	}
}

INT	NES::IsStateFile( const char* fname, ROM* rom )
{
FILE*	fp = NULL;
FILEHDR2 header;

	if( !(fp = ::fopen( fname, "rb" )) )
		return	-1;

	if( ::fread( &header, sizeof(header), 1, fp ) != 1 ) {
		FCLOSE( fp );
		return	-1;
	}
	FCLOSE( fp );

	if( ::memcmp( header.ID, VIRTUALNES_ST_SIGN, sizeof(header.ID) ) == 0 ) {
		if( header.BlockVersion < 0x0100 )
			return	0;

		if( Config.emulator.bCrcCheck ) {
			if( header.BlockVersion >= 0x200 ) {
				if( rom->GetMapperNo() != 20 ) {
				// #### Not part of FDS
					if( header.Ext0 != rom->GetPROM_CRC() ) {
						return	IDS_ERROR_ILLEGALSTATECRC;	// #### Isn't it different?
					}
				} else {
				// FDS
					if( header.Ext0 != rom->GetGameID() ||
					    header.Ext1 != (WORD)rom->GetMakerID() ||
					    header.Ext2 != (WORD)rom->GetDiskNo() )
						return	IDS_ERROR_ILLEGALSTATECRC;	// #### Different, isn't it?
				}
			}
		}
		return	0;
	}
	return	-1;
}

BOOL	NES::LoadState( const char* fname )
{
FILE*	fp = NULL;
BOOL	bRet = FALSE;

	if( rom->IsNSF() )
		return	TRUE;

	try {
		if( !(fp = ::fopen( fname, "rb" )) ) {
			// #### I cannot open the xxx file.
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			sprintf( szErrorString, szErrStr, fname );
			throw	szErrorString;
		}

		bRet = ReadState( fp );

		FCLOSE( fp );
	} catch( CHAR* str ) {
		DEBUGOUT( "State load error.\n" );
		DEBUGOUT( "%s\n", str );
		FCLOSE( fp );
		return	FALSE;
#ifndef	_DEBUG
	} catch(...) {
		DEBUGOUT( "State load error.\n" );
		FCLOSE( fp );
		// #### An unknown error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}

	return	bRet;
}

BOOL	NES::SaveState( const char* fname )
{
FILE*	fp = NULL;

	if( rom->IsNSF() )
		return	TRUE;

	try {
		if( !(fp = ::fopen( fname, "wb" )) ) {
			// #### I cannot open it.
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			sprintf( szErrorString, szErrStr, fname );
			throw	szErrorString;
		}

		WriteState( fp );

		FCLOSE( fp );
	} catch( CHAR* str ) {
		DEBUGOUT( "State save error.\n" );
		FCLOSE( fp );
		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		DEBUGOUT( "State save error.\n" );
		FCLOSE( fp );
		// #### An unknown error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}

	return	TRUE;
}

BOOL	NES::ReadState( FILE* fp )
{
	INT	i;
	BOOL	bHeader = FALSE;
	WORD	Version = 0;

	BLOCKHDR hdr;
	INT	type;

	while( TRUE ) {
		// Read File
		if( ::fread( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 )
			break;

		// File Header check
		if( !bHeader ) {
			LPFILEHDR	fh = (LPFILEHDR)&hdr;
			if( ::memcmp( fh->ID, VIRTUALNES_ST_SIGN, sizeof(fh->ID) ) == 0 ) {
				Version = fh->BlockVersion;
				if( Version == 0x0100 ) {
				// #### up until a specific version number.
					bHeader = TRUE;
					// #### Old people cannot go on the road.
					if( m_bMoviePlay ) {
						return	FALSE;
					}
					// #### The old FDS cannot load.
					if( rom->GetMapperNo() == 20 ) {
						// #### Not in a responsive form.
						throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
					}
				} else 
				if( Version == 0x0200 || Version == 0x0210 ) {
				// #### Version 0.60 or later.
					FILEHDR2 hdr2;
					// #### Re-reading the heading
					if( ::_fseeki64( fp, -(LONG_PTR)sizeof(BLOCKHDR), SEEK_CUR ) ) {
						// #### File loading failed.
						throw	CApp::GetErrorString( IDS_ERROR_READ );
					}
					// Read File
					if( ::fread( &hdr2, sizeof(FILEHDR2), 1, fp ) != 1 ) {
						// #### File loading failed.
						throw	CApp::GetErrorString( IDS_ERROR_READ );
					}

#if	0
					if( Config.emulator.bCrcCheck ) {
						// #### now
						if( rom->GetMapperNo() != 20 ) {
						// #### FDS
							if( hdr2.Ext0 != rom->GetPROM_CRC() ) {
								return	FALSE;	// #### Different, right?
							}
						} else {
						// FDS
							if( hdr2.Ext0 != rom->GetGameID() ||
							    hdr2.Ext1 != (WORD)rom->GetMakerID() ||
							    hdr2.Ext2 != (WORD)rom->GetDiskNo() )
								return	FALSE;	// #### It's different.
						}
					}
#endif

					// #### During movie playback, file pointer and step count.
					// #### Enter recording mode and capture a video.
					if( m_bMoviePlay || m_bMovieRec ) {
						// #### reshots
						if( m_hedMovie.Control & 0x80 ) {
							if( hdr2.MovieOffset && hdr2.MovieStep ) {
								if( m_bMoviePlay ) {
								// #### Playing
									// #### Steps count
									if( hdr2.MovieStep > m_hedMovie.MovieStep )
										return	FALSE;
								} else {
								// #### record
									// #### Step count
									if( hdr2.MovieStep > m_MovieStep )
										return	FALSE;
								}
//DEBUGOUT( "LD STEP=%d POS=%d\n", hdr2.MovieStep, hdr2.MovieOffset );

								m_bMoviePlay = FALSE;
								m_bMovieRec = TRUE;
								m_MovieStep = hdr2.MovieStep;
								m_hedMovie.RecordTimes++;	// #### Shoot and reshare count +1
								if( ::fseek( m_fpMovie, hdr2.MovieOffset, SEEK_SET ) ) {
// #### MOVIE:STATE LOAD SEEK FAILED\n
									// #### File reading failure.
									throw	CApp::GetErrorString( IDS_ERROR_READ );
								}
							} else {
								return	FALSE;
							}
						} else {
							return	FALSE;
						}
					}
				}
				bHeader = TRUE;
				continue;
			}
		}

		if( !bHeader ) {
			// #### Unresponsive form
			throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
		}

//DEBUGOUT( "HEADER ID=%8s\n", hdr.ID );

		type = -1;
		if( ::memcmp( hdr.ID, "REG DATA", sizeof(hdr.ID) ) == 0 ) 
			type = 0;
		if( ::memcmp( hdr.ID, "RAM DATA", sizeof(hdr.ID) ) == 0 )
			type = 1;
		if( ::memcmp( hdr.ID, "MMU DATA", sizeof(hdr.ID) ) == 0 )
			type = 2;
		if( ::memcmp( hdr.ID, "MMC DATA", sizeof(hdr.ID) ) == 0 )
			type = 3;
		if( ::memcmp( hdr.ID, "CTR DATA", sizeof(hdr.ID) ) == 0 )
			type = 4;
		if( ::memcmp( hdr.ID, "SND DATA", sizeof(hdr.ID) ) == 0 )
			type = 5;

		if( rom->GetMapperNo() == 20 ) {
			if( ::memcmp( hdr.ID, "DISKDATA", sizeof(hdr.ID) ) == 0 )
				type = 6;
		}

		if( ::memcmp( hdr.ID, "EXCTRDAT", sizeof(hdr.ID) ) == 0 )
			type = 7;

		if( type == -1 ) {
//DEBUGOUT( "UNKNOWN HEADER ID=%8s\n", hdr.ID );
			break;
		}

		switch( type ) {
			case	0:
				// REGISTER STATE
				{
				if( hdr.BlockVersion < 0x0200 ) {
					REGSTAT_O	reg;
					if( ::fread( &reg, sizeof(REGSTAT_O), 1, fp ) != 1 ) {
						// #### File read-in failure.
						throw	CApp::GetErrorString( IDS_ERROR_READ );
					}

					// LOAD CPU STATE
					R6502	R;
					R.PC = reg.cpureg.cpu.PC;
					R.A  = reg.cpureg.cpu.A;
					R.X  = reg.cpureg.cpu.X;
					R.Y  = reg.cpureg.cpu.Y;
					R.S  = reg.cpureg.cpu.S;
					R.P  = reg.cpureg.cpu.P;
					R.INT_pending = reg.cpureg.cpu.I;
					cpu->SetContext( R );
//					FrameIRQ = reg.cpureg.cpu.FrameIRQ;

					if( hdr.BlockVersion < 0x0110 ) {
						emul_cycles = 0;
						base_cycles = reg.cpureg.cpu.mod_cycles;
					} else if( hdr.BlockVersion == 0x0110 ) {
//						FrameIRQ_cycles = reg.cpureg.cpu.mod_cycles;
						emul_cycles = reg.cpureg.cpu.emul_cycles;
						base_cycles = reg.cpureg.cpu.base_cycles;
					}

					// LOAD PPU STATE
					PPUREG[0] = reg.ppureg.ppu.reg0;
					PPUREG[1] = reg.ppureg.ppu.reg1;
					PPUREG[2] = reg.ppureg.ppu.reg2;
					PPUREG[3] = reg.ppureg.ppu.reg3;
					PPU7_Temp = reg.ppureg.ppu.reg7;
					loopy_t = reg.ppureg.ppu.loopy_t;
					loopy_v = reg.ppureg.ppu.loopy_v;
					loopy_x = reg.ppureg.ppu.loopy_x;
					PPU56Toggle = reg.ppureg.ppu.toggle56;
				} else {
					REGSTAT	reg;
					if( ::fread( &reg, sizeof(REGSTAT), 1, fp ) != 1 ) {
						// #### File loading failed.
						throw	CApp::GetErrorString( IDS_ERROR_READ );
					}

					// LOAD CPU STATE
					R6502	R;
					R.PC = reg.cpureg.cpu.PC;
					R.A  = reg.cpureg.cpu.A;
					R.X  = reg.cpureg.cpu.X;
					R.Y  = reg.cpureg.cpu.Y;
					R.S  = reg.cpureg.cpu.S;
					R.P  = reg.cpureg.cpu.P;
					R.INT_pending = reg.cpureg.cpu.I;
					cpu->SetContext( R );

					if( hdr.BlockVersion == 0x0200 ) {
//						FrameIRQ = reg.cpureg.cpu.FrameIRQ;
//						bFrameIRQ_occur = (reg.cpureg.cpu.FrameIRQ_occur!=0)?TRUE:FALSE;
//						FrameIRQ_cycles = reg.cpureg.cpu.FrameIRQ_cycles;
					} else {
						apu->SetFrameIRQ( reg.cpureg.cpu.FrameIRQ_cycles,
								  reg.cpureg.cpu.FrameIRQ_count,
								  reg.cpureg.cpu.FrameIRQ_type,
								  reg.cpureg.cpu.FrameIRQ,
								  reg.cpureg.cpu.FrameIRQ_occur );
					}

					emul_cycles = reg.cpureg.cpu.emul_cycles;
					base_cycles = reg.cpureg.cpu.base_cycles;

					cpu->SetDmaCycles( (INT)reg.cpureg.cpu.DMA_cycles );

					// LOAD PPU STATE
					PPUREG[0] = reg.ppureg.ppu.reg0;
					PPUREG[1] = reg.ppureg.ppu.reg1;
					PPUREG[2] = reg.ppureg.ppu.reg2;
					PPUREG[3] = reg.ppureg.ppu.reg3;
					PPU7_Temp = reg.ppureg.ppu.reg7;
					loopy_t = reg.ppureg.ppu.loopy_t;
					loopy_v = reg.ppureg.ppu.loopy_v;
					loopy_x = reg.ppureg.ppu.loopy_x;
					PPU56Toggle = reg.ppureg.ppu.toggle56;
				}

				// APU STATE
				// #### Erase the queue
				apu->QueueClear();

// #### I saved the APU state, so you can't blame me.
// #### DMC
//				for( i = 0x4010; i <= 0x4013; i++ ) {
//					apu->Write( i, 0 );
//				}
				}
				break;
			case	1:
				// RAM STATE
				{
				RAMSTAT	ram;
				if( ::fread( &ram, sizeof(RAMSTAT), 1, fp ) != 1 ) {
					// #### File loading failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}
				::memcpy( RAM, ram.RAM, sizeof(ram.RAM) );
				::memcpy( BGPAL, ram.BGPAL, sizeof(ram.BGPAL) );
				::memcpy( SPPAL, ram.SPPAL, sizeof(ram.SPPAL) );
				::memcpy( SPRAM, ram.SPRAM, sizeof(ram.SPRAM) );
				if( rom->IsSAVERAM() ) {
					if( ::fread( WRAM, SAVERAM_SIZE, 1, fp ) != 1 ) {
						// #### File read failure.
						throw	CApp::GetErrorString( IDS_ERROR_READ );
					}
				}
				}
				break;
			case	2:
				// BANK STATE
				{
				MMUSTAT mmu;
				if( ::fread( &mmu, sizeof(MMUSTAT), 1, fp ) != 1 ) {
					// #### File loading failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}
				if( hdr.BlockVersion == 0x100 ) {
				// #### Older version
					if( mmu.CPU_MEM_TYPE[3] == BANKTYPE_RAM
					 || mmu.CPU_MEM_TYPE[3] == BANKTYPE_DRAM ) {
						if( ::fread( CPU_MEM_BANK[3], 8*1024, 1, fp ) != 1 ) {
							// #### Failed to load file.
							throw	CApp::GetErrorString( IDS_ERROR_READ );
						}
					} else if( !rom->IsSAVERAM() ) {
						SetPROM_8K_Bank( 3, mmu.CPU_MEM_PAGE[3] );
					}
					// #### Bank 0-3 excluding road
					for( i = 4; i < 8; i++ ) {
						CPU_MEM_TYPE[i] = mmu.CPU_MEM_TYPE[i];
						CPU_MEM_PAGE[i] = mmu.CPU_MEM_PAGE[i];
						if( CPU_MEM_TYPE[i] == BANKTYPE_ROM ) {
							SetPROM_8K_Bank( i, CPU_MEM_PAGE[i] );
						} else {
							if( ::fread( CPU_MEM_BANK[i], 8*1024, 1, fp ) != 1 ) {
								// #### File load failed.
								throw	CApp::GetErrorString( IDS_ERROR_READ );
							}
						}
					}
				} else if( hdr.BlockVersion == 0x200 ) {
				// #### The latest version.
					// #### to load all roads,
					for( i = 3; i < 8; i++ ) {
						CPU_MEM_TYPE[i] = mmu.CPU_MEM_TYPE[i];
						CPU_MEM_PAGE[i] = mmu.CPU_MEM_PAGE[i];
						if( CPU_MEM_TYPE[i] == BANKTYPE_ROM ) {
							SetPROM_8K_Bank( i, CPU_MEM_PAGE[i] );
						} else {
							if( ::fread( CPU_MEM_BANK[i], 8*1024, 1, fp ) != 1 ) {
								// #### File loading failed.
								throw	CApp::GetErrorString( IDS_ERROR_READ );
							}
						}
					}
				}
				// VRAM
				if( ::fread( VRAM, 4*1024, 1, fp ) != 1 ) {
					// #### File loading failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}

				// CRAM
				for( i = 0; i < 8; i++ ) {
					if( mmu.CRAM_USED[i] != 0 ) {
						if( ::fread( &CRAM[0x1000*i], 4*1024, 1, fp ) != 1 ) {
							// #### File loading failed.
							throw	CApp::GetErrorString( IDS_ERROR_READ );
						}
					}
				}
				// BANK
				for( i = 0; i < 12; i++ ) {
					if( mmu.PPU_MEM_TYPE[i] == BANKTYPE_VROM ) {
						SetVROM_1K_Bank( i, mmu.PPU_MEM_PAGE[i] );
					} else if( mmu.PPU_MEM_TYPE[i] == BANKTYPE_CRAM ) {
						SetCRAM_1K_Bank( i, mmu.PPU_MEM_PAGE[i] );
					} else if( mmu.PPU_MEM_TYPE[i] == BANKTYPE_VRAM ) {
						SetVRAM_1K_Bank( i, mmu.PPU_MEM_PAGE[i] );
					} else {
						throw	"Unknown bank types.";
					}
				}
				}
				break;
			case	3:
				// MMC STATE
				{
				MMCSTAT	mmc;
				if( ::fread( &mmc, sizeof(MMCSTAT), 1, fp ) != 1 ) {
					// #### File loading failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}
				mapper->LoadState( mmc.mmcdata );
				}
				break;
			case	4:
				// CTR STATE
				{
				CTRSTAT	ctr;
				if( ::fread( &ctr, sizeof(CTRSTAT), 1, fp ) != 1 ) {
					// #### File reading failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}

				pad->pad1bit = ctr.ctrreg.ctr.pad1bit;
				pad->pad2bit = ctr.ctrreg.ctr.pad2bit;
				pad->pad3bit = ctr.ctrreg.ctr.pad3bit;
				pad->pad4bit = ctr.ctrreg.ctr.pad4bit;
				pad->SetStrobe( (ctr.ctrreg.ctr.strobe!=0)?TRUE:FALSE );
				}
				break;

			case	5:
				// SND STATE
				{
				SNDSTAT	snd;
				if( ::fread( &snd, sizeof(SNDSTAT), 1, fp ) != 1 ) {
					// #### File loading failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}
				apu->LoadState( snd.snddata );
				}
				break;

			// Disk Images
			// #### Version 0.30 or later
			case	6:
				{
				DISKDATA ddata;
				DWORD	pos;
				BYTE	data;
				LONG	DiskSize = 16+65500*rom->GetDiskNo();
				LPBYTE	lpDisk  = rom->GetPROM();
				LPBYTE	lpWrite = rom->GetDISK();

				// #### to erase or cancel a personal flag or emblem
				::ZeroMemory( lpWrite, 16+65500*rom->GetDiskNo() );

				if( ::fread( &ddata, sizeof(DISKDATA), 1, fp ) != 1 ) {
					// #### File reading failed.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}

				for( i = 0; i < ddata.DifferentSize; i++ ) {
					if( ::fread( &pos, sizeof(DWORD), 1, fp ) != 1 ) {
						// #### Failed to load file.
						throw	CApp::GetErrorString( IDS_ERROR_READ );
					}
					data = (BYTE)(pos>>24);
					pos &= 0x00FFFFFF;
					if( pos >= 16 && pos < (DWORD)DiskSize ) {
						lpDisk[pos] = data;
						lpWrite[pos] = 0xFF;
					}
				}

				}
				break;

			// EXCTR STATE
			case  7:
				{
				EXCTRSTAT exctr;
				if( ::fread( &exctr, sizeof(EXCTRSTAT), 1, fp ) != 1 ) {
					// #### The file could not be read successfully.
					throw CApp::GetErrorString( IDS_ERROR_READ );
				}

				pad->SetSyncExData( exctr.data );
				}
				break;
		}
	}

	return	TRUE;
}

void	NES::WriteState( FILE* fp )
{
	INT	i;

	// HEADER
	{
	FILEHDR2 hdr;

	ZEROMEMORY( &hdr, sizeof(FILEHDR2) );

	::memcpy( hdr.ID, VIRTUALNES_ST_SIGN, sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0200;

	if( rom->GetMapperNo() != 20 ) {
		hdr.Ext0 = rom->GetPROM_CRC();
	} else {
		hdr.Ext0 = rom->GetGameID();
		hdr.Ext1 = (WORD)rom->GetMakerID();
		hdr.Ext2 = (WORD)rom->GetDiskNo();
	}

	// #### If playing a movie or recording, record the position.
	if( m_bMoviePlay || m_bMovieRec ) {
		hdr.MovieStep   = m_MovieStep;
		hdr.MovieOffset = ::ftell( m_fpMovie );
//DEBUGOUT( "\nSV STEP=%d POS=%d\n", m_MovieStep, hdr.MovieOffset );
	}

	// Write File
	if( ::fwrite( &hdr, sizeof(FILEHDR2), 1, fp ) != 1 )
		// #### File write failure.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}

	BLOCKHDR hdr;

	// REGISTER STATE
	{
	REGSTAT	reg;

	ZEROMEMORY( &hdr, sizeof(BLOCKHDR) );
	ZEROMEMORY( &reg, sizeof(REGSTAT) );

	// Create Header
	::memcpy( hdr.ID, "REG DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0210;
	hdr.BlockSize    = sizeof(REGSTAT);

	// SAVE CPU STATE
	R6502	R;
	cpu->GetContext( R );

	reg.cpureg.cpu.PC = R.PC;
	reg.cpureg.cpu.A  = R.A;
	reg.cpureg.cpu.X  = R.X;
	reg.cpureg.cpu.Y  = R.Y;
	reg.cpureg.cpu.S  = R.S;
	reg.cpureg.cpu.P  = R.P;
	reg.cpureg.cpu.I  = R.INT_pending;

	INT	cycles;
	apu->GetFrameIRQ( cycles,
			  reg.cpureg.cpu.FrameIRQ_count,
			  reg.cpureg.cpu.FrameIRQ_type,
			  reg.cpureg.cpu.FrameIRQ,
			  reg.cpureg.cpu.FrameIRQ_occur );
	reg.cpureg.cpu.FrameIRQ_cycles = (LONG)cycles;	// #### Reference is an integer because...

	reg.cpureg.cpu.DMA_cycles = (LONG)cpu->GetDmaCycles();
	reg.cpureg.cpu.emul_cycles = emul_cycles;
	reg.cpureg.cpu.base_cycles = base_cycles;

	// SAVE PPU STATE
	reg.ppureg.ppu.reg0 = PPUREG[0];
	reg.ppureg.ppu.reg1 = PPUREG[1];
	reg.ppureg.ppu.reg2 = PPUREG[2];
	reg.ppureg.ppu.reg3 = PPUREG[3];
	reg.ppureg.ppu.reg7 = PPU7_Temp;
	reg.ppureg.ppu.loopy_t  = loopy_t;
	reg.ppureg.ppu.loopy_v  = loopy_v;
	reg.ppureg.ppu.loopy_x  = loopy_x;
	reg.ppureg.ppu.toggle56 = PPU56Toggle;

	// Write File
	if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
		// #### Unable to write to file.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	if( ::fwrite( &reg, sizeof(REGSTAT), 1, fp ) != 1 ) {
		// #### File write failure.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	}

	// RAM STATE
	{
	RAMSTAT	ram;
	DWORD	size = 0;

	::ZeroMemory( &hdr, sizeof(BLOCKHDR) );
	::ZeroMemory( &ram, sizeof(RAMSTAT) );

	// SAVE RAM STATE
	::memcpy( ram.RAM, RAM, sizeof(ram.RAM) );
	::memcpy( ram.BGPAL, BGPAL, sizeof(ram.BGPAL) );
	::memcpy( ram.SPPAL, SPPAL, sizeof(ram.SPPAL) );
	::memcpy( ram.SPRAM, SPRAM, sizeof(ram.SPRAM) );

	// #### Persist state across uses
	if( rom->IsSAVERAM() ) {
		size = SAVERAM_SIZE;
	}

	// Create Header
	::memcpy( hdr.ID, "RAM DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = size+sizeof(RAMSTAT);

	// Write File
	if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
		// #### File write failure.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	if( ::fwrite( &ram, sizeof(RAMSTAT), 1, fp ) != 1 ) {
		// #### Failed to write to file.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	if( rom->IsSAVERAM() ) {
		if( ::fwrite( WRAM, SAVERAM_SIZE, 1, fp ) != 1 )
			// #### File write failure.
			throw	CApp::GetErrorString( IDS_ERROR_WRITE );
		}
	}

	// BANK STATE
	{
	MMUSTAT mmu;
	DWORD	size;

	::ZeroMemory( &hdr, sizeof(BLOCKHDR) );
	::ZeroMemory( &mmu, sizeof(MMUSTAT) );

	size = 0;
	// SAVE CPU MEMORY BANK DATA
	// #### BANK0, 1, and 2 have no relation to BankSave.
	// #### Virtual Nintendo Entertainment System 0.30
	// #### Bank 3 does not use SRAM for savings.
	for( i = 3; i < 8; i++ ) {
		mmu.CPU_MEM_TYPE[i] = CPU_MEM_TYPE[i];
		mmu.CPU_MEM_PAGE[i] = CPU_MEM_PAGE[i];

		if( CPU_MEM_TYPE[i] == BANKTYPE_RAM
		 || CPU_MEM_TYPE[i] == BANKTYPE_DRAM ) {
			size += 8*1024;	// 8K BANK
		}
	}

	// SAVE VRAM MEMORY DATA
	for( i = 0; i < 12; i++ ) {
		mmu.PPU_MEM_TYPE[i] = PPU_MEM_TYPE[i];
		mmu.PPU_MEM_PAGE[i] = PPU_MEM_PAGE[i];
	}
	size += 4*1024;	// 1K BANK x 4 (VRAM)

	for( i = 0; i < 8; i++ ) {
		mmu.CRAM_USED[i] = CRAM_USED[i];
		if( CRAM_USED[i] != 0 ) {
			size += 4*1024;	// 4K BANK
		}
	}

	// Create Header
	::memcpy( hdr.ID, "MMU DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0200;
	hdr.BlockSize    = size+sizeof(MMUSTAT);

	// Write File
	if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
		// #### File write failure.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	if( ::fwrite( &mmu, sizeof(MMUSTAT), 1, fp ) != 1 ) {
		// #### Failed to write to file.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}

	// WRITE CPU RAM MEMORY BANK
	for( i = 3; i < 8; i++ ) {
		if( mmu.CPU_MEM_TYPE[i] != BANKTYPE_ROM ) {
			if( ::fwrite( CPU_MEM_BANK[i], 8*1024, 1, fp ) != 1 ) {
				// #### Failed to write to file.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
		}
	}
	// #### 4KiB
	if( ::fwrite( VRAM, 4*1024, 1, fp ) != 1 ) {
		// #### File write failure.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}

	// WRITE CRAM MEMORY
	for( i = 0; i < 8; i++ ) {
		if( CRAM_USED[i] != 0 ) {
			if( ::fwrite( &CRAM[0x1000*i], 4*1024, 1, fp ) != 1 ) {
				// #### File write failure
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
		}
	}
	}

	// MMC STATE
	{
	MMCSTAT	mmc;

	::ZeroMemory( &hdr, sizeof(BLOCKHDR) );
	::ZeroMemory( &mmc, sizeof(MMCSTAT) );

	// Create Header
	::memcpy( hdr.ID, "MMC DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = sizeof(MMCSTAT);

	if( mapper->IsStateSave() ) {
		mapper->SaveState( mmc.mmcdata );
		// Write File
		if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
			// ####  failed to write to file 
			throw	CApp::GetErrorString( IDS_ERROR_WRITE );
		}
		if( ::fwrite( &mmc, sizeof(MMCSTAT), 1, fp ) != 1 ) {
			// #### File write failure.
			throw	CApp::GetErrorString( IDS_ERROR_WRITE );
		}
	}
	}

	// CONTROLLER STATE
	{
	CTRSTAT	ctr;

	::ZeroMemory( &hdr, sizeof(BLOCKHDR) );
	::ZeroMemory( &ctr, sizeof(CTRSTAT) );

	// Create Header
	::memcpy( hdr.ID, "CTR DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = sizeof(CTRSTAT);

	ctr.ctrreg.ctr.pad1bit = pad->pad1bit;
	ctr.ctrreg.ctr.pad2bit = pad->pad2bit;
	ctr.ctrreg.ctr.pad3bit = pad->pad3bit;
	ctr.ctrreg.ctr.pad4bit = pad->pad4bit;
	ctr.ctrreg.ctr.strobe  = pad->GetStrobe()?0xFF:0;
//DEBUGOUT( "SV pad1bit=%08X Strobe=%d\n", pad->pad1bit, pad->GetStrobe()?1:0 );

	if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
		// #### Failed to write to file.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}

	if( ::fwrite( &ctr, sizeof(CTRSTAT), 1, fp ) != 1 ) {
		// #### File write failure
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	}

	// SND STATE
	{
	SNDSTAT	snd;

	::ZeroMemory( &hdr, sizeof(BLOCKHDR) );
	::ZeroMemory( &snd, sizeof(SNDSTAT) );

	// Create Header
	::memcpy( hdr.ID, "SND DATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0100;
	hdr.BlockSize    = sizeof(SNDSTAT);

	apu->SaveState( snd.snddata );

	if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
		// #### File write failure.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}

	if( ::fwrite( &snd, sizeof(SNDSTAT), 1, fp ) != 1 ) {
		// #### Failed to write to file.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	}

	// DISKIMAGE STATE
	if( rom->GetMapperNo() == 20 )
	{
	DISKDATA dsk;
	LPBYTE	lpDisk  = rom->GetPROM();
	LPBYTE	lpWrite = rom->GetDISK();
	LONG	DiskSize = 16+65500*rom->GetDiskNo();
	DWORD	data;

	::ZeroMemory( &hdr, sizeof(BLOCKHDR) );
	::ZeroMemory( &dsk, sizeof(DISKDATA) );

	// #### Count the difference in numbers.
	for( i = 16; i < DiskSize; i++ ) {
		if( lpWrite[i] )
			dsk.DifferentSize++;
	}

	::memcpy( hdr.ID, "DISKDATA", sizeof(hdr.ID) );
	hdr.BlockVersion = 0x0210;
	hdr.BlockSize    = 0;

	// Write File
	if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
		// #### Failed to write to file.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}
	// Write File
	if( ::fwrite( &dsk, sizeof(DISKDATA), 1, fp ) != 1 ) {
		// #### Failed to write to file.
		throw	CApp::GetErrorString( IDS_ERROR_WRITE );
	}

	for( i = 16; i < DiskSize; i++ ) {
		if( lpWrite[i] ) {
			data = i & 0x00FFFFFF;
			data |= ((DWORD)lpDisk[i]&0xFF)<<24;

			// Write File
			if( ::fwrite( &data, sizeof(DWORD), 1, fp ) != 1 ) {
				// #### File writing failed.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
		}
	}
	}

	// EXCTR STATE
	if( pad->GetExController() ) {
		EXCTRSTAT exctr;

		::ZeroMemory( &hdr, sizeof(BLOCKHDR) );
		::ZeroMemory( &exctr, sizeof(EXCTRSTAT) );

		// Create Header
		::memcpy( hdr.ID, "EXCTRDAT", sizeof(hdr.ID) );
		hdr.BlockVersion = 0x0100;
		hdr.BlockSize    = sizeof(EXCTRSTAT);

		// Some excontrollers will default 0
		exctr.data = pad->GetSyncExData();

		if( ::fwrite( &hdr, sizeof(BLOCKHDR), 1, fp ) != 1 ) {
			// #### Failed to write to file.
			throw CApp::GetErrorString( IDS_ERROR_WRITE );
		}

		if( ::fwrite( &exctr, sizeof(EXCTRSTAT), 1, fp ) != 1 ) {
			// #### File write failure.
			throw CApp::GetErrorString( IDS_ERROR_WRITE );
		}
	}
}

INT	NES::GetDiskNo()
{
	return	rom->GetDiskNo();
}

void	NES::SoundSetup()
{
	apu->SoundSetup();
}

void	NES::Command( NESCOMMAND cmd )
{
	CommandParam( cmd, 0 );
}

BOOL	NES::CommandParam( NESCOMMAND cmd, INT param )
{
	switch( cmd ) {
		case	NESCMD_NONE:
			break;
		case	NESCMD_DISK_THROTTLE_ON:
			if( Config.emulator.bDiskThrottle ) {
				m_bDiskThrottle = TRUE;
			}
			break;
		case	NESCMD_DISK_THROTTLE_OFF:
			m_bDiskThrottle = FALSE;
			break;
		case	NESCMD_DISK_EJECT:
			mapper->ExCmdWrite( Mapper::EXCMDWR_DISKEJECT, 0 );
			m_CommandRequest = (INT)cmd;
			break;
		case	NESCMD_DISK_0A:
			if( rom->GetDiskNo() > 0 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 0 );
				m_CommandRequest = (INT)cmd;
			}
			break;
		case	NESCMD_DISK_0B:
			if( rom->GetDiskNo() > 1 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 1 );
				m_CommandRequest = (INT)cmd;
			}
			break;
		case	NESCMD_DISK_1A:
			if( rom->GetDiskNo() > 2 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 2 );
				m_CommandRequest = (INT)cmd;
			}
			break;
		case	NESCMD_DISK_1B:
			if( rom->GetDiskNo() > 3 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 3 );
				m_CommandRequest = (INT)cmd;
			}
			break;
		case	NESCMD_DISK_2A:
			if( rom->GetDiskNo() > 4 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 4 );
				m_CommandRequest = (INT)cmd;
			}
			break;
		case	NESCMD_DISK_2B:
			if( rom->GetDiskNo() > 5 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 5 );
				m_CommandRequest = (INT)cmd;
			}
			break;
		case	NESCMD_DISK_3A:
			if( rom->GetDiskNo() > 6 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 6 );
				m_CommandRequest = (INT)cmd;
			}
			break;
		case	NESCMD_DISK_3B:
			if( rom->GetDiskNo() > 7 ) {
				mapper->ExCmdWrite( Mapper::EXCMDWR_DISKINSERT, 7 );
				m_CommandRequest = (INT)cmd;
			}
			break;

		case	NESCMD_HWRESET:
			Reset();
			m_CommandRequest = (INT)cmd;
			break;
		case	NESCMD_SWRESET:
			SoftReset();
			m_CommandRequest = (INT)cmd;
			break;

		case	NESCMD_EXCONTROLLER:
			pad->SetExController( param&0xFF );
			m_CommandRequest = 0x0100|(param&0xFF);
			break;

		case	NESCMD_SOUND_MUTE:
			return	apu->SetChannelMute( (BOOL)param ); // #### Return value is the mutation state after changes
	}

	return	TRUE;
}

BOOL	NES::Snapshot()
{
FILE*	fp = NULL;

	try {
		SYSTEMTIME	now;
		::GetLocalTime( &now );

		CHAR	name[_MAX_PATH];

		if( !Config.emulator.bPNGsnapshot ) {
			sprintf( name, "%s %04d%02d%02d%02d%02d%02d%01d.bmp", rom->GetRomName(),
				now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds/100 );
		} else {
			sprintf( name, "%s %04d%02d%02d%02d%02d%02d%01d.png", rom->GetRomName(),
				now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute, now.wSecond, now.wMilliseconds/100 );
		}

		string	pathstr, tempstr;
		if( Config.path.bSnapshotPath ) {
			pathstr = CPathlib::CreatePath( CApp::GetModulePath(), Config.path.szSnapshotPath );
			::CreateDirectory( pathstr.c_str(), NULL );
		} else {
			pathstr = rom->GetRomPath();
		}
		tempstr = CPathlib::MakePath( pathstr.c_str(), name );
		DEBUGOUT( "Snapshot: %s\n", tempstr.c_str() );

		if( !Config.emulator.bPNGsnapshot ) {
			if( !(fp = ::fopen( tempstr.c_str(), "wb" )) ) {
				// #### Sorry, but I'm just an AI and do not have the ability to open files. Can you please provide more context or clarify which file you are trying to open?
				LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
				sprintf( szErrorString, szErrStr, tempstr.c_str() );
				throw	szErrorString;
			}

			LPBYTE	lpScn = ppu->GetScreenPtr();

			BITMAPFILEHEADER bfh;
			BITMAPINFOHEADER bih;
			RGBQUAD		 rgb[256];

			ZEROMEMORY( &bfh, sizeof(bfh) );
			ZEROMEMORY( &bih, sizeof(bih) );
			ZEROMEMORY( rgb, sizeof(rgb) );

			bfh.bfType = 0x4D42;	// 'BM'
			bfh.bfOffBits = sizeof(BITMAPFILEHEADER)+sizeof(BITMAPINFOHEADER)+sizeof(RGBQUAD)*256;
			bfh.bfSize = bfh.bfOffBits+256*240;

			bih.biSize          = sizeof(bih);
			bih.biWidth         = 256;
			bih.biHeight        = 240;
			bih.biPlanes        = 1;
			bih.biBitCount      = 8;
			bih.biCompression   = BI_RGB;
			bih.biSizeImage     = 0;
			bih.biXPelsPerMeter = 0;
			bih.biYPelsPerMeter = 0;
			bih.biClrUsed       = 256;
			bih.biClrImportant  = 0;

			DirectDraw.GetPaletteData( rgb );

			if( ::fwrite( &bfh, sizeof(bfh), 1, fp ) != 1 ) {
				// #### File write failure.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
			if( ::fwrite( &bih, sizeof(bih), 1, fp ) != 1 ) {
				// #### File writing failed.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
			if( ::fwrite( &rgb, sizeof(rgb), 1, fp ) != 1 ) {
				// ####  failed to write to file
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}

			lpScn += 8;
			for( INT i = 239; i >= 0; i-- ) {
				if( ::fwrite( &lpScn[(256+16)*i], 256, 1, fp ) != 1 ) {
					// #### Failed to write to file.
					throw	CApp::GetErrorString( IDS_ERROR_WRITE );
				}
			}

			FCLOSE( fp );
		} else {
			LPBYTE	lpScn = ppu->GetScreenPtr();
			RGBQUAD		 rgb[256];
			ZEROMEMORY( rgb, sizeof(rgb) );
			DirectDraw.GetPaletteData( rgb );

			PNGWRITE png;

			png.Write( tempstr.c_str(), 256, 240, rgb, lpScn+8, CDirectDraw::RENDER_WIDTH );
		}
	} catch( CHAR* str ) {
		DEBUGOUT( "Snapshot error.\n" );
		FCLOSE( fp );
		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		DEBUGOUT( "Snapshot error.\n" );
		FCLOSE( fp );
		// #### An unclear error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}

	return	TRUE;
}

INT	NES::IsMovieFile( const char* fname, ROM* rom )
{
FILE*	fp = NULL;
MOVIEFILEHDR	header;

	if( !(fp = ::fopen( fname, "rb" )) )
		return	-1;

	if( ::fread( &header, sizeof(header), 1, fp ) != 1 ) {
		FCLOSE( fp );
		return	-1;
	}
	FCLOSE( fp );

	if( ::memcmp( header.ID, VIRTUALNES_MV_SIGN, sizeof(header.ID) ) == 0 ) {
		if( header.BlockVersion < 0x0300 ) {
			return	IDS_ERROR_ILLEGALMOVIEOLD;
		} else 
		if( header.BlockVersion >= 0x0300 ) {
			if( rom->GetMapperNo() != 20 ) {
			// #### outside of Federated Deadline System
				if( header.Ext0 != rom->GetPROM_CRC() ) {
					return	IDS_ERROR_ILLEGALMOVIECRC;	// #### other
				}
			} else {
			// FDS
				if( header.Ext0 != rom->GetGameID() ||
				    header.Ext1 != (WORD)rom->GetMakerID() ||
				    header.Ext2 != (WORD)rom->GetDiskNo() )
					return	IDS_ERROR_ILLEGALMOVIECRC;	// #### It's not the same
			}

			if( header.RecordVersion != VIRTUALNES_VERSION ) {
				return	IDS_ERROR_ILLEGALMOVIEVER;
			}

			return	0;
		}
	}

	return	-1;
}

BOOL	NES::MoviePlay( const char* fname )
{
	if( rom->IsNSF() )
		return	FALSE;

	if( IsMoviePlay() || IsMovieRec() ) {
		MovieStop();
	}

DEBUGOUT( "NES::MoviePlay\n" );

	try {
		if( !(m_fpMovie = ::fopen( fname, "rb+" )) ) {
			DEBUGOUT( "Movie play error. File not found.\n" );
			// #### File not found.
			return	FALSE;
		}

		// #### absorption
		if( ::fread( &m_hedMovie, sizeof(m_hedMovie), 1, m_fpMovie ) != 1 ) {
			// #### File read failure.
			throw	CApp::GetErrorString( IDS_ERROR_READ );
		}

		if( ::memcmp( m_hedMovie.ID, VIRTUALNES_MV_SIGN, sizeof(m_hedMovie.ID) ) == 0 ) {
			m_MovieVersion = m_hedMovie.BlockVersion;

//			if( m_hedMovie.BlockVersion == 0x0300 ) {
			if( m_hedMovie.BlockVersion >= 0x0300 ) {
				if( m_hedMovie.CRC != 0 ) {
					if( CRC::Crc( sizeof(m_hedMovie)-sizeof(DWORD), (LPBYTE)&m_hedMovie ) != m_hedMovie.CRC ) {
						FCLOSE( m_fpMovie );
						return	FALSE;	// #### You're right, it's definitely different
					}
				}
				// #### Elder sister
			} else {
				// #### Oh dear!
				FCLOSE( m_fpMovie );
				return	FALSE;
			}
		}

		// #### game-specific options
		m_saveRenderMethod = (INT)GetRenderMethod();
		m_saveIrqType      = GetIrqType();
		m_saveFrameIRQ     = GetFrameIRQmode();
		m_saveVideoMode    = GetVideoMode();
		SetRenderMethod( (RENDERMETHOD)m_hedMovie.RenderMethod );
		SetIrqType( (INT)m_hedMovie.IRQtype );
		SetFrameIRQmode( (m_hedMovie.FrameIRQ!=0)?TRUE:FALSE );
		SetVideoMode( (m_hedMovie.VideoMode!=0)?TRUE:FALSE );

		LONG	MovieOffset;
		m_MovieControl   = m_hedMovie.Control;
		m_MovieStepTotal = m_hedMovie.MovieStep;
		MovieOffset      = m_hedMovie.MovieOffset;

		if( m_hedMovie.BlockVersion < 0x0400 ) {
			// #### stochastic learning
			ReadState( m_fpMovie );
		} else if( !(m_MovieControl & 0x40) ) {
			// #### data analysis
			ReadState( m_fpMovie );
		} else {
			Reset();
		}

		if( ::fseek( m_fpMovie, MovieOffset, SEEK_SET ) ) {
			// #### File reading failed.
			throw	CApp::GetErrorString( IDS_ERROR_READ );
		}

		// #### Is there a movie recorded?
		if( m_MovieStepTotal == 0 ) {
			MovieStop();
			return	FALSE;
		}

		m_bMoviePlay = TRUE;
		m_MovieStep = 0;
	} catch( CHAR* str ) {
		DEBUGOUT( "Movie play error. %s\n", str );
		FCLOSE( m_fpMovie );
		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		DEBUGOUT( "Movie play error.\n" );
		FCLOSE( m_fpMovie );
		// #### An uncertain error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}

	return	TRUE;
}

BOOL	NES::MovieRec( const char* fname )
{
	if( rom->IsNSF() )
		return	FALSE;

	if( IsMoviePlay() || IsMovieRec() ) {
		MovieStop();
	}

DEBUGOUT( "NES::MovieRec\n" );

	try {
		if( !(m_fpMovie = ::fopen( fname, "wb" )) ) {
			// #### I can't open the xxx file.
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			sprintf( szErrorString, szErrStr, fname );
			throw	szErrorString;
		}
		::ZeroMemory( &m_hedMovie, sizeof(m_hedMovie) );
		::memcpy( m_hedMovie.ID, VIRTUALNES_MV_SIGN, sizeof(m_hedMovie.ID) );
//		m_hedMovie.BlockVersion = 0x0300;
		m_hedMovie.BlockVersion = 0x0400;
		m_hedMovie.RecordVersion = VIRTUALNES_VERSION;

		m_hedMovie.StateStOffset = sizeof(m_hedMovie);

		m_hedMovie.Control |= Config.movie.bUsePlayer[0]?0x01:0x00;
		m_hedMovie.Control |= Config.movie.bUsePlayer[1]?0x02:0x00;
		m_hedMovie.Control |= Config.movie.bUsePlayer[2]?0x04:0x00;
		m_hedMovie.Control |= Config.movie.bUsePlayer[3]?0x08:0x00;
		m_hedMovie.Control |= Config.movie.bResetRec?0x40:0x00;
		m_hedMovie.Control |= Config.movie.bRerecord?0x80:0x00;
		m_MovieControl = m_hedMovie.Control;

		// #### Game specific options
		m_hedMovie.RenderMethod = (BYTE)GetRenderMethod();
		m_hedMovie.IRQtype      = (BYTE)GetIrqType();
		m_hedMovie.FrameIRQ     = GetFrameIRQmode()?0xFF:0;
		m_hedMovie.VideoMode    = GetVideoMode()?0xFF:0;

		// #### ID
		if( rom->GetMapperNo() != 20 ) {
		// #### Not part of FDS
			m_hedMovie.Ext0 = rom->GetPROM_CRC();
		} else {
		// FDS
			m_hedMovie.Ext0 =       rom->GetGameID();
			m_hedMovie.Ext1 = (WORD)rom->GetMakerID();
			m_hedMovie.Ext2 = (WORD)rom->GetDiskNo();
		}

		// #### pretend
		if( ::fwrite( &m_hedMovie, sizeof(m_hedMovie), 1, m_fpMovie ) != 1 ) {
			FCLOSE( m_fpMovie );
			// #### File write failure.
			throw	CApp::GetErrorString( IDS_ERROR_WRITE );
		}

		if( Config.movie.bResetRec ) {
			Reset();	// #### Hardware Reset from Record Start
		} else {
			// #### practice writing
			WriteState( m_fpMovie );
		}

		m_hedMovie.MovieOffset = ::ftell( m_fpMovie );
		m_bMovieRec = TRUE;
		m_MovieStep = m_MovieStepTotal = 0;
//		m_MovieVersion = 0x0300;
		m_MovieVersion = 0x0400;
	} catch( CHAR* str ) {
		DEBUGOUT( "Movie record error. %s\n", str );
		FCLOSE( m_fpMovie );
		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		DEBUGOUT( "Movie record error.\n" );
		FCLOSE( m_fpMovie );
		// #### Unclear error occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}

	return	TRUE;
}

BOOL	NES::MovieRecAppend( const char* fname )
{
	if( rom->IsNSF() )
		return	FALSE;

	// ####  Recorded but no meaning
	if( IsMovieRec() )
		return	FALSE;

	if( IsMoviePlay() ) {
		MovieStop();
	}

DEBUGOUT( "NES::MovieAppendRec\n" );

	try {
		if( !(m_fpMovie = ::fopen( fname, "rb" )) ) {
			// #### When there is no file.
			if( !(m_fpMovie = ::fopen( fname, "wb" )) ) {
				// #### I can't open the xxx file.
				LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
				sprintf( szErrorString, szErrStr, fname );
				throw	szErrorString;
			}

			::ZeroMemory( &m_hedMovie, sizeof(m_hedMovie) );
			::memcpy( m_hedMovie.ID, VIRTUALNES_MV_SIGN, sizeof(m_hedMovie.ID) );
//			m_hedMovie.BlockVersion = 0x0300;
			m_hedMovie.BlockVersion = 0x0400;
			m_hedMovie.RecordVersion = VIRTUALNES_VERSION;
			m_hedMovie.StateStOffset = sizeof(m_hedMovie);

			m_hedMovie.Control |= Config.movie.bUsePlayer[0]?0x01:0x00;
			m_hedMovie.Control |= Config.movie.bUsePlayer[1]?0x02:0x00;
			m_hedMovie.Control |= Config.movie.bUsePlayer[2]?0x04:0x00;
			m_hedMovie.Control |= Config.movie.bUsePlayer[3]?0x08:0x00;
			m_hedMovie.Control |= Config.movie.bRerecord?0x80:0x00;
			m_hedMovie.Control |= Config.movie.bResetRec?0x40:0x00;
			m_MovieControl = m_hedMovie.Control;

			// #### Game-specific option (in English)
			m_hedMovie.RenderMethod = (BYTE)GetRenderMethod();
			m_hedMovie.IRQtype      = (BYTE)GetIrqType();
			m_hedMovie.FrameIRQ     = GetFrameIRQmode()?0xFF:0;
			m_hedMovie.VideoMode    = GetVideoMode()?0xFF:0;

			// #### Invalid vote
			if( ::fwrite( &m_hedMovie, sizeof(m_hedMovie), 1, m_fpMovie ) != 1 ) {
				FCLOSE( m_fpMovie );
				// #### File write failure.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}

			if( Config.movie.bResetRec ) {
				Reset();	// #### Hardware Reset Record Start
			} else {
				// #### state write-through
				WriteState( m_fpMovie );
			}
			m_hedMovie.MovieOffset = ::ftell( m_fpMovie );
			m_MovieStep = m_MovieStepTotal = 0;
//			m_MovieVersion = 0x0300;
			m_MovieVersion = 0x0400;
		} else {
			if( !(m_fpMovie = ::fopen( fname, "rb+" )) ) {
				// #### I cannot open
				LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
				sprintf( szErrorString, szErrStr, fname );
				throw	szErrorString;
			}
			// #### interpretation
			if( ::fseek( m_fpMovie, 0, SEEK_SET ) ) {
				FCLOSE( m_fpMovie );
				// #### File loading failed.
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}
			if( ::fread( &m_hedMovie, sizeof(m_hedMovie), 1, m_fpMovie ) != 1 ) {
				FCLOSE( m_fpMovie );
				// #### Error reading file
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}

			if( ::memcmp( m_hedMovie.ID, VIRTUALNES_MV_SIGN, sizeof(m_hedMovie.ID) ) != 0 ) {
				FCLOSE( m_fpMovie );
				return	FALSE;
			}
			// #### Old version, discard.
			if( m_hedMovie.BlockVersion < 0x0300 ) {
				FCLOSE( m_fpMovie );
				return	FALSE;
			}

			m_MovieControl = m_hedMovie.Control;
			m_MovieStep = m_MovieStepTotal = m_hedMovie.MovieStep;
//			m_MovieVersion = 0x0300;
			m_MovieVersion = 0x0400;

			if( ::fseek( m_fpMovie, m_hedMovie.StateEdOffset, SEEK_SET ) ) {
				FCLOSE( m_fpMovie );
				// #### File reading failed.
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}
			if( !ReadState( m_fpMovie ) ) {
				FCLOSE( m_fpMovie );
				// #### File loading failed.
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}
			if( ::fseek( m_fpMovie, m_hedMovie.StateEdOffset, SEEK_SET ) ) {
				FCLOSE( m_fpMovie );
				// #### File loading failed.
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}
		}
		m_bMovieRec = TRUE;
	} catch( CHAR* str ) {
		DEBUGOUT( "Movie record error. %s\n", str );
		FCLOSE( m_fpMovie );
		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		DEBUGOUT( "Movie record error.\n" );
		FCLOSE( m_fpMovie );
		// #### An unclear error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif
	}

	return	TRUE;
}

BOOL	NES::MovieStop()
{
	if( !m_fpMovie && !(m_bMoviePlay||m_bMovieRec) )
		return	FALSE;

DEBUGOUT( "NES::MovieStop\n" );

	DirectDraw.SetMessageString( "Movie stop." );

	if( m_bMovieRec ) {
		m_hedMovie.MovieStep = m_MovieStep;
		m_hedMovie.StateEdOffset = ::ftell( m_fpMovie );
		WriteState( m_fpMovie );

// #### Photography is prohibited in this area. If you are caught taking photos, you will be asked to delete them.
//		if( m_MovieControl & 0x80 ) {
//		} else {
//			m_hedMovie.StateEdOffset = 0;
//		}
		if( ::fseek( m_fpMovie, 0, SEEK_SET ) ) {
			FCLOSE( m_fpMovie );
			// #### Unable to write to file
			throw	CApp::GetErrorString( IDS_ERROR_WRITE );
		}

		// #### CRC Writing
		m_hedMovie.CRC = CRC::Crc( sizeof(m_hedMovie)-sizeof(DWORD), (LPBYTE)&m_hedMovie );

		// #### The final header writing.
		if( ::fwrite( &m_hedMovie, sizeof(m_hedMovie), 1, m_fpMovie ) != 1 ) {
			FCLOSE( m_fpMovie );
			// #### File write failure.
			throw	CApp::GetErrorString( IDS_ERROR_WRITE );
		}

		FCLOSE( m_fpMovie );
		m_bMovieRec = FALSE;
	}

	if( m_bMoviePlay ) {
		FCLOSE( m_fpMovie );
		m_bMoviePlay = FALSE;

		// #### Restore game settings to their original state
		SetRenderMethod( (RENDERMETHOD)m_saveRenderMethod );
		SetIrqType     ( m_saveIrqType );
		SetFrameIRQmode( m_saveFrameIRQ );
		SetVideoMode   ( m_saveVideoMode );
	}

	return	TRUE;
}

void	NES::GetMovieInfo( WORD& wRecVersion, WORD& wVersion, DWORD& dwRecordFrames, DWORD& dwRecordTimes )
{
	wRecVersion    = m_hedMovie.RecordVersion;
	wVersion       = m_hedMovie.BlockVersion;
	dwRecordFrames = m_hedMovie.MovieStep;
	dwRecordTimes  = m_hedMovie.RecordTimes;
}

// #### Call every frame.
void	NES::Movie()
{
	if( !m_fpMovie && !(m_bMoviePlay||m_bMovieRec) ) {
		m_CommandRequest = 0;	// #### If you don't put this in, you will die.
		return;
	}

	INT	exctr = pad->GetExController();

	BYTE	Data;
	WORD	wData;
	DWORD	dwData;

	if( m_bMovieRec ) {
		// #### If the initial expansion was set from the beginning.
		if( m_MovieStep == 0 ) {
			if( exctr == PAD::EXCONTROLLER_ZAPPER
			 || exctr == PAD::EXCONTROLLER_PADDLE
			 || exctr == PAD::EXCONTROLLER_CRAZYCLIMBER
			 || exctr == PAD::EXCONTROLLER_TOPRIDER
			 || exctr == PAD::EXCONTROLLER_SPACESHADOWGUN
			 || exctr == PAD::EXCONTROLLER_FAMILYTRAINER_A
			 || exctr == PAD::EXCONTROLLER_FAMILYTRAINER_B
			 || exctr == PAD::EXCONTROLLER_MAHJANG
			 || exctr == PAD::EXCONTROLLER_EXCITINGBOXING
			 || exctr == PAD::EXCONTROLLER_OEKAKIDS_TABLET ) {
				// #### Command ID
				Data = 0xF0;
				// #### writing
				if( ::fwrite( &Data, sizeof(Data), 1, m_fpMovie ) != 1 ) {
					MovieStop();
					// #### File write failure.
					throw	CApp::GetErrorString( IDS_ERROR_WRITE );
				}
				// #### kind
				wData = (WORD)(0x0100|(pad->GetExController()&0x0FF));
				// #### inputting
				if( ::fwrite( &wData, sizeof(wData), 1, m_fpMovie ) != 1 ) {
					MovieStop();
					// #### File write failed.
					throw	CApp::GetErrorString( IDS_ERROR_WRITE );
				}
			}
		}

		if( m_CommandRequest ) {
			// #### Command ID
			Data = 0xF0;
			// #### writing in
			if( ::fwrite( &Data, sizeof(Data), 1, m_fpMovie ) != 1 ) {
				MovieStop();
				// #### File write failure.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
			// #### command
			wData = (WORD)m_CommandRequest;
			// #### written input
			if( ::fwrite( &wData, sizeof(wData), 1, m_fpMovie ) != 1 ) {
				MovieStop();
				// #### File write failure.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
		}
		m_CommandRequest = 0;

		// #### Enlargement
		if( exctr == PAD::EXCONTROLLER_ZAPPER
		 || exctr == PAD::EXCONTROLLER_PADDLE
		 || exctr == PAD::EXCONTROLLER_CRAZYCLIMBER
		 || exctr == PAD::EXCONTROLLER_TOPRIDER
		 || exctr == PAD::EXCONTROLLER_SPACESHADOWGUN
		 || exctr == PAD::EXCONTROLLER_FAMILYTRAINER_A
		 || exctr == PAD::EXCONTROLLER_FAMILYTRAINER_B
		 || exctr == PAD::EXCONTROLLER_MAHJANG
		 || exctr == PAD::EXCONTROLLER_EXCITINGBOXING
		 || exctr == PAD::EXCONTROLLER_OEKAKIDS_TABLET ) {
			// #### Extend Controller Data ID
			Data = 0xF3;
			// #### writing
			if( ::fwrite( &Data, sizeof(Data), 1, m_fpMovie ) != 1 ) {
				MovieStop();
				// #### File writing failed.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
			// #### Data
			dwData = pad->GetSyncExData();

			// #### inserting
			if( ::fwrite( &dwData, sizeof(dwData), 1, m_fpMovie ) != 1 ) {
				MovieStop();
				// #### File write failure.
				throw	CApp::GetErrorString( IDS_ERROR_WRITE );
			}
		}

		dwData = pad->GetSyncData();
		for( INT i = 0; i < 4; i++ ) {
			Data = (BYTE)(dwData>>(i*8));
			if( m_MovieControl & (1<<i) ) {
				// #### inclusion
				if( ::fwrite( &Data, sizeof(Data), 1, m_fpMovie ) != 1 ) {
					MovieStop();
					// #### File write failure.
					throw	CApp::GetErrorString( IDS_ERROR_WRITE );
				}
			}
		}

		m_MovieStep++;
	}

	if( m_bMoviePlay ) {
		DWORD	dwPadData = 0;
		INT	num = 0;
		BYTE	PadBuf[4];

		PadBuf[0] = PadBuf[1] = PadBuf[2] = PadBuf[3] = 0;

		// #### Movie finished?
		if( m_MovieStep >= m_MovieStepTotal ) {
			if( !Config.movie.bLoopPlay ) {
				MovieStop();
				return;
			} else {
				// #### living in the moment.
				m_bMoviePlay = FALSE;
				m_MovieStep = 0;
				::fseek( m_fpMovie, m_hedMovie.StateStOffset, SEEK_SET );
				// #### state learning
				ReadState( m_fpMovie );
				::fseek( m_fpMovie, m_hedMovie.MovieOffset, SEEK_SET );
				// #### to blow something out of proportion.
				m_bMoviePlay = TRUE;
			}
		}

		do {
			// #### reading in
			if( ::fread( &Data, sizeof(Data), 1, m_fpMovie ) != 1 ) {
				// #### finished
				MovieStop();
				return;
			}

			// #### command
			if( (Data & 0xF0) == 0xF0 ) {
				if( Data == 0xF0 ) {
					// #### inputting
					if( ::fread( &wData, sizeof(wData), 1, m_fpMovie ) != 1 ) {
						// #### Ended
						MovieStop();
						return;
					}
					if( wData < 0x0100 ) {
						Command( (NESCOMMAND)((INT)wData) );
					} else {
						// #### Extension
						CommandParam( NESCMD_EXCONTROLLER, ((INT)wData) & 0x00FF );
					}
				} else 
				if( Data == 0xF3 ) {
					// #### inclusion
					if( ::fread( &dwData, sizeof(dwData), 1, m_fpMovie ) != 1 ) {
						// #### Ended
						MovieStop();
						return;
					}
					pad->SetSyncExData( dwData );
				} else {
					// #### Has it ended?
					MovieStop();
					return;
				}
			} else {
				// #### Unused player character flies away
				while( !(m_MovieControl & (1<<num)) && (num < 4) ) {
					PadBuf[num] = 0;
					num++;
				}
				PadBuf[num] = Data;
				num++;
				// #### Unused players were sent flying away
				while( !(m_MovieControl & (1<<num)) && (num < 4) ) {
					PadBuf[num] = 0;
					num++;
				}
			}
		} while( num < 4 );

		dwData = (((DWORD)PadBuf[3])<<24)|(((DWORD)PadBuf[2])<<16)|(((DWORD)PadBuf[1])<<8)|((DWORD)PadBuf[0]);
		pad->SetSyncData( dwData );

		// #### Increase the counter.
		m_MovieStep++;
	}

	m_CommandRequest = 0;
}

// For Cheat
void	NES::CheatInitial()
{
	m_CheatCode.clear();
}

BOOL	NES::IsCheatCodeAdd()
{
	BOOL bRet = m_bCheatCodeAdd;
	m_bCheatCodeAdd = FALSE;

	return	bRet;
}

INT	NES::GetCheatCodeNum()
{
	return	(int)m_CheatCode.size();
}

BOOL	NES::GetCheatCode( INT no, CHEATCODE& code )
{
	if( (int)m_CheatCode.size()-1 < no )
		return	FALSE;

	code = m_CheatCode[no];
	return	TRUE;
}

void	NES::SetCheatCodeFlag( INT no, BOOL bEnable )
{
	if((int)m_CheatCode.size()-1 < no )
		return;

	if( bEnable ) {
		m_CheatCode[no].enable |= CHEAT_ENABLE;
	} else {
		m_CheatCode[no].enable &= ~CHEAT_ENABLE;
	}
}

void	NES::SetCheatCodeAllFlag( BOOL bEnable, BOOL bKey )
{
	for( size_t i = 0; i < m_CheatCode.size(); i++ ) {
		if( !bKey ) {
			if( bEnable ) {
				m_CheatCode[i].enable |= CHEAT_ENABLE;
			} else {
				m_CheatCode[i].enable &= ~CHEAT_ENABLE;
			}
		} else if( !(m_CheatCode[i].enable&CHEAT_KEYDISABLE) ) {
			if( bEnable ) {
				m_CheatCode[i].enable |= CHEAT_ENABLE;
			} else {
				m_CheatCode[i].enable &= ~CHEAT_ENABLE;
			}
		}
	}
}

void	NES::ReplaceCheatCode( INT no, CHEATCODE code )
{
	if((int)m_CheatCode.size()-1 < no )
		return;

	m_CheatCode[no] = code;
}

void	NES::AddCheatCode( CHEATCODE code )
{
	m_CheatCode.push_back( code );
	m_bCheatCodeAdd = TRUE;
}

void	NES::DelCheatCode( INT no )
{
	if((int)m_CheatCode.size()-1 < no )
		return;

	//m_CheatCode.erase( &m_CheatCode[no] );
}

DWORD	NES::CheatRead( INT length, WORD addr )
{
	DWORD	data = 0;
	for( INT i = 0; i <= length; i++ ) {
		data |= (DWORD)Read( addr+i )*(1<<(i*8));
	}

	return	data;
}

void	NES::CheatWrite( INT length, WORD addr, DWORD data )
{
	for( INT i = 0; i <= length; i++ ) {
		Write( (WORD)(addr+i), data&0xFF );
		data >>= 8;
	}
}

void	NES::CheatCodeProcess()
{
	for( vector<CHEATCODE>::iterator it = m_CheatCode.begin(); it != m_CheatCode.end(); it++ ) {
		if( !(it->enable & CHEAT_ENABLE) )
			continue;

		switch( it->type ) {
			case	CHEAT_TYPE_ALWAYS:
				CheatWrite( it->length, it->address, it->data );
				break;
			case	CHEAT_TYPE_ONCE:
				CheatWrite( it->length, it->address, it->data );
				it->enable = 0;
				break;
			case	CHEAT_TYPE_GREATER:
				if( CheatRead( it->length, it->address ) > it->data ) {
					CheatWrite( it->length, it->address, it->data );
				}
				break;
			case	CHEAT_TYPE_LESS:
				if( CheatRead( it->length, it->address ) < it->data ) {
					CheatWrite( it->length, it->address, it->data );
				}
				break;
		}
	}
}

void	NES::GenieInitial()
{
	m_bCheatCodeAdd = FALSE;
	m_GenieCode.clear();
}

void	NES::GenieLoad( char* fname )
{
FILE*	fp = NULL;

	CHAR	buf[256];
	GENIECODE	code;
	BYTE	codetmp[9];
	INT	no;

	if( (fp = ::fopen( fname, "r" )) ) {
		m_GenieCode.clear();

		while( ::fgets( buf, sizeof(buf), fp ) ) {
			if( buf[0] == ';' )
				continue;
			if( buf[0] == 0x0D || buf[0] == 0x0A )
				continue;

			if( ::strlen( buf ) < 6 )
				continue;

			code.address = 0;
			code.data = 0;
			code.cmp = 0;

			for( no = 0; isalpha(buf[no]) && no < 8; no++ ) {
				switch( buf[no] ) {
					case	'A': codetmp[no] = 0x00; break;
					case	'P': codetmp[no] = 0x01; break;
					case	'Z': codetmp[no] = 0x02; break;
					case	'L': codetmp[no] = 0x03; break;
					case	'G': codetmp[no] = 0x04; break;
					case	'I': codetmp[no] = 0x05; break;
					case	'T': codetmp[no] = 0x06; break;
					case	'Y': codetmp[no] = 0x07; break;
					case	'E': codetmp[no] = 0x08; break;
					case	'O': codetmp[no] = 0x09; break;
					case	'X': codetmp[no] = 0x0A; break;
					case	'U': codetmp[no] = 0x0B; break;
					case	'K': codetmp[no] = 0x0C; break;
					case	'S': codetmp[no] = 0x0D; break;
					case	'V': codetmp[no] = 0x0E; break;
					case	'N': codetmp[no] = 0x0F; break;
				}
			}

			if( no == 6 ) {
				// Address
				code.address |= (WORD)(codetmp[3] & 0x07)<<12;
				code.address |= (WORD)(codetmp[4] & 0x08)<< 8;
				code.address |= (WORD)(codetmp[5] & 0x07)<< 8;
				code.address |= (WORD)(codetmp[1] & 0x08)<< 4;
				code.address |= (WORD)(codetmp[2] & 0x07)<< 4;
				code.address |= (WORD)(codetmp[3] & 0x08);
				code.address |= (WORD)(codetmp[4] & 0x07);
				// Data
				code.data |= (codetmp[0] & 0x08)<<4;
				code.data |= (codetmp[1] & 0x07)<<4;
				code.data |= (codetmp[5] & 0x08);
				code.data |= (codetmp[0] & 0x07);

				m_GenieCode.push_back( code );
			} else
			if( no == 8 ) {
				// Address
				code.address |= 0x8000;
				code.address |= (WORD)(codetmp[3] & 0x07)<<12;
				code.address |= (WORD)(codetmp[4] & 0x08)<< 8;
				code.address |= (WORD)(codetmp[5] & 0x07)<< 8;
				code.address |= (WORD)(codetmp[1] & 0x08)<< 4;
				code.address |= (WORD)(codetmp[2] & 0x07)<< 4;
				code.address |= (WORD)(codetmp[3] & 0x08);
				code.address |= (WORD)(codetmp[4] & 0x07);
				// Data
				code.data |= (codetmp[0] & 0x08)<<4;
				code.data |= (codetmp[1] & 0x07)<<4;
				code.data |= (codetmp[7] & 0x08);
				code.data |= (codetmp[0] & 0x07);
				// Data
				code.cmp  |= (codetmp[6] & 0x08)<<4;
				code.cmp  |= (codetmp[7] & 0x07)<<4;
				code.cmp  |= (codetmp[5] & 0x08);
				code.cmp  |= (codetmp[6] & 0x07);

				m_GenieCode.push_back( code );
			}
		}

		GenieCodeProcess();
	}

	FCLOSE( fp );
}

void	NES::GenieCodeProcess()
{
	WORD	addr;

	for( size_t i = 0; i < m_GenieCode.size(); i++ ) {
		addr = m_GenieCode[i].address;
		if( addr & 0x8000 ) {
		// 8character codes
			if( CPU_MEM_BANK[addr>>13][addr&0x1FFF] == m_GenieCode[i].cmp ) {
				CPU_MEM_BANK[addr>>13][addr&0x1FFF] = m_GenieCode[i].data;
			}
		} else {
		// 6character codes
			addr |= 0x8000;
			CPU_MEM_BANK[addr>>13][addr&0x1FFF] = m_GenieCode[i].data;
		}
	}
}

void	NES::DrawPad()
{
	if( m_bMoviePlay ) {
		INT	offset_h = 12;
		INT	offset_v = Config.graphics.bAllLine?(240-18):(240-22);

		if( Config.movie.bPadDisplay ) {

			DWORD	dwData = pad->GetSyncData();
			for( INT i = 0; i < 4; i++ ) {
				BYTE	Data = (BYTE)(dwData>>(i*8));
				if( m_MovieControl & (1<<i) ) {
					DrawBitmap( offset_h, offset_v, m_PadImg );

					// KEY
					if( Data&(1<<4) ) DrawBitmap( offset_h+3, offset_v+1, m_KeyImg0 ); // U
					if( Data&(1<<5) ) DrawBitmap( offset_h+3, offset_v+5, m_KeyImg0 ); // D
					if( Data&(1<<6) ) DrawBitmap( offset_h+1, offset_v+3, m_KeyImg0 ); // L
					if( Data&(1<<7) ) DrawBitmap( offset_h+5, offset_v+3, m_KeyImg0 ); // R

					// START,SELECT
					if( Data&(1<<2) ) DrawBitmap( offset_h+ 9, offset_v+5, m_KeyImg1 ); // SELECT
					if( Data&(1<<3) ) DrawBitmap( offset_h+13, offset_v+5, m_KeyImg1 ); // START

					// A,B
					if( Data&(1<<0) ) DrawBitmap( offset_h+23, offset_v+3, m_KeyImg2 ); // A
					if( Data&(1<<1) ) DrawBitmap( offset_h+18, offset_v+3, m_KeyImg2 ); // B

					offset_h += 30;
				}
			}
		}

		if( Config.movie.bTimeDisplay ) {
			// Time display
			INT	t = m_MovieStep;
			INT	h = t / 216000;
			t -= h * 216000;
			INT	m = t / 3600;
			t -= m * 3600;
			INT	s = t / 60;
			t -= s * 60;

			CHAR	szTemp[64];
			sprintf( szTemp, "%02d:%02d:%02d.%02d", h, m, s, t * 100 / 60 );
			DrawString( 256-80+0, offset_v-1, szTemp, 0x1F );
			DrawString( 256-80+0, offset_v+1, szTemp, 0x1F );
			DrawString( 256-80-1, offset_v+0, szTemp, 0x1F );
			DrawString( 256-80+1, offset_v+0, szTemp, 0x1F );
			DrawString( 256-80, offset_v, szTemp, 0x30 );
		}
	}
}

void	NES::DrawBitmap( INT x, INT y, LPBYTE lpBitmap )
{
INT	i, j;
INT	h, v;
LPBYTE	pScn = ppu->GetScreenPtr()+8+(256+16)*y+x;
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

// TapeDevice
BOOL	NES::TapePlay( const char* fname )
{
	if( rom->IsNSF() )
		return	FALSE;

	if( IsTapePlay() || IsTapeRec() ) {
		TapeStop();
	}

DEBUGOUT( "NES::TapePlay\n" );

	if( !(m_fpTape = ::fopen( fname, "rb" )) ) {
		DEBUGOUT( "Tape play error. File not found.\n" );
		// #### File is missing.
		return	FALSE;
	}

	m_bTapePlay  = TRUE;
	m_TapeCycles = 0;
	m_TapeOut    = 0;

	cpu->SetClockProcess( TRUE );

	return	TRUE;
}

BOOL	NES::TapeRec( const char* fname )
{
	if( rom->IsNSF() )
		return	FALSE;

	if( IsTapePlay() || IsTapeRec() ) {
		TapeStop();
	}

DEBUGOUT( "NES::TapeRec\n" );

	if( !(m_fpTape = ::fopen( fname, "wb" )) ) {
		DEBUGOUT( "Tape rec error. File not found.\n" );
		// #### File no de arimasu.
		return	FALSE;
	}

	m_bTapeRec   = TRUE;
	m_TapeCycles = 0;
	m_TapeIn     = 0;

	cpu->SetClockProcess( TRUE );

	return	TRUE;
}

void	NES::TapeStop()
{
	DirectDraw.SetMessageString( "Tape stop." );

	if( !m_bBarcode ) {
		cpu->SetClockProcess( FALSE );
	}

	m_bTapePlay = m_bTapeRec = FALSE;
	FCLOSE( m_fpTape );
}

void	NES::Tape( INT cycles )
{
	if( !(IsTapePlay() || IsTapeRec()) ) {
		return;
	}

	if( (m_TapeCycles-=(double)cycles) > 0 )
		return;

	m_TapeCycles += (nescfg->CpuClock / 32000.0);
// #### The code is calculating the number of cycles required for a CPU instruction sequence, based on the clock speed of the processor and the number of instructions in the sequence. Here's a breakdown of the code:  * `m_TapeCycles`: This is a variable that stores the total number of cycles required for the CPU instruction sequence. It is initialized to 0 at the beginning of the function. * `nescfg->CpuClock`: This is a pointer to a configuration structure that contains information about the processor, including its clock speed. The clock speed is stored in the `CpuClock` member of the structure. * `(nescfg->CpuClock / 22050.0)`: This expression calculates the number of cycles required for one instruction in the CPU. The formula is `CpuClock / 22050.0`, where `22050.0` is a constant that represents the number of cycles per second of the processor. * `+=` : This operator adds the result of the previous expression to the value of `m_TapeCycles`.  So, the code calculates the number of cycles required for each instruction in the CPU sequence by dividing the clock speed of the processor by 22050.0, and then adding the result to the initial value of `m_TapeCycles`. This way, the total number of cycles required for the entire sequence is calculated.

	if( m_bTapePlay ) {
		INT	data = ::fgetc( m_fpTape );
		if( data != EOF ) {
			if( (data&0xFF) >= 0x8C ) {
				m_TapeOut = 0x02;
			} else 
			if( (data&0xFF) <= 0x74 ) {
				m_TapeOut = 0x00;
			}
		} else {
			TapeStop();
		}
	}
	if( m_bTapeRec ) {
		::fputc( (int)((m_TapeIn&7)==7)?0x90:0x70, m_fpTape );
	}
}

void	NES::Barcode( INT cycles )
{
	if( m_bBarcode ) {
		if( (m_BarcodeCycles+=cycles) > 1000 ) {
			m_BarcodeCycles = 0;
			// #### halt
			if( m_BarcodeData[m_BarcodePtr] != 0xFF ) {
				m_BarcodeOut = m_BarcodeData[m_BarcodePtr++];
			} else {
				m_bBarcode   = FALSE;
				m_BarcodeOut = 0;
DEBUGOUT( "Barcode data trasnfer complete!!\n" );

				if( !(IsTapePlay() || IsTapeRec()) ) {
					cpu->SetClockProcess( FALSE );
				}
			}
		}
	}
}

void	NES::SetBarcodeData( LPBYTE code, INT len )
{
	if( rom->GetPROM_CRC() == 0x67898319 ) {	// Barcode World (J)
		SetBarcode2Data( code, len );
		return;
	}

	DEBUGOUT( "NES::SetBarcodeData code=%s len=%d\n", code, len );

	bool	prefix_parity_type[10][6] = {
		{0,0,0,0,0,0}, {0,0,1,0,1,1}, {0,0,1,1,0,1}, {0,0,1,1,1,0},
		{0,1,0,0,1,1}, {0,1,1,0,0,1}, {0,1,1,1,0,0}, {0,1,0,1,0,1},
		{0,1,0,1,1,0}, {0,1,1,0,1,0}
	};
	bool	data_left_odd[10][7] = {
		{0,0,0,1,1,0,1}, {0,0,1,1,0,0,1}, {0,0,1,0,0,1,1}, {0,1,1,1,1,0,1},
		{0,1,0,0,0,1,1}, {0,1,1,0,0,0,1}, {0,1,0,1,1,1,1}, {0,1,1,1,0,1,1},
		{0,1,1,0,1,1,1}, {0,0,0,1,0,1,1}
	};
	bool	data_left_even[10][7] = {
		{0,1,0,0,1,1,1}, {0,1,1,0,0,1,1}, {0,0,1,1,0,1,1}, {0,1,0,0,0,0,1},
		{0,0,1,1,1,0,1}, {0,1,1,1,0,0,1}, {0,0,0,0,1,0,1}, {0,0,1,0,0,0,1},
		{0,0,0,1,0,0,1}, {0,0,1,0,1,1,1}
	};
	bool	data_right[10][7] = {
		{1,1,1,0,0,1,0}, {1,1,0,0,1,1,0}, {1,1,0,1,1,0,0}, {1,0,0,0,0,1,0},
		{1,0,1,1,1,0,0}, {1,0,0,1,1,1,0}, {1,0,1,0,0,0,0}, {1,0,0,0,1,0,0},
		{1,0,0,1,0,0,0}, {1,1,1,0,1,0,0}
	};

	INT	i, j, count = 0;;

	// #### Number conversion
	for( i = 0; i < len; i++ ) {
		code[i] = code[i]-'0';
	}

	// #### Left-handed
	for( i = 0; i < 32; i++ ) {
		m_BarcodeData[count++] = 0x08;
	}
	// #### Left Guard Bar
	m_BarcodeData[count++] = 0x00;
	m_BarcodeData[count++] = 0x08;
	m_BarcodeData[count++] = 0x00;

	if( len == 13 ) {
#if	0
		// #### recalculation
		INT	sum = 0;
		for( i = 0; i < 12; i++ ) {
			sum += (i&1)?(code[i]*3):code[i];
		}
		code[12] = (10-(sum%10))%10;
// test start
//		INT	cs = (10-(sum%10))%10;
//		if( cs == 0 ) {
//			cs = 9;
//		} else {
//			cs--;
//		}
//		code[12] = cs;
// test end
#endif
		// #### left
		for( i = 0; i < 6; i++ ) {
			if( prefix_parity_type[code[0]][i] ) {
				// #### odd parity
				for( j = 0; j < 7; j++ ) {
					m_BarcodeData[count++] = data_left_even[code[i+1]][j]?0x00:0x08;
				}
			} else {
				// #### odd parity
				for( j = 0; j < 7; j++ ) {
					m_BarcodeData[count++] = data_left_odd[code[i+1]][j]?0x00:0x08;
				}
			}
		}

		// #### Central Bar
		m_BarcodeData[count++] = 0x08;
		m_BarcodeData[count++] = 0x00;
		m_BarcodeData[count++] = 0x08;
		m_BarcodeData[count++] = 0x00;
		m_BarcodeData[count++] = 0x08;

		// #### check digit
		for( i = 7; i < 13; i++ ) {
			// #### even parity
			for( j = 0; j < 7; j++ ) {
				m_BarcodeData[count++] = data_right[code[i]][j]?0x00:0x08;
			}
		}
	} else
	if( len == 8 ) {
		// #### Check digit recalculation in English means:  Re-computing the check digit of a number or code. This is often done when there is a discrepancy between the check digit that was originally computed and the one that is required for a particular application or system. The process involves re-running the calculation to determine the correct check digit for the given value.
		INT	sum = 0;
		for( i = 0; i < 7; i++ ) {
			sum += (i&1)?code[i]:(code[i]*3);
		}
		code[7] = (10-(sum%10))%10;

		// #### Four left-handed characters
		for( i = 0; i < 4; i++ ) {
			// #### Odd-even parity
			for( j = 0; j < 7; j++ ) {
				m_BarcodeData[count++] = data_left_odd[code[i]][j]?0x00:0x08;
			}
		}

		// #### Central Bar
		m_BarcodeData[count++] = 0x08;
		m_BarcodeData[count++] = 0x00;
		m_BarcodeData[count++] = 0x08;
		m_BarcodeData[count++] = 0x00;
		m_BarcodeData[count++] = 0x08;

		// #### RIGHT SIDE THREE CHARACTERS AND CHECDIGIT
		for( i = 4; i < 8; i++ ) {
			// #### odd parity
			for( j = 0; j < 7; j++ ) {
				m_BarcodeData[count++] = data_right[code[i]][j]?0x00:0x08;
			}
		}
	}

	// #### Light Guard.
	m_BarcodeData[count++] = 0x00;
	m_BarcodeData[count++] = 0x08;
	m_BarcodeData[count++] = 0x00;
	// #### Light Manger
	for( i = 0; i < 32; i++ ) {
		m_BarcodeData[count++] = 0x08;
	}
	// #### Finished Mark.
	m_BarcodeData[count++] = 0xFF;

	// #### Transmission started.
	m_bBarcode      = TRUE;
	m_BarcodeCycles = 0;
	m_BarcodePtr    = 0;
	m_BarcodeOut    = 0x08;

	cpu->SetClockProcess( TRUE );

DEBUGOUT( "BARCODE DATA MAX:%d\n", count );
}

BYTE	NES::Barcode2( void )
{
BYTE	ret = 0x00;

	if( !m_bBarcode2 || m_Barcode2seq < 0 )
		return	ret;

	switch( m_Barcode2seq ) {
		case	0:
			m_Barcode2seq++;
			m_Barcode2ptr = 0;
			ret = 0x04;		// d3
			break;

		case	1:
			m_Barcode2seq++;
			m_Barcode2bit = m_Barcode2data[m_Barcode2ptr];
			m_Barcode2cnt = 0;
			ret = 0x04;		// d3
			break;

		case	2:
			ret = (m_Barcode2bit&0x01)?0x00:0x04;	// Bit rev.
			m_Barcode2bit >>= 1;
			if( ++m_Barcode2cnt > 7 ) {
				m_Barcode2seq++;
			}
			break;
		case	3:
			if( ++m_Barcode2ptr > 19 ) {
				m_bBarcode2 = FALSE;
				m_Barcode2seq = -1;
			} else {
				m_Barcode2seq = 1;
			}
			break;
		default:
			break;
	}

	return	ret;
}

void	NES::SetBarcode2Data( LPBYTE code, INT len )
{
	DEBUGOUT( "NES::SetBarcodeData2 code=%s len=%d\n", code, len );

	if( len < 13 )
		return;

	m_bBarcode2   = TRUE;
	m_Barcode2seq = 0;
	m_Barcode2ptr = 0;

	::strcpy( (char*)m_Barcode2data, (char*)code );

	m_Barcode2data[13] = 'S';
	m_Barcode2data[14] = 'U';
	m_Barcode2data[15] = 'N';
	m_Barcode2data[16] = 'S';
	m_Barcode2data[17] = 'O';
	m_Barcode2data[18] = 'F';
	m_Barcode2data[19] = 'T';
}

//#if	NES_PROFILER
#if	1
BYTE	NES::Font6x8[] = {
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

void	NES::DrawFont( INT x, INT y, BYTE chr, BYTE col )
{
INT	i;
LPBYTE	pFnt;
LPBYTE	pPtr;
LPBYTE	pScn = ppu->GetScreenPtr()+8;

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

void	NES::DrawString( INT x, INT y, LPSTR str, BYTE col )
{
	while( *str ) {
		DrawFont( x, y, *str, col );
		str++;
		x += 6;
	}
}

#endif
