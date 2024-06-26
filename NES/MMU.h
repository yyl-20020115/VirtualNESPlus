//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES Memory Management Unit                                      //
//                                                           Norix      //
//                                               written     2001/02/21 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////

#ifndef	__MMU_INCLUDED__
#define	__MMU_INCLUDED__

#include "typedef.h"
#include "macro.h"

// #### CPU Memory Bank
extern	LPBYTE	CPU_MEM_BANK[8];	// #### eight thousand
extern	BYTE	CPU_MEM_TYPE[8];
extern	INT	CPU_MEM_PAGE[8];	// #### Used to save states

// #### PS Vita Memory Card
extern	LPBYTE	PPU_MEM_BANK[12];	// #### one thousand
extern	BYTE	PPU_MEM_TYPE[12];
extern	INT	PPU_MEM_PAGE[12];	// #### Here are a few options for saving data in a state-save system using JavaScript:  1. LocalStorage: This is a simple way to store small amounts of data locally on the client-side. It's persistent across page reloads, but can be cleared by the user or through the browser's privacy settings. ``` // Set a value in LocalStorage localStorage.setItem('myValue', 'someValue');  // Get a value from LocalStorage const myValue = localStorage.getItem('myValue'); ``` 2. SessionStorage: Similar to LocalStorage, but the data is only available until the user closes their browser window or navigates away from the page. ``` // Set a value in SessionStorage sessionStorage.setItem('myValue', 'someValue');  // Get a value from SessionStorage const myValue = sessionStorage.getItem('myValue'); ``` 3. Cookies: You can use cookies to store data on the client-side, but they have some limitations. For example, they can only store strings, and they may be deleted or modified by the user. ``` // Set a value in a cookie document.cookie = 'myValue=someValue';  // Get a value from a cookie const myValue = document.cookie.match(/myValue=([^;]*)/)[1]; ``` 4. IndexedDB: This is a more powerful storage system than the others mentioned here, but it can be more complex to use. It allows you to store large amounts of structured data, and it's persistent across page reloads. ``` // Create an IndexedDB database const db = window.indexeddb || window.webkitIndexedDB; const request = db.open('myDatabase', 1);  // Add a value to the database request.onsuccess = function() {   const transaction = request.result;   transaction.objectStore('myObjectStore').put({ myValue: 'someValue' }); }; ``` 5. Web Storage: This is a unified storage API that provides a simple way to store and retrieve data in the browser. It's similar to LocalStorage, but it's more flexible and can be used for storing different types of data. ``` // Set a value in Web Storage window.storage.setItem('myValue', 'someValue');  // Get a value from Web Storage const myValue = window.storage.getItem('myValue'); ``` These are just a few examples of the many ways you can store data in a state-save system using JavaScript. The best option for your project will depend on your specific requirements and constraints.
extern	BYTE	CRAM_USED[16];		// #### Used to save states

// #### NES Flashbacks.
extern	BYTE	RAM [  8*1024];		// #### Internal Memory
extern	BYTE	WRAM[128*1024];		// #### work.
extern	BYTE	DRAM[ 40*1024];		// #### Extend RAM
extern	BYTE	XRAM[  8*1024];		// #### Fake Bank
extern	BYTE	ERAM[ 32*1024];		// #### Extend RAM

extern	BYTE	CRAM[ 32*1024];		// #### Character Pattern RAM
extern	BYTE	VRAM[  4*1024];		// #### Attributed Random Access Memory

extern	BYTE	SPRAM[0x100];		// #### Sprite RAM
extern	BYTE	BGPAL[0x10];		// #### pallet
extern	BYTE	SPPAL[0x10];		// #### SP pallet

// #### Register
extern	BYTE	CPUREG[0x18];		// Nes $4000-$4017
extern	BYTE	PPUREG[0x04];		// Nes $2000-$2003

// #### The value of the register $4017 in the 8086 microprocessor is the Frame-IRQ (Interrupt Register). It stores the address of the frame buffer that contains the Interrupt Handler routine for the current interrupt.  In English, this can be translated as:  Frame-IRQ Register ($4017)
extern	BYTE	FrameIRQ;

// #### PPU inner registers
extern	BYTE	PPU56Toggle;		// $2005-$2006 Toggle
extern	BYTE	PPU7_Temp;		// $2007 read buffer
extern	WORD	loopy_t;		// same as $2005/$2006
extern	WORD	loopy_v;		// same as $2005/$2006
extern	WORD	loopy_x;		// tile x offset

// #### ROM data pointer
extern	LPBYTE	PROM;		// PROM ptr
extern	LPBYTE	VROM;		// VROM ptr

#ifdef	_DATATRACE
// For dis...
extern	LPBYTE	PROM_ACCESS;
#endif

// #### Bank Size in Romance Languages
extern	INT	PROM_8K_SIZE, PROM_16K_SIZE, PROM_32K_SIZE;
extern	INT	VROM_1K_SIZE, VROM_2K_SIZE, VROM_4K_SIZE,  VROM_8K_SIZE;


// #### function
extern	void	NesSub_MemoryInitial();

extern	void	SetPROM_Bank( BYTE page, LPBYTE ptr, BYTE type );
extern	void	SetPROM_8K_Bank ( BYTE page, INT bank );
extern	void	SetPROM_16K_Bank( BYTE page, INT bank );
extern	void	SetPROM_32K_Bank( INT bank );
extern	void	SetPROM_32K_Bank( INT bank0, INT bank1, INT bank2, INT bank3 );

extern	void	SetVROM_Bank( BYTE page, LPBYTE ptr, BYTE type );
extern	void	SetVROM_1K_Bank( BYTE page, INT bank );
extern	void	SetVROM_2K_Bank( BYTE page, INT bank );
extern	void	SetVROM_4K_Bank( BYTE page, INT bank );
extern	void	SetVROM_8K_Bank( INT bank );
extern	void	SetVROM_8K_Bank( INT bank0, INT bank1, INT bank2, INT bank3,
				 INT bank4, INT bank5, INT bank6, INT bank7 );

extern	void	SetCRAM_1K_Bank( BYTE page, INT bank );
extern	void	SetCRAM_2K_Bank( BYTE page, INT bank );
extern	void	SetCRAM_4K_Bank( BYTE page, INT bank );
extern	void	SetCRAM_8K_Bank( INT bank );

extern	void	SetVRAM_1K_Bank( BYTE page, INT bank );
extern	void	SetVRAM_Bank( INT bank0, INT bank1, INT bank2, INT bank3 );
extern	void	SetVRAM_Mirror( INT type );
extern	void	SetVRAM_Mirror( INT bank0, INT bank1, INT bank2, INT bank3 );


// #### Memory Type
// For PROM (CPU)
#define	BANKTYPE_ROM	0x00
#define	BANKTYPE_RAM	0xFF
#define	BANKTYPE_DRAM	0x01
#define	BANKTYPE_MAPPER	0x80
// For VROM/VRAM/CRAM (PPU)
#define	BANKTYPE_VROM	0x00
#define	BANKTYPE_CRAM	0x01
#define	BANKTYPE_VRAM	0x80

// #### reflection-type
#define	VRAM_HMIRROR	0x00	// Horizontal
#define	VRAM_VMIRROR	0x01	// Virtical
#define	VRAM_MIRROR4	0x02	// All screen
#define	VRAM_MIRROR4L	0x03	// #### PA10
#define	VRAM_MIRROR4H	0x04	// #### $2400-$27FF

#endif	// !__MMU_INCLUDED__

