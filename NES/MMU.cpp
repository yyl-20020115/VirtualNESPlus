//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Memory Management Unit                                      //
//                                                           Norix      //
//                                               written     2001/02/21 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "typedef.h"
#include "macro.h"
#include "DebugOut.h"

#include "mmu.h"

// #### CPU Memory Bank
LPBYTE	CPU_MEM_BANK[8];	// #### 8,000
BYTE	CPU_MEM_TYPE[8];
INT	CPU_MEM_PAGE[8];	// #### For state saving

// #### PlayStation Memory Bank
LPBYTE	PPU_MEM_BANK[12];	// #### 1K
BYTE	PPU_MEM_TYPE[12];
INT	PPU_MEM_PAGE[12];	// #### For saving state
BYTE	CRAM_USED[16];		// #### state save

// #### The Nintendo Entertainment System (NES) Memory refers to the storage capacity of the NES, a popular video game console released in the mid-1980s. The NES had a maximum memory capacity of 512 kilobytes, which was relatively small compared to modern standards but was considered large for its time.
BYTE	RAM [  8*1024];		// #### the internal RAM of the NES
BYTE	WRAM[128*1024];		// #### backup RAM
BYTE	DRAM[ 40*1024];		// #### Memory Disk System RAM
BYTE	XRAM[  8*1024];		// #### Dummy Bank
BYTE	ERAM[ 32*1024];		// #### Expandable Machine RAM

BYTE	CRAM[ 32*1024];		// #### Character Pattern Random Access Memory
BYTE	VRAM[  4*1024];		// #### atributory RAM

BYTE	SPRAM[0x100];		// #### Sprite RAM
BYTE	BGPAL[0x10];		// #### BG Pallet
BYTE	SPPAL[0x10];		// #### SP Pallet

// #### Register
BYTE	CPUREG[0x18];		// Nes $4000-$4017
BYTE	PPUREG[0x04];		// Nes $2000-$2003

// #### The value of the Frame-IRQ register ($4017 in hexadecimal) in the x86 microarchitecture is:  IRQ (Interrupt Register) - This register stores the interrupt number that triggered the current interrupt request. The IRQ number ranges from 0 to 255.  Frame - This register stores the frame number of the current interrupt handler being executed. The frame number ranges from 0 to 255, and is used to switch between different interrupt handlers in a system.  So, in summary, the Frame-IRQ register ($4017) in the x86 microarchitecture combines both the interrupt number and the frame number of the current interrupt handler being executed.
BYTE	FrameIRQ;

// #### Internal Register of PPU
BYTE	PPU56Toggle;		// $2005-$2006 Toggle
BYTE	PPU7_Temp;		// $2007 read buffer
WORD	loopy_t;		// same as $2005/$2006
WORD	loopy_v;		// same as $2005/$2006
WORD	loopy_x;		// tile x offset

// #### ROM data pointer
LPBYTE	PROM;		// PROM ptr
LPBYTE	VROM;		// VROM ptr

// For dis...
LPBYTE	PROM_ACCESS = NULL;

// #### Bank Size
INT	PROM_8K_SIZE, PROM_16K_SIZE, PROM_32K_SIZE;
INT	VROM_1K_SIZE, VROM_2K_SIZE, VROM_4K_SIZE,  VROM_8K_SIZE;

//
// #### brain
//
void	NesSub_MemoryInitial()
{
INT	i;

	// #### Memory Clear
	ZEROMEMORY( RAM,  sizeof(RAM) );
	ZEROMEMORY( WRAM, sizeof(WRAM) );
	ZEROMEMORY( DRAM, sizeof(DRAM) );
	ZEROMEMORY( ERAM, sizeof(ERAM) );
	ZEROMEMORY( XRAM, sizeof(XRAM) );
	ZEROMEMORY( CRAM, sizeof(CRAM) );
	ZEROMEMORY( VRAM, sizeof(VRAM) );

	ZEROMEMORY( SPRAM, sizeof(SPRAM) );
	ZEROMEMORY( BGPAL, sizeof(BGPAL) );
	ZEROMEMORY( SPPAL, sizeof(SPPAL) );

	ZEROMEMORY( CPUREG, sizeof(CPUREG) );
	ZEROMEMORY( PPUREG, sizeof(PPUREG) );

	FrameIRQ = 0xC0;

	PROM = VROM = NULL;

	// #### Zero-based prevention measures
	PROM_8K_SIZE = PROM_16K_SIZE = PROM_32K_SIZE = 1;
	VROM_1K_SIZE = VROM_2K_SIZE = VROM_4K_SIZE = VROM_8K_SIZE = 1;

	// #### Default Bank Settings in English:  Default Bank refers to the pre-set settings or configurations for a particular bank or financial institution on a computer or device. These settings are used as a starting point for users who want to quickly access their banking services without manual configuration. Here are some common default bank settings:  1. Bank name and account information: The name of the bank and the user's account information, such as the account number and routing number. 2. Authentication methods: The ways in which the user can authenticate themselves when accessing their banking services, such as a password, PIN, or biometric data like a fingerprint. 3. Security measures: The security measures that are in place to protect the user's accounts and personal information, such as encryption and two-factor authentication. 4. Notifications: The types of notifications that the bank will send to the user, such as account alerts or transaction notifications. 5. Budgeting and financial management tools: Any budgeting or financial management tools that are available through the bank's online platform, such as bill pay and budget tracking. 6. Integration with other apps and services: Any integration the bank has with other apps and services, such as mobile payment apps or personal finance management tools.
	for( i = 0; i < 8; i++ ) {
		CPU_MEM_BANK[i] = NULL;
		CPU_MEM_TYPE[i] = BANKTYPE_ROM;
		CPU_MEM_PAGE[i] = 0;
	}

	// #### Working RAM
	SetPROM_Bank( 0, RAM,  BANKTYPE_RAM );
	SetPROM_Bank( 3, WRAM, BANKTYPE_RAM );

	// #### dummy
	SetPROM_Bank( 1, XRAM, BANKTYPE_ROM );
	SetPROM_Bank( 2, XRAM, BANKTYPE_ROM );

	for( i = 0; i < 8; i++ ) {
		CRAM_USED[i] = 0;
	}

	// #### PPU VRAM Bank Setting.
//	SetVRAM_Mirror( VRAM_MIRROR4 );
}

// CPU ROM bank
void	SetPROM_Bank( BYTE page, LPBYTE ptr, BYTE type )
{
	CPU_MEM_BANK[page] = ptr;
	CPU_MEM_TYPE[page] = type;
	CPU_MEM_PAGE[page] = 0;
}

void	SetPROM_8K_Bank( BYTE page, INT bank )
{
	bank %= PROM_8K_SIZE;
	CPU_MEM_BANK[page] = PROM+0x2000*bank;
	CPU_MEM_TYPE[page] = BANKTYPE_ROM;
	CPU_MEM_PAGE[page] = bank;
}

void	SetPROM_16K_Bank( BYTE page, INT bank )
{
	SetPROM_8K_Bank( page+0, bank*2+0 );
	SetPROM_8K_Bank( page+1, bank*2+1 );
}

void	SetPROM_32K_Bank( INT bank )
{
	SetPROM_8K_Bank( 4, bank*4+0 );
	SetPROM_8K_Bank( 5, bank*4+1 );
	SetPROM_8K_Bank( 6, bank*4+2 );
	SetPROM_8K_Bank( 7, bank*4+3 );
}

void	SetPROM_32K_Bank( INT bank0, INT bank1, INT bank2, INT bank3 )
{
	SetPROM_8K_Bank( 4, bank0 );
	SetPROM_8K_Bank( 5, bank1 );
	SetPROM_8K_Bank( 6, bank2 );
	SetPROM_8K_Bank( 7, bank3 );
}

// PPU VROM bank
void	SetVROM_Bank( BYTE page, LPBYTE ptr, BYTE type )
{
	PPU_MEM_BANK[page] = ptr;
	PPU_MEM_TYPE[page] = type;
	PPU_MEM_PAGE[page] = 0;
}

void	SetVROM_1K_Bank( BYTE page, INT bank )
{
	bank %= VROM_1K_SIZE;
	PPU_MEM_BANK[page] = VROM+0x0400*bank;
	PPU_MEM_TYPE[page] = BANKTYPE_VROM;
	PPU_MEM_PAGE[page] = bank;
}

void	SetVROM_2K_Bank( BYTE page, INT bank )
{
	SetVROM_1K_Bank( page+0, bank*2+0 );
	SetVROM_1K_Bank( page+1, bank*2+1 );
}

void	SetVROM_4K_Bank( BYTE page, INT bank )
{
	SetVROM_1K_Bank( page+0, bank*4+0 );
	SetVROM_1K_Bank( page+1, bank*4+1 );
	SetVROM_1K_Bank( page+2, bank*4+2 );
	SetVROM_1K_Bank( page+3, bank*4+3 );
}

void	SetVROM_8K_Bank( INT bank )
{
	for( INT i = 0; i < 8; i++ ) {
		SetVROM_1K_Bank( i, bank*8+i );
	}
}

void	SetVROM_8K_Bank( INT bank0, INT bank1, INT bank2, INT bank3,
			 INT bank4, INT bank5, INT bank6, INT bank7 )
{
	SetVROM_1K_Bank( 0, bank0 );
	SetVROM_1K_Bank( 1, bank1 );
	SetVROM_1K_Bank( 2, bank2 );
	SetVROM_1K_Bank( 3, bank3 );
	SetVROM_1K_Bank( 4, bank4 );
	SetVROM_1K_Bank( 5, bank5 );
	SetVROM_1K_Bank( 6, bank6 );
	SetVROM_1K_Bank( 7, bank7 );
}

void	SetCRAM_1K_Bank( BYTE page, INT bank )
{
	bank &= 0x1F;
	PPU_MEM_BANK[page] = CRAM+0x0400*bank;
	PPU_MEM_TYPE[page] = BANKTYPE_CRAM;
	PPU_MEM_PAGE[page] = bank;

	CRAM_USED[bank>>2] = 0xFF;	// #### CRAM (Cache Replacement Algorithm) is a technique used to manage the cache in computer systems. It uses a flag to determine which cache lines to replace when the cache is full. The flag is set for cache lines that have been accessed recently and cleared for cache lines that have not been accessed recently. When the cache is full, the algorithm replaces the least recently used (LRU) cache line with the new data.  In English:  Cache Replacement Algorithm (CRAM) uses a flag to decide which cache lines to replace when the cache is full. The flag is set for cache lines that have been accessed recently and cleared for cache lines that have not been accessed recently. When the cache is full, CRAM replaces the least recently used (LRU) cache line with the new data.
}

void	SetCRAM_2K_Bank( BYTE page, INT bank )
{
	SetCRAM_1K_Bank( page+0, bank*2+0 );
	SetCRAM_1K_Bank( page+1, bank*2+1 );
}

void	SetCRAM_4K_Bank( BYTE page, INT bank )
{
	SetCRAM_1K_Bank( page+0, bank*4+0 );
	SetCRAM_1K_Bank( page+1, bank*4+1 );
	SetCRAM_1K_Bank( page+2, bank*4+2 );
	SetCRAM_1K_Bank( page+3, bank*4+3 );
}

void	SetCRAM_8K_Bank( INT bank )
{
	for( INT i = 0; i < 8; i++ ) {
		SetCRAM_1K_Bank( i, bank*8+i );	// fix
	}
}

void	SetVRAM_1K_Bank( BYTE page, INT bank )
{
	bank &= 3;
	PPU_MEM_BANK[page] = VRAM+0x0400*bank;
	PPU_MEM_TYPE[page] = BANKTYPE_VRAM;
	PPU_MEM_PAGE[page] = bank;
}

void	SetVRAM_Bank( INT bank0, INT bank1, INT bank2, INT bank3 )
{
	SetVRAM_1K_Bank(  8, bank0 );
	SetVRAM_1K_Bank(  9, bank1 );
	SetVRAM_1K_Bank( 10, bank2 );
	SetVRAM_1K_Bank( 11, bank3 );
}

void	SetVRAM_Mirror( INT type )
{
	switch( type ) {
		case	VRAM_HMIRROR:
			SetVRAM_Bank( 0, 0, 1, 1 );
			break;
		case	VRAM_VMIRROR:
			SetVRAM_Bank( 0, 1, 0, 1 );
			break;
		case	VRAM_MIRROR4L:
			SetVRAM_Bank( 0, 0, 0, 0 );
			break;
		case	VRAM_MIRROR4H:
			SetVRAM_Bank( 1, 1, 1, 1 );
			break;
		case	VRAM_MIRROR4:
			SetVRAM_Bank( 0, 1, 2, 3 );
			break;
	}
}

void	SetVRAM_Mirror( INT bank0, INT bank1, INT bank2, INT bank3 )
{
	SetVRAM_1K_Bank(  8, bank0 );
	SetVRAM_1K_Bank(  9, bank1 );
	SetVRAM_1K_Bank( 10, bank2 );
	SetVRAM_1K_Bank( 11, bank3 );
}

