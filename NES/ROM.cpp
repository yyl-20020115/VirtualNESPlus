//////////////////////////////////////////////////////////////////////////
//                                                                      //
//      NES ROM Cartridge class                                         //
//                                                           Norix      //
//                                               written     2001/02/20 //
//                                               last modify ----/--/-- //
//////////////////////////////////////////////////////////////////////////
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <mbstring.h>

#include "typedef.h"
#include "macro.h"

#include "VirtualNESres.h"

#include "DebugOut.h"
#include "App.h"
#include "Plugin.h"
#include "Pathlib.h"
#include "Crclib.h"
#include "Config.h"

#include "Archive.h"

#include "rom.h"
#include "romdb.h"
#include "mmu.h"

#include "ips.h"

//
// #### Contractor
//
ROM::ROM( const char* fname )
{
FILE	*fp = NULL;
LPBYTE	temp = NULL;
LPBYTE	bios = NULL;
LONG	FileSize;

	ZEROMEMORY( &header, sizeof(header) );
	ZEROMEMORY( path, sizeof(path) );
	ZEROMEMORY( name, sizeof(name) );

	bPAL = FALSE;
	bNSF = FALSE;
	NSF_PAGE_SIZE = 0;

	lpPRG = lpCHR = lpTrainer = lpDiskBios = lpDisk = NULL;

	crc = crcall = 0;
	mapper = 0;
	diskno = 0;

	try {
		if( !(fp = ::fopen( fname, "rb" )) ) {
			// #### I can't open the xxx file.
			LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
			::wsprintf( szErrorString, szErrStr, fname );
			throw	szErrorString;
		}

		// #### File size acquisition.
		::fseek( fp, 0, SEEK_END );
		FileSize = ::ftell( fp );
		::fseek( fp, 0, SEEK_SET );
		// #### +
		if( FileSize < 17 ) {
			// #### File size is too small.
			throw	CApp::GetErrorString( IDS_ERROR_SMALLFILE );
		}

		// #### Temporary memory guarantee
		if( !(temp = (LPBYTE)::malloc( FileSize )) ) {
			// #### I cannot secure memory.
			throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
		}

		// #### big-picture reading.
		if( ::fread( temp, FileSize, 1, fp ) != 1 ) {
			// #### File read failed.
			throw	CApp::GetErrorString( IDS_ERROR_READ );
		}

		FCLOSE( fp );

		// #### headline copy
		::memcpy( &header, temp, sizeof(NESHEADER) );

		if( header.ID[0] == 'N' && header.ID[1] == 'E'
		 && header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
			// #### Header Copy
			memcpy( &header, temp, sizeof(NESHEADER) );
		} else if( header.ID[0] == 'F' && header.ID[1] == 'D'
			&& header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
			// #### Title copy
			memcpy( &header, temp, sizeof(NESHEADER) );
		} else if( header.ID[0] == 'N' && header.ID[1] == 'E'
			&& header.ID[2] == 'S' && header.ID[3] == 'M') {
			// #### header copy
			memcpy( &header, temp, sizeof(NESHEADER) );
		} else {
			FREE( temp );

			if( !UnCompress( fname, &temp, (LPDWORD)&FileSize ) ) {
				// #### unresponsive form
				throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
			}
			// #### title copy
			::memcpy( &header, temp, sizeof(NESHEADER) );
		}

		// Since the zip/fds/nes is defrosted and raw, now apply the patch
		if( Config.emulator.bAutoIPS ) {
			LPBYTE	ipstemp = NULL;
			if( !(ipstemp = (LPBYTE)::malloc( FileSize )) ) {
				// #### I can't secure memory.
				throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
			}
			::memcpy( ipstemp, temp, FileSize );
			if( ApplyIPS( fname, ipstemp, FileSize ) ) {
				::memcpy( &header, ipstemp, sizeof(NESHEADER) );
				::memcpy( temp, ipstemp, FileSize );
			}

			FREE( ipstemp );
		}

		DWORD	PRGoffset, CHRoffset;
		LONG	PRGsize, CHRsize;

		if( header.ID[0] == 'N' && header.ID[1] == 'E'
		 && header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
		// #### Normal NES file
			PRGsize = (LONG)header.PRG_PAGE_SIZE*0x4000;
			CHRsize = (LONG)header.CHR_PAGE_SIZE*0x2000;
			PRGoffset = sizeof(NESHEADER);
			CHRoffset = PRGoffset + PRGsize;

			if( IsTRAINER() ) {
				PRGoffset += 512;
				CHRoffset += 512;
			}

			if( PRGsize <= 0 || (PRGsize+CHRsize) > FileSize ) {
				// #### NES header is abnormal.
				throw	CApp::GetErrorString( IDS_ERROR_INVALIDNESHEADER );
			}

			// PRG BANK
			if( !(lpPRG = (LPBYTE)malloc( PRGsize )) ) {
				// #### I cannot secure memory.
				throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
			}

			::memcpy( lpPRG, temp+PRGoffset, PRGsize );

			// CHR BANK
			if( CHRsize > 0 ) {
				if( !(lpCHR = (LPBYTE)malloc( CHRsize )) ) {
					// #### secure
					throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
				}

				if( FileSize >= (LONG)(CHRoffset+CHRsize) ) {
					memcpy( lpCHR, temp+CHRoffset, CHRsize );
				} else {
					// #### CHR
					CHRsize -= (CHRoffset+CHRsize - FileSize);
					memcpy( lpCHR, temp+CHRoffset, CHRsize );
				}
			} else {
				lpCHR = NULL;
			}

			// Trainer
			if( IsTRAINER() ) {
				if( !(lpTrainer = (LPBYTE)malloc( 512 )) ) {
					// #### I cannot secure memory.
					throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
				}

				memcpy( lpTrainer, temp+sizeof(NESHEADER), 512 );
			} else {
				lpTrainer = NULL;
			}
		} else if( header.ID[0] == 'F' && header.ID[1] == 'D'
			&& header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
		// FDS(Nintendo Disk System)
			// #### disk size
			diskno = header.PRG_PAGE_SIZE;

			if( FileSize < (16+65500*diskno) ) {
				// #### Disc size is abnormal.
				throw	CApp::GetErrorString( IDS_ERROR_ILLEGALDISKSIZE );
			}
			if( diskno > 8 ) {
				// #### The disc with more than 8 faces cannot be accommodated.
				throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTDISK );
			}

			ZEROMEMORY( &header, sizeof(NESHEADER) );

			// #### dummy head
			header.ID[0] = 'N';
			header.ID[1] = 'E';
			header.ID[2] = 'S';
			header.ID[3] = 0x1A;
			header.PRG_PAGE_SIZE = (BYTE)diskno*4;
			header.CHR_PAGE_SIZE = 0;
			header.control1 = 0x40;
			header.control2 = 0x10;

			PRGsize = sizeof(NESHEADER)+65500*(LONG)diskno;
			// PRG BANK
			if( !(lpPRG = (LPBYTE)malloc( PRGsize )) ) {
				// #### I can't allocate memory.
				throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
			}
			// #### Backup data
			if( !(lpDisk = (LPBYTE)malloc( PRGsize )) ) {
				// #### I can't secure memory.
				throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
			}
			// CHR BANK
			lpCHR = NULL;

			::memcpy( lpPRG, &header, sizeof(NESHEADER) );
			::memcpy( lpPRG+sizeof(NESHEADER), temp+sizeof(NESHEADER), 65500*(LONG)diskno );
			// #### Software for identifying the location of modified data
			ZEROMEMORY( lpDisk, PRGsize );
//			memcpy( lpDisk, &header, sizeof(NESHEADER) );
//			memcpy( lpDisk+sizeof(NESHEADER), temp+sizeof(NESHEADER), PRGsize-sizeof(NESHEADER) );

			lpPRG[0] = 'F';
			lpPRG[1] = 'D';
			lpPRG[2] = 'S';
			lpPRG[3] = 0x1A;
			lpPRG[4] = (BYTE)diskno;

			// #### Loading Disk System BIOS.
			string	Path = CPathlib::MakePathExt( CApp::GetModulePath(), "DISKSYS", "ROM" );

			if( !(fp = fopen( Path.c_str(), "rb" )) ) {
				// #### There is no DISKSYS.ROM file.
				throw	CApp::GetErrorString( IDS_ERROR_NODISKBIOS );
			}

			::fseek( fp, 0, SEEK_END );
			FileSize = ::ftell( fp );
			::fseek( fp, 0, SEEK_SET );
			if( FileSize < 17 ) {
				// #### File size is too small.
				throw	CApp::GetErrorString( IDS_ERROR_SMALLFILE );
			}
			if( !(bios = (LPBYTE)malloc( FileSize )) ) {
				// #### I cannot secure memory.
				throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
			}
			if( fread( bios, FileSize, 1, fp ) != 1 ) {
				// #### File read failed.
				throw	CApp::GetErrorString( IDS_ERROR_READ );
			}
			FCLOSE( fp );
			const int _8K = 8 * 1024;
			if( !(lpDiskBios = (LPBYTE)malloc(_8K)) ) {
				// #### I cannot secure memory.
				throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
			}

			if( bios[0] == 'N' && bios[1] == 'E' && bios[2] == 'S' && bios[3] == 0x1A ) {
			// #### New Economic System Biological Integration Operating System
				::memcpy( lpDiskBios, bios+0x6010, _8K);
			} else {
			// #### Biological BIOS
				::memcpy( lpDiskBios, bios, _8K);
			}
			FREE( bios );
		} else if( header.ID[0] == 'N' && header.ID[1] == 'E'
			&& header.ID[2] == 'S' && header.ID[3] == 'M') {
		// NSF
			bNSF = TRUE;
			ZEROMEMORY( &header, sizeof(NESHEADER) );

			// #### headline copy
			memcpy( &nsfheader, temp, sizeof(NSFHEADER) );

			PRGsize = FileSize-sizeof(NSFHEADER);
DEBUGOUT( "PRGSIZE:%d\n", PRGsize );
			PRGsize = (PRGsize+0x0FFF)&~0x0FFF;
DEBUGOUT( "PRGSIZE:%d\n", PRGsize );
			if( !(lpPRG = (LPBYTE)malloc( PRGsize )) ) {
				// #### I cannot secure memory.
				throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
				throw	szErrorString;
			}
			ZEROMEMORY( lpPRG, PRGsize );
			memcpy( lpPRG, temp+sizeof(NSFHEADER), FileSize-sizeof(NSFHEADER) );

			NSF_PAGE_SIZE = PRGsize>>12;
DEBUGOUT( "PAGESIZE:%d\n", NSF_PAGE_SIZE );
		} else {
			// #### Not responded format.
			throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
		}

		// #### File name retrieval
		{
		string	tempstr;
		tempstr = CPathlib::SplitPath( fname );
		::strcpy( path, tempstr.c_str() );
		tempstr = CPathlib::SplitFname( fname );
		::strcpy( name, tempstr.c_str() );
		// #### Original File Name (Full Path).
		::strcpy( fullpath, fname );
		}

		// #### MPP Setting
		if( !bNSF ) {
			mapper = (header.control1>>4)|(header.control2&0xF0);
			crc = crcall = crcvrom = 0;

			if( mapper != 20 ) {
				// #### The PRG (Pseudorandom Generator) CRC (Cyclic Redundancy Check) calculation in NesToy is the same as the one used in other emulators and game consoles. Here's an overview of how it works:  1. Initialize a 32-bit integer variable `crc` to zero. 2. For each byte of the PRG data, perform the following calculations: 	* Calculate the XOR of the current byte with the previous byte (if any). This is done by taking the bitwise XOR of the two bytes and storing the result in a new 8-bit integer variable `xor`. 	* Calculate the AND of the current byte with a fixed constant `0x12345678`. This is done by taking the bitwise AND of the current byte with the constant and storing the result in a new 8-bit integer variable `and`. 	* Calculate the XOR of the `xor` and `and` variables. This is done by taking the bitwise XOR of `xor` and `and` and storing the result in the `crc` variable. 3. Repeat step 2 for each byte of the PRG data. 4. The final value of `crc` represents the CRC checksum of the entire PRG data.  Here's an example code snippet in assembly language that shows how to calculate the PRG CRC in NesToy: ```asm section .text global _start  PRG_CRC:     ; Initialize crc to 0     mov eax, 0     mov crc, eax  loop:     ; Get the current byte of the PRG data     ldb rsi, $PRG_DATA_ADDR     mov rdx, rsi      ; Calculate the XOR of the current byte with the previous byte (if any)     xor rdx, [rdx + 1]     mov [crc], rdx      ; Calculate the AND of the current byte with a fixed constant     and rdx, $0x12345678     mov [crc], rdx      ; Increment the previous address for the next iteration     inc rsi     jmp loop  ; The final value of crc represents the CRC checksum of the entire PRG data ``` Note that this is just an example implementation, and the actual code used in NesToy may be different. Also, keep in mind that the PRG CRC calculation is a simple one-time operation that happens at the beginning of the gameplay session, and it's not related to the game logic or physics.
				if( IsTRAINER() ) {
					crcall  = CRC::CrcRev( 512+PRGsize+CHRsize, temp+sizeof(NESHEADER) );
					crc     = CRC::CrcRev( 512+PRGsize, temp+sizeof(NESHEADER) );
					if( CHRsize )
						crcvrom = CRC::CrcRev( CHRsize, temp+PRGsize+512+sizeof(NESHEADER) );
				} else {
					crcall  = CRC::CrcRev( PRGsize+CHRsize, temp+sizeof(NESHEADER) );
					crc     = CRC::CrcRev( PRGsize, temp+sizeof(NESHEADER) );
					if( CHRsize )
						crcvrom = CRC::CrcRev( CHRsize, temp+PRGsize+sizeof(NESHEADER) );
				}

				FilenameCheck( name );

				romdatabase.HeaderCorrect( header, crcall, crc );

//#include "ROM_Patch.cpp"
				{
					//
// #### Modification of ROM Header (including corrections made using NES Toy) and ROM Patch.
//

// Mapper 000
					if (crc == 0x57970078) {	// #### Coercive patch
						lpPRG[0x078C] = 0x6C;
						lpPRG[0x3FE1] = 0xFF;
						lpPRG[0x3FE6] = 0x00;
					}
					if (crc == 0xaf2bbcbc		// Mach Rider(JU)
						|| crc == 0x3acd4bf1		// #### Unnecessary Patch
						|| crc == 0x8bbe9bec) {
						lpPRG[0x090D] = 0x6E;
						lpPRG[0x7FDF] = 0xFF;
						lpPRG[0x7FE4] = 0x00;

						header.control1 = ROM_VMIRROR;
					}

					if (crc == 0xe16bb5fe) {	// Zippy Race(J)
						header.control1 &= 0xf6;
					}
					if (crc == 0x85534474) {	// Lode Runner(J)
						lpPRG[0x29E9] = 0xEA;	// #### The patch to display the save menu is: `save_menu`.
						lpPRG[0x29EA] = 0xEA;
						lpPRG[0x29F8] = 0xEA;
						lpPRG[0x29F9] = 0xEA;
					}

					// Mapper 001
					if (crc == 0x7831b2ff		// America Daitouryou Senkyo(J)
						|| crc == 0x190a3e11		// Be-Bop-Highschool - Koukousei Gokuraku Densetsu(J)
						|| crc == 0x52449508		// Home Run Nighter - Pennant League!!(J)
						|| crc == 0x0973f714		// Jangou(J)
						|| crc == 0x7172f3d4		// Kabushiki Doujou(J)
						|| crc == 0xa5781280		// Kujaku Ou 2(J)
						|| crc == 0x8ce9c87b		// Money Game, The(J)
						|| crc == 0xec47296d		// Morita Kazuo no Shougi(J)
						|| crc == 0xcee5857b		// Ninjara Hoi!(J)
						|| crc == 0xe63d9193		// Tanigawa Kouji no Shougi Shinan 3(J)
						|| crc == 0xd54f5da9		// Tsuppari Wars(J)
						|| crc == 0x1e0c7ea3) {	// AD&D Dragons of Flame(J)
						header.control1 |= ROM_SAVERAM;
					}
					if (crc == 0x1995ac4e) {	// #### grand prix.
						lpPRG[0x1F7AD] = 0xFF;
						lpPRG[0x1F7BC] = 0x00;
					}

					if (crc == 0x20d22251) {	// #### Forced smile
						lpPRG[0x1F17E] = 0xEA;
						lpPRG[0x1F17F] = 0xEA;
					}

					if (crc == 0x11469ce3) {	// #### Unnatural laughter
						lpCHR[0x0000] = 0x01;
					}

					if (crc == 0x3fccdc7b) {	// #### Baseball Star - Make a Miracle Happen!!(J)  Unlikely Patch(^^;
						lpPRG[0x0F666] = 0x9D;
					}

					if (crc == 0xdb564628) {	// Mario Open Golf(J)
						lpPRG[0x30195] = 0xC0;
					}

					// Mapper 002
					if (crc == 0x63af202f) {	// JJ - Tobidase Daisakusen Part 2(J)
						header.control1 &= 0xf6;
						header.control1 |= ROM_VMIRROR;
					}

					if (crc == 0x99a62e47) {	// Black Bass 2, The(J)
						header.control1 &= 0xf6;
						header.control1 |= ROM_VMIRROR;
					}

					if (crc == 0x0eaa7515		// Rod Land(J)
						|| crc == 0x22ab9694) {	// Rod Land(E)
						header.control1 &= 0xf6;
						header.control1 |= ROM_VMIRROR;
					}

					if (crc == 0x2061772a) {	// Tantei Jinguji Taburou Tokino Sugiyukumamani (J)
						header.control1 &= 0xf6;
						header.control1 |= ROM_VMIRROR;
					}

					// Mapper 003
					if (crc == 0x29401686) {	// Minna no Taabou no Nakayoshi Dai Sakusen(J)
						//	lpPRG[0x2B3E] = 0x60;
					}
					if (crc == 0x932a077a) {	// TwinBee(J)
						mapper = 87;
					}
					if (crc == 0x8218c637) {	// Space Hunter(J)
						//	header.control1 &= 0xf6;
						//	header.control1 |= ROM_4SCREEN;
						header.control1 = ROM_VMIRROR;
					}
					if (crc == 0x2bb6a0f8		// Sherlock Holmes - Hakushaku Reijou Yuukai Jiken(J)
						|| crc == 0x28c11d24		// Sukeban Deka 3(J)
						|| crc == 0x02863604) {	// Sukeban Deka 3(J)(Alt)
						header.control1 &= 0xf6;
						header.control1 |= ROM_VMIRROR;
					}

					// Mapper 004
					if (crc == 0x58581770) {	// Rasaaru Ishii no Childs Quest(J)
						header.control1 &= 0xf6;
						header.control1 |= ROM_VMIRROR;
					}
					if (crc == 0xf3feb3ab		// Kunio Kun no Jidaigeki Dayo Zenin Shuugou! (J)
						|| crc == 0xa524ae9b		// Otaku no Seiza - An Adventure in the Otaku Galaxy (J)
						|| crc == 0x46dc6e57		// SD Gundam - Gachapon Senshi 2 - Capsule Senki (J)
						|| crc == 0x66b2dec7		// SD Gundam - Gachapon Senshi 3 - Eiyuu Senki (J)
						|| crc == 0x92b07fd9		// SD Gundam - Gachapon Senshi 4 - New Type Story (J)
						|| crc == 0x8ee6463a		// SD Gundam - Gachapon Senshi 5 - Battle of Universal Century (J)
						|| crc == 0xaf754426		// Ultraman Club 3 (J)
						|| crc == 0xfe4e5b11		// Ushio to Tora - Shinen no Daiyou (J)
						|| crc == 0x57c12c17) {	// Yamamura Misa Suspense - Kyouto Zaiteku Satsujin Jiken (J)
						header.control1 |= ROM_SAVERAM;
					}
					if (crc == 0x42e03e4a) {	// RPG Jinsei Game (J)
						mapper = 118;
						header.control1 |= ROM_SAVERAM;
					}
					if (crc == 0xfd0299c3) {	// METAL MAX(J)
						lpPRG[0x3D522] = 0xA9;
						lpPRG[0x3D523] = 0x19;
					}
					if (crc == 0x1d2e5018		// Rockman 3(J)
						|| crc == 0x6b999aaf) {	// Mega Man 3(U)
						//	lpPRG[0x3C179] = 0xBA;//
						//	lpPRG[0x3C9CC] = 0x9E;
					}

					// Mapper 005
					if (crc == 0xe91548d8) {	// Shin 4 Nin Uchi Mahjong - Yakuman Tengoku (J)
						header.control1 |= ROM_SAVERAM;
					}

					if (crc == 0x255b129c) {	// Gun Sight (J) / Gun Sight (J)[a1]
						lpPRG[0x02D0B] = 0x01;
						lpPRG[0x0BEC0] = 0x01;
					}


					// Mapper 010
					if (crc == 0xc9cce8f2) {	// Famicom Wars (J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 016
					if (crc == 0x983d8175		// Datach - Battle Rush - Build Up Robot Tournament (J)
						|| crc == 0x894efdbc		// Datach - Crayon Shin Chan - Ora to Poi Poi (J)
						|| crc == 0x19e81461		// Datach - Dragon Ball Z - Gekitou Tenkaichi Budou Kai (J)
						|| crc == 0xbe06853f		// Datach - J League Super Top Players (J)
						|| crc == 0x0be0a328		// Datach - SD Gundam - Gundam Wars (J)
						|| crc == 0x5b457641		// Datach - Ultraman Club - Supokon Fight! (J)
						|| crc == 0xf51a7f46		// Datach - Yuu Yuu Hakusho - Bakutou Ankoku Bujutsu Kai (J)
						|| crc == 0x31cd9903		// Dragon Ball Z - Kyoushuu! Saiya Jin (J)
						|| crc == 0xe49fc53e		// Dragon Ball Z 2 - Gekishin Freeza!! (J)
						|| crc == 0x09499f4d		// Dragon Ball Z 3 - Ressen Jinzou Ningen (J)
						|| crc == 0x2e991109		// Dragon Ball Z Gaiden - Saiya Jin Zetsumetsu Keikaku (J)
						|| crc == 0x170250de) {	// Rokudenashi Blues(J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 019
					if (crc == 0x3296ff7a		// Battle Fleet (J)
						|| crc == 0x429fd177		// Famista '90 (J)
						|| crc == 0xdd454208		// Hydlide 3 - Yami Kara no Houmonsha (J)
						|| crc == 0xb1b9e187		// Kaijuu Monogatari (J)
						|| crc == 0xaf15338f) {	// Mindseeker (J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 026
					if (crc == 0x836cc1ab) {	// Mouryou Senki Madara (J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 033
					if (crc == 0x547e6cc1) {	// Flintstones - The Rescue of Dino & Hoppy, The(J)
						mapper = 48;
					}

					// Mapper 065
					if (crc == 0xfd3fc292) {	// Ai Sensei no Oshiete - Watashi no Hoshi (J)
						mapper = 32;
					}

					// Mapper 068
					if (crc == 0xfde79681) {	// Maharaja (J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 069
					if (crc == 0xfeac6916		// Honoo no Toukyuuji - Dodge Danpei 2(J)
						|| crc == 0x67898319) {	// Barcode World(J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 080
					if (crc == 0x95aaed34		// Mirai Shinwa Jarvas (J)
						|| crc == 0x17627d4b) {	// Taito Grand Prix - Eikou heno License (J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 082
					if (crc == 0x4819a595) {	// Kyuukyoku Harikiri Stadium - Heisei Gannen Ban (J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 086
					if (crc == 0xe63f7d0b) {	// Urusei Yatsura - Lum no Wedding Bell(J)
						mapper = 101;
					}

					// Mapper 118
					if (crc == 0x3b0fb600) {	// Ys 3 - Wonderers From Ys (J)
						header.control1 |= ROM_SAVERAM;
					}

					// Mapper 180
					if (crc == 0xc68363f6) {	// Crazy Climber(J)
						header.control1 &= 0xf6;
					}

					// VS-Unisystem
					if (crc == 0x70901b25) {	// VS Slalom
						mapper = 99;
					}

					if (crc == 0xd5d7eac4) {	// VS Dr. Mario
						mapper = 1;
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (crc == 0xffbef374		// VS Castlevania
						|| crc == 0x8c0c2df5) {	// VS Top Gun
						mapper = 2;
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (crc == 0xeb2dba63		// VS TKO Boxing
						|| crc == 0x98cfe016		// VS TKO Boxing (Alt)
						|| crc == 0x9818f656) {	// VS TKO Boxing (f1)
						mapper = 4;
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (crc == 0x135adf7c) {	// VS Atari RBI Baseball
						mapper = 4;
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (crc == 0xf9d3b0a3		// VS Super Xevious
						|| crc == 0x9924980a		// VS Super Xevious (b1)
						|| crc == 0x66bb838f) {	// VS Super Xevious (b2)
						mapper = 4;
						header.control1 &= 0xF6;
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (crc == 0x17ae56be) {	// VS Freedom Force
						mapper = 4;
						header.control1 &= 0xF6;
						header.control1 |= ROM_4SCREEN;
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (crc == 0xe2c0a2be) {	// VS Platoon
						mapper = 68;
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (crc == 0xcbe85490		// VS Excitebike
						|| crc == 0x29155e0c		// VS Excitebike (Alt)
						|| crc == 0xff5135a3) {	// VS Hogan's Alley
						header.control1 &= 0xF6;
						header.control1 |= ROM_4SCREEN;
					}

					if (crc == 0x0b65a917) {	// VS Mach Rider(Endurance Course)
						lpPRG[0x7FDF] = 0xFF;
						lpPRG[0x7FE4] = 0x00;
					}

					if (crc == 0x8a6a9848		// VS Mach Rider(Endurance Course)(Alt)
						|| crc == 0xae8063ef) {	// VS Mach Rider(Japan, Fighting Course)
						lpPRG[0x7FDD] = 0xFF;
						lpPRG[0x7FE2] = 0x00;
					}

					if (crc == 0x16d3f469) {	// VS Ninja Jajamaru Kun (J)
						header.control1 &= 0xf6;
						header.control1 |= ROM_VMIRROR;
					}

					if (crc == 0xc99ec059) {	// VS Raid on Bungeling Bay(J)
						mapper = 99;
						header.control1 &= 0xF6;
						header.control1 |= ROM_4SCREEN;
					}
					if (crc == 0xca85e56d) {	// VS Mighty Bomb Jack(J)
						mapper = 99;
						header.control1 &= 0xF6;
						header.control1 |= ROM_4SCREEN;
					}


					if (crc == 0xeb2dba63		// VS TKO Boxing
						|| crc == 0x9818f656		// VS TKO Boxing
						|| crc == 0xed588f00		// VS Duck Hunt
						|| crc == 0x8c0c2df5		// VS Top Gun
						|| crc == 0x16d3f469		// VS Ninja Jajamaru Kun
						|| crc == 0x8850924b		// VS Tetris
						|| crc == 0xcf36261e		// VS Sky Kid
						|| crc == 0xe1aa8214		// VS Star Luster
						|| crc == 0xec461db9		// VS Pinball
						|| crc == 0xe528f651		// VS Pinball (alt)
						|| crc == 0x17ae56be		// VS Freedom Force
						|| crc == 0xe2c0a2be		// VS Platoon
						|| crc == 0xff5135a3		// VS Hogan's Alley
						|| crc == 0x70901b25		// VS Slalom
						|| crc == 0x0b65a917		// VS Mach Rider(Endurance Course)
						|| crc == 0x8a6a9848		// VS Mach Rider(Endurance Course)(Alt)
						|| crc == 0xae8063ef		// VS Mach Rider(Japan, Fighting Course)
						|| crc == 0xcc2c4b5d		// VS Golf
						|| crc == 0xa93a5aee		// VS Stroke and Match Golf
						|| crc == 0x86167220		// VS Lady Golf
						|| crc == 0xffbef374		// VS Castlevania
						|| crc == 0x135adf7c		// VS Atari RBI Baseball
						|| crc == 0xd5d7eac4		// VS Dr. Mario
						|| crc == 0x46914e3e		// VS Soccer
						|| crc == 0x70433f2c		// VS Battle City
						|| crc == 0x8d15a6e6		// VS bad .nes
						|| crc == 0x1e438d52		// VS Goonies
						|| crc == 0xcbe85490		// VS Excitebike
						|| crc == 0x29155e0c		// VS Excitebike (alt)
						|| crc == 0x07138c06		// VS Clu Clu Land
						|| crc == 0x43a357ef		// VS Ice Climber
						|| crc == 0x737dd1bf		// VS Super Mario Bros
						|| crc == 0x4bf3972d		// VS Super Mario Bros
						|| crc == 0x8b60cc58		// VS Super Mario Bros
						|| crc == 0x8192c804		// VS Super Mario Bros
						|| crc == 0xd99a2087		// VS Gradius
						|| crc == 0xf9d3b0a3		// VS Super Xevious
						|| crc == 0x9924980a		// VS Super Xevious
						|| crc == 0x66bb838f		// VS Super Xevious
						|| crc == 0xc99ec059		// VS Raid on Bungeling Bay(J)
						|| crc == 0xca85e56d) {	// VS Mighty Bomb Jack(J)
						header.control2 |= ROM_VSUNISYSTEM;
					}

					if (mapper == 99 || mapper == 151) {
						header.control2 |= ROM_VSUNISYSTEM;
					}

				}
				fdsmakerID = fdsgameID = 0;
			} else {
				crc = crcall = crcvrom = 0;

				fdsmakerID = lpPRG[0x1F];
				fdsgameID  = (lpPRG[0x20]<<24)|(lpPRG[0x21]<<16)|(lpPRG[0x22]<<8)|(lpPRG[0x23]<<0);
			}
		} else {
		// NSF
			mapper = 0x0100;	// Private mapper
			crc = crcall = crcvrom = 0;
			fdsmakerID = fdsgameID = 0;
		}

		FREE( temp );
	} catch( CHAR* str ) {
		// #### Error handling is clear due to known cause.
		FCLOSE( fp );
		FREE( temp );
		FREE( bios );

		FREE( lpPRG );
		FREE( lpCHR );
		FREE( lpTrainer );
		FREE( lpDiskBios );
		FREE( lpDisk );

		throw	str;
#ifndef	_DEBUG
	} catch(...) {
		// #### I don't want to do/make anything...
		FCLOSE( fp );
		FREE( temp );
		FREE( bios );

		FREE( lpPRG );
		FREE( lpCHR );
		FREE( lpTrainer );
		FREE( lpDiskBios );
		FREE( lpDisk );

#ifdef	_DATATRACE
		// For dis...
		FREE( PROM_ACCESS );
#endif

		// #### An uncertain error has occurred.
		throw	CApp::GetErrorString( IDS_ERROR_UNKNOWN );
#endif	// !_DEBUG
	}
}

//
// #### Destroyer
//
ROM::~ROM()
{
	FREE( lpPRG );
	FREE( lpCHR );
	FREE( lpTrainer );
	FREE( lpDiskBios );
	FREE( lpDisk );
}

//
// #### ROM file check
//
INT	ROM::IsRomFile( const char* fname )
{
FILE*	fp = NULL;
NESHEADER	header;

	if( !(fp = fopen( fname, "rb" )) )
		return	IDS_ERROR_OPEN;

	// #### size-based reading
	if( fread( &header, sizeof(header), 1, fp ) != 1 ) {
		FCLOSE( fp );
		return	IDS_ERROR_READ;
	}
	FCLOSE( fp );

	if( header.ID[0] == 'N' && header.ID[1] == 'E'
	 && header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
		for( INT i = 0; i < 8; i++ ) {
			if( header.reserved[i] )
				return	IDS_ERROR_ILLEGALHEADER;
		}
		return	0;
	} else if( header.ID[0] == 'F' && header.ID[1] == 'D'
		&& header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
		return	0;
	} else if( header.ID[0] == 'N' && header.ID[1] == 'E'
		&& header.ID[2] == 'S' && header.ID[3] == 'M') {
		return	0;
	} else {
		LPBYTE	temp = NULL;
		LONG	size = 0;
		if( !UnCompress( fname, &temp, (LPDWORD)&size ) )
			return	IDS_ERROR_UNSUPPORTFORMAT;

		memcpy( &header, temp, sizeof(NESHEADER) );
		FREE( temp );
		if( header.ID[0] == 'N' && header.ID[1] == 'E'
		 && header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
			for( INT i = 0; i < 8; i++ ) {
				if( header.reserved[i] )
					return	IDS_ERROR_ILLEGALHEADER;
			}
			return	0;
		} else if( header.ID[0] == 'F' && header.ID[1] == 'D'
			&& header.ID[2] == 'S' && header.ID[3] == 0x1A ) {
			return	0;
		} else if( header.ID[0] == 'N' && header.ID[1] == 'E'
			&& header.ID[2] == 'S' && header.ID[3] == 'M') {
			return	0;
		}
	}

	return	IDS_ERROR_UNSUPPORTFORMAT;
}

//
// #### The translation of the Japanese text to English is:  Check the name of the ROM file (Automatic PAL detection)
//
void	ROM::FilenameCheck( const char* fname )
{
	unsigned char*	p = (unsigned char*)fname;

	while( *p != (unsigned char)'\0' ) {
		if( *p == (unsigned char)'(' ) {
			if( _mbsnbicmp( p, (unsigned char*)"(E)", 3 ) == 0 ) {
				bPAL = TRUE;
				return;
			}
		}

		p = _mbsinc(p);
	}
}

