#ifndef	__STATE_INCLUDED__
#define	__STATE_INCLUDED__

#pragma pack( push, 1 )

typedef	struct	tagFILEHDR {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES ST"
	WORD	Reserved;
	WORD	BlockVersion;
} FILEHDR, *LPFILEHDR;

// #### In VirtualNES versions 0.30 and later, use...
typedef	struct	tagFILEHDR2 {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES ST"
	WORD	Reserved;
	WORD	BlockVersion;	// #### Shift

	DWORD	Ext0;		// #### Program CRC ( Rom ): ID Program FDS
	WORD	Ext1;		// #### Maker ID
	WORD	Ext2;		// #### disk count
	LONG	MovieStep;	// #### Number of frames in a revised/edited video.
	LONG	MovieOffset;	// #### Rerecording Movie File Offset.
} FILEHDR2, *LPFILEHDR2;

typedef	struct	tagBLOCKHDR {
	BYTE	ID[8];
	WORD	Reserved;
	WORD	BlockVersion;
	DWORD	BlockSize;
} BLOCKHDR, *LPBLOCKHDR;

// #### CPU Register
// #### up to version 0x0110
typedef	struct	tagCPUSTAT_O {
	WORD	PC;
	BYTE	A;
	BYTE	X;
	BYTE	Y;
	BYTE	S;
	BYTE	P;
	BYTE	I;	// Interrupt pending flag

	BYTE	FrameIRQ;
	BYTE	reserved[3];

	LONG	mod_cycles;	// #### time warping.

	// version 0x0110
	SQWORD	emul_cycles;
	SQWORD	base_cycles;
} CPUSTAT_O, *LPCPUSTAT_O;

// version 0x0210
typedef	struct	tagCPUSTAT {
	WORD	PC;
	BYTE	A;
	BYTE	X;
	BYTE	Y;
	BYTE	S;
	BYTE	P;
	BYTE	I;	// Interrupt pending flag

	BYTE	FrameIRQ;
	BYTE	FrameIRQ_occur;
	BYTE	FrameIRQ_count;
	BYTE	FrameIRQ_type;
	LONG	FrameIRQ_cycles;

	LONG	DMA_cycles;

	SQWORD	emul_cycles;
	SQWORD	base_cycles;
} CPUSTAT, *LPCPUSTAT;

// #### Processor Unit
typedef	struct	tagPPUSTAT {
	BYTE	reg0;
	BYTE	reg1;
	BYTE	reg2;
	BYTE	reg3;
	BYTE	reg7;
	BYTE	toggle56;

	WORD	loopy_t;
	WORD	loopy_v;
	WORD	loopy_x;
} PPUSTAT, *LPPPUSTAT;

// #### APU Registers
typedef	struct	tagAPUSTAT_O {
	BYTE	reg[0x0018];
	BYTE	ext[0x0100];
} APUSTAT_O, *LPAPUSTAT_O;

// #### Controller Register
typedef	struct	tagCTRREG {
	DWORD	pad1bit;
	DWORD	pad2bit;
	DWORD	pad3bit;
	DWORD	pad4bit;
	BYTE	strobe;
} CTRREG, *LPCTRREG;

//
// #### Registered Data
// ID "REG DATA"
// #### Up to version 0x0110
typedef	struct	tagREGSTAT_O {
	union	uniCPUREG {
		BYTE	cpudata[32];
		CPUSTAT_O	cpu;
	} cpureg;
	union	uniPPUREG {
		BYTE	ppudata[32];
		PPUSTAT	ppu;
	} ppureg;
	APUSTAT_O	apu;
} REGSTAT_O, *LPREGSTAT_O;

// #### Version 0x0200 or later
typedef	struct	tagREGSTAT {
	union	uniCPUREG {
		BYTE	cpudata[64];
		CPUSTAT	cpu;
	} cpureg;
	union	uniPPUREG {
		BYTE	ppudata[32];
		PPUSTAT	ppu;
	} ppureg;
} REGSTAT, *LPREGSTAT;


//
// #### Internal RAM data
// ID "RAM DATA"
typedef	struct	tagRAMSTAT {
	BYTE	RAM[2*1024];	// Internal NES RAM
	BYTE	BGPAL[16];	// BG Palette
	BYTE	SPPAL[16];	// SP Palette
	BYTE	SPRAM[256];	// Sprite RAM
} RAMSTAT, *LPRAMSTAT;

//
// #### MMU Data (MMU stands for Memory Management Unit) 
// ID "MMU DATA"
typedef	struct	tagMMUSTAT {
	BYTE	CPU_MEM_TYPE[8];
	WORD	CPU_MEM_PAGE[8];
	BYTE	PPU_MEM_TYPE[12];
	WORD	PPU_MEM_PAGE[12];
	BYTE	CRAM_USED[8];
} MMUSTAT, *LPMMUSTAT;

//
// #### Map Data
// ID "MMC DATA"
typedef	struct	tagMMCSTAT {
	BYTE	mmcdata[256];
} MMCSTAT, *LPMMCSTAT;

//
// #### Controller Data
// ID "CTR DATA"
typedef	struct	tagCTRSTAT {
	union uniCTRDATA {
		BYTE	ctrdata[32];
		CTRREG	ctr;
	} ctrreg;
} CTRSTAT, *LPCTRSTAT;

//
// #### sound data
// ID "SND DATA"
typedef	struct	tagSNDSTAT {
	BYTE	snddata[0x800];	// 2KB
} SNDSTAT, *LPSNDSTAT;

//
// #### image disc
// #### Up to version 0.24
// ID "DSIDE 0A","DSIDE 0B","DSIDE 1A","DSIDE 1B"
typedef	struct	tagDISKSTAT {
	BYTE	DiskTouch[16];
} DISKSTAT, *LPDISKSTAT;

// #### Version 0.30 and later
// ID "DISKDATA"
typedef	struct	tagDISKDATA {
	LONG	DifferentSize;
} DISKDATA, *LPDISKDATA;

// #### The following is a disc image file used for
// #### Up to version 0.24
typedef	struct	tagDISKIMGFILEHDR {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES DI"
	WORD	BlockVersion;
	WORD	DiskNumber;
} DISKIMGFILEHDR, *LPDISKIMGFILEHDR;

typedef	struct	tagDISKIMGHDR {
	BYTE	ID[6];		// ID "SIDE0A","SIDE0B","SIDE1A","SIDE1B"
	BYTE	DiskTouch[16];
} DISKIMGHDR, *LPDISKIMGHDR;

// #### From VirtualNES version 0.30 and onwards, the following changes have been made:  1. Improved graphics: The graphics engine has been overhauled to provide smoother and more detailed graphics, including new backgrounds, sprites, and effects. 2. Enhanced sound: The sound engine has been updated to provide higher quality audio, including new sounds and music tracks. 3. New features: Several new features have been added to the game, including a built-in level editor, a password system for saving progress, and improved multiplayer functionality. 4. Bug fixes: Many bugs and issues from previous versions have been fixed, resulting in a more stable and enjoyable gaming experience. 5. Optimized performance: The game has been optimized to run faster and more smoothly, even on lower-end hardware. 6. Improved user interface: The user interface has been redesigned for improved usability and visibility, including new menus and options. 7. Support for new resolutions: VirtualNES now supports a wider range of resolutions, including 1080p and higher. 8. Enhanced performance on modern hardware: The game has been optimized to take advantage of modern hardware, resulting in faster frame rates and smoother gameplay.
typedef	struct	tagDISKFILEHDR {	//  0123456789AB
	BYTE	ID[12];		// "VirtuaNES DI"
	WORD	BlockVersion;	// 0x0200:0.30	0x0210:0.31
	WORD	Reserved;
	DWORD	ProgID;		// #### PID
	WORD	MakerID;	// #### Manufacturer ID
	WORD	DiskNo;		// #### disk number
	DWORD	DifferentSize;	// #### difference
} DISKFILEHDR, *LPDISKFILEHDR;

// ID "EXCTRDAT"
typedef	struct	tagEXCTRSTAT {
	DWORD	data;
} EXCTRSTAT, *LPEXCTRSTAT;

//
// #### Movie File
//
// #### For use with VirtualNES version 0.60 or later.
typedef	struct	tagMOVIEFILEHDR {
	BYTE	ID[12];			// "VirtuaNES MV"
	WORD	BlockVersion;		// Movie version 0x0300
	WORD	RecordVersion;		// Record version
	DWORD	Control;		// #### Control byte
					// 76543210(Bit)
					// E---4321
					// #### 1P data
					// #### secondary personal information,
					// #### 3P data
					// #### label or comment beginning with '+', followed by a horizontal rule symbol, and then the term '4P Data'
					// #### And forbidden revisions
					// #### The other controls can use 1P to 4P directional keys (any of them is fine).
					// #### When all bits are ON, the next 4 bytes become control data.
	DWORD	Ext0;			// #### The translation from Japanese to English is as follows:  ROM (Program) = FDS (Program ID)
	WORD	Ext1;			// #### Manufacturer ID
	WORD	Ext2;			// #### The translation is:  ROM: None FDS: Number of disks
	DWORD	RecordTimes;		// #### Editing Count

	BYTE	RenderMethod;		// #### Rendering method
	BYTE	IRQtype;		// #### IRQ (Interrupt Request) type 
	BYTE	FrameIRQ;		// #### Frame IRQ prohibited
	BYTE	VideoMode;		// NTSC/PAL

	BYTE	reserved2[8];		// #### reservation

	LONG	StateStOffset;		// Movie start state offset
	LONG	StateEdOffset;		// Movie end state offset
	LONG	MovieOffset;		// Movie data offset
	LONG	MovieStep;		// #### Movie Frames

	DWORD	CRC;			// #### anti-fake.
} MOVIEFILEHDR, *LPMOVIEFILEHDR;

typedef	struct	tagMOVIEIMGFILEHDRx {
	BYTE	ID[12];			// "VirtuaNES MV"
	WORD	BlockVersion;
	WORD	reserved;
	LONG	StateStOffset;		// Movie start state offset
	LONG	StateEdOffset;		// Movie end state offset
	LONG	MovieOffset;		// Movie data offset
	LONG	MovieStep;		// Movie steps
} MOVIEIMGFILEHDRx, *LPMOVIEIMGFILEHDRx;

// Famtasia Movie....
typedef	struct	tagFMVHDR {
	BYTE	ID[4];			// "FMV^Z"
	BYTE	Control1;		// #### 0: Reset after recording? 1: Record from the middle?
	BYTE	Control2;		// #### One Piece Information T: Two Piece Information
	DWORD	Unknown1;
	WORD	RecordTimes;		// #### Record Count - 1
	DWORD	Unknown2;
	BYTE	szEmulators[0x40];	// #### Recorded Emulator
	BYTE	szTitle    [0x40];	// #### label.
} FMVHDR, *LPFMVHDR;

// Nesticle Movie....
typedef	struct	tagNMVHDR {
	BYTE	ExRAM[0x2000];
	BYTE	S_RAM[0x0800];
	WORD	PC;
	BYTE	A;
	BYTE	P;
	BYTE	X;
	BYTE	Y;
	BYTE	SP;
	BYTE	OAM[0x0100];
	BYTE	VRAM[0x4000];
	BYTE	Other[0xC9];
	DWORD	ScanlineCycles;
	DWORD	VblankScanlines;
	DWORD	FrameScanlines;
	DWORD	VirtualFPS;
} NMVHDR, *LPNMVHDR;

// #### macro.
#define	SETBYTE(p,v)	{ *p = (v); p++; }
#define	SETWORD(p,v)	{ *(LPWORD)p = (v); p += sizeof(WORD); }
#define	SETDWORD(p,v)	{ *(LPDWORD)p = (v); p += sizeof(DWORD); }
#define	SETINT(p,v)	{ *(LPINT)p = (v); p += sizeof(INT); }
#define	SETBLOCK(p,v,s)	{ memcpy( p, (v), s ); p += s; }

#define	GETBYTE(p,v)	{ (v) = *p; p++; }
#define	GETWORD(p,v)	{ (v) = *(LPWORD)p; p += sizeof(WORD); }
#define	GETDWORD(p,v)	{ (v) = *(LPDWORD)p; p += sizeof(DWORD); }
#define	GETINT(p,v)	{ (v) = *(LPINT)p; p += sizeof(INT); }
#define	GETBLOCK(p,v,s)	{ memcpy( (v), p, s ); p += s; }

#pragma pack( pop )

#endif	// !__STATE_INCLUDED__
