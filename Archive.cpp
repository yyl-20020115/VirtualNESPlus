//
// #### Archive File Operation
//
// Original:NesterJ arc.cpp arc.h by Mikami Kana
// Original:NNNesterJ ulunzip.cpp
//
// Zlib use!
// Reprogrammed by Norix
//
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlwapi.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mbstring.h>
#include <time.h>

#include "typedef.h"
#include "macro.h"
#include "DebugOut.h"

#include "App.h"
#include "Pathlib.h"

#include "VirtualNESres.h"

#define UNZ_BUFSIZE (65536)

#include "unzip.h"

#pragma	pack(1)
#define	FNAME_MAX32	512

typedef struct {
	DWORD 	dwOriginalSize;
 	DWORD 	dwCompressedSize;
	DWORD	dwCRC;
	UINT	uFlag;
	UINT	uOSType;
	WORD	wRatio;
	WORD	wDate;
	WORD 	wTime;
	char	szFileName[FNAME_MAX32 + 1];
	char	dummy1[3];
	char	szAttribute[8];
	char	szMode[8];
} INDIVIDUALINFO, *LPINDIVIDUALINFO;
#pragma pack()

// Un??? use function
typedef	int(WINAPI *EXECUTECOMMAND)(HWND,LPCSTR,LPSTR,const DWORD);
typedef	BOOL(WINAPI *CHECKARCHIVE)(LPCSTR,const int);
typedef	int(WINAPI *EXTRACTMEM)(HWND,LPCSTR,LPBYTE,const DWORD,time_t,LPWORD,LPDWORD);
typedef	HGLOBAL(WINAPI *OPENARCHIVE)(HWND,LPCSTR,const DWORD);
typedef	int(WINAPI *CLOSEARCHIVE)(HGLOBAL);
typedef	int(WINAPI *FINDFIRST)(HGLOBAL,LPCSTR,INDIVIDUALINFO*);

static	LPCSTR	pszArchiver[] = {
	"UNLHA32",
	"UNZIP32",
	"UNRAR32",
	"CAB32",
	NULL
};

static	LPCSTR	pszFuncPrefix[] = {
	"Unlha",
	"UnZip",
	"Unrar",
	"Cab",
};

static	LPCSTR	pszCommand[] = {
	NULL,
	NULL,
	"-e -u \"%s\" \"%s\" \"%s\"",
	"-x -j \"%s\" \"%s\" \"%s\"",
};

static	LPCSTR	pszExtension[] = {
	"*.nes",
	"*.fds",
	"*.nsf",
	NULL
};

static	BOOL	bFileMatching[] = {
	FALSE,
	TRUE,
	FALSE,
	FALSE,
};

#define	FREEDLL(h)	if( h ) { FreeLibrary(h);h=NULL; }

#define M_ERROR_MESSAGE_OFF		0x00800000L

// #### Using zlib, a routine for unzipping a ZIP file.
BOOL	ZlibUnZip( LPCSTR fname, LPBYTE* ppBuf, LPDWORD lpdwSize )
{
	unzFile		unzipFile = NULL;
	unz_global_info	unzipGlobalInfo;
	unz_file_info	unzipFileInfo;
	char		fname_buf[256];

	*ppBuf = NULL;
	*lpdwSize = 0;

	if( !(unzipFile = unzOpen( (const char*)fname )) )
		return	FALSE;

	if( unzGetGlobalInfo( unzipFile, &unzipGlobalInfo ) != UNZ_OK ) {
		unzClose( unzipFile );
		return	FALSE;
	}

	for( uLong i = 0; i < unzipGlobalInfo.number_entry; i++ ) {
		if( unzGetCurrentFileInfo( unzipFile, &unzipFileInfo, fname_buf, sizeof(fname_buf), NULL, 0, NULL, 0 ) != UNZ_OK )
			break;

		char*	pExt = ::PathFindExtension( fname_buf );
		if( _stricmp( pExt, ".nes" ) == 0 || _stricmp( pExt, ".fds" ) == 0 || _stricmp( pExt, ".nsf" ) == 0 ) {
			if( unzipFileInfo.uncompressed_size ) {
				if( unzOpenCurrentFile( unzipFile ) != UNZ_OK )
					break;

				if( unzipFileInfo.uncompressed_size > 0 ) {
					if( !(*ppBuf = (LPBYTE)::malloc( unzipFileInfo.uncompressed_size )) )
						break;

					uInt	size = unzReadCurrentFile( unzipFile, *ppBuf, unzipFileInfo.uncompressed_size );
					if( size != unzipFileInfo.uncompressed_size )
						break;
				}
				*lpdwSize = unzipFileInfo.uncompressed_size;

				if( unzCloseCurrentFile( unzipFile ) != UNZ_OK )
					break;
				unzClose( unzipFile );
				return	TRUE;
			}
		}

		// Next file
		if( (i+1) < unzipGlobalInfo.number_entry ) {
			if( unzGoToNextFile( unzipFile ) != UNZ_OK ) {
				break;
			}
		}
	}

	FREE( *ppBuf );

	if( unzipFile ) {
		unzCloseCurrentFile( unzipFile );
		unzClose( unzipFile );
	}
	return	FALSE;
}

BOOL	UnCompress( LPCSTR fname, LPBYTE* ppBuf, LPDWORD lpdwSize )
{
HMODULE		hDLL;
INDIVIDUALINFO	idvinfo;

//	char*	pExt = ::PathFindExtension( fname );
//	if( _stricmp( pExt, ".zip" ) == 0 ) {
		// #### Let's try using the zlib library to unzip the ZIP file.
		if( ZlibUnZip( fname, ppBuf, lpdwSize ) ) {
//DEBUGOUT( "zlib unzip ok! [%s]\n", fname );
			return	TRUE;
		}
//	}

	hDLL = NULL;
	for( INT i = 0; pszArchiver[i]; i++ ) {
		// #### dll unload
		FREEDLL( hDLL );

		// #### DLL Load
		if( !(hDLL = LoadLibrary( pszArchiver[i] )) )
			continue;

		CHAR	szTemp[256];
		sprintf( szTemp, "%sCheckArchive", pszFuncPrefix[i] );
		CHECKARCHIVE	CheckArchive;
		if( !(CheckArchive = (CHECKARCHIVE)GetProcAddress( hDLL, szTemp )) )
			continue;
		// #### Check the correspondence archive.
		if( !CheckArchive( fname, 1 ) )
			continue;

		// #### Check if there is a corresponding file exists in the archive.
		OPENARCHIVE	OpenArchive;
		CLOSEARCHIVE	CloseArchive;
		FINDFIRST	FindFirst;

		sprintf( szTemp, "%sOpenArchive", pszFuncPrefix[i] );
		OpenArchive = (OPENARCHIVE)GetProcAddress( hDLL, szTemp );
		sprintf( szTemp, "%sFindFirst", pszFuncPrefix[i] );
		FindFirst = (FINDFIRST)GetProcAddress( hDLL, szTemp );
		sprintf( szTemp, "%sCloseArchive", pszFuncPrefix[i] );
		CloseArchive = (CLOSEARCHIVE)GetProcAddress( hDLL, szTemp );

		HGLOBAL		hARC;
		BOOL	bFound = FALSE;
		for( INT j = 0; pszExtension[j]; j++ ) {
			if( !(hARC = OpenArchive( NULL, fname, M_ERROR_MESSAGE_OFF ) ) ) {
				CloseArchive( hARC );
				break;
			}
			INT	ret = FindFirst( hARC, pszExtension[j], &idvinfo );
			CloseArchive( hARC );
			if( ret == 0 ) {		// Found!!
				bFound = TRUE;
				break;
			} else if( ret == -1 ) {	// Not found.
			} else {			// #### Abnormal termination
				break;
			}
		}
		if( !bFound )
			continue;

		if( !pszCommand[i] ) {
		// #### Memory Unfreeze (LHA32, ZIP32)
			*lpdwSize = idvinfo.dwOriginalSize;
			*ppBuf = (LPBYTE)malloc( *lpdwSize );

			CHAR	szCmd [256];
			CHAR	szFunc[256];

			if( !bFileMatching[i] ) {
				sprintf( szCmd, "\"%s\" \"%s\"", fname, idvinfo.szFileName );
			} else {
			// UNZIP32 only
				BYTE	szFile[FNAME_MAX32+1];
				LPBYTE	lpF0, lpF1;

				// #### I wanted to cut off options
				lpF0 = (LPBYTE)idvinfo.szFileName;
				lpF1 = szFile;
				while( *lpF0 ) {
					if( *lpF0 == '[' || *lpF0 == ']' ) {
						*lpF1++ = '\\';
					}
					_mbsncpy( lpF1, lpF0, 1 );
					lpF0 = _mbsinc( lpF0 );
					lpF1 = _mbsinc( lpF1 );
				}
				*lpF1 = '\0';

				sprintf( szCmd, "\"%s\" \"%s\"", fname, szFile );
			}
			sprintf( szFunc, "%sExtractMem", pszFuncPrefix[i] );

			EXTRACTMEM	ExtractMem;
			ExtractMem = (EXTRACTMEM)GetProcAddress( hDLL, szFunc );
			INT ret = ExtractMem( NULL, szCmd, (LPBYTE)(*ppBuf), *lpdwSize, NULL, NULL, NULL );
			FREEDLL( hDLL );
			if( ret == 0 )
				return TRUE;
		} else {
		// #### If the memory thawing does not occur, it means that the frozen memory cannot be restored or accessed. This can happen for a variety of reasons, including:  1. Hardware failure: If the hardware that contains the frozen memory fails or is damaged beyond repair, the memory may be irretrievable. 2. Data corruption: If the data on the frozen memory is corrupted or damaged during the freezing process, it may not be possible to restore it. 3. Lack of technology: In some cases, the technology required to thaw and access frozen memories may not be available or accessible. 4. Legal restrictions: Depending on the laws and regulations in place, there may be legal restrictions on who can access or modify frozen memories. 5. Ethical considerations: There may be ethical considerations that prevent the thawing of frozen memories, such as respect for privacy or the potential impact on the individual's identity.  In general, the concept of memory thawing is still in its infancy and there are many challenges and limitations associated with it.
			CHAR	szCmd [256];
			CHAR	szTempPath[_MAX_PATH];
			EXECUTECOMMAND	ExecuteCommand;

			GetTempPath( _MAX_PATH, szTempPath );
//DEBUGOUT( "TempPath:%s\n", szTempPath );

			sprintf( szCmd, pszCommand[i], fname, szTempPath, idvinfo.szFileName );
			ExecuteCommand = (EXECUTECOMMAND)GetProcAddress( hDLL, pszFuncPrefix[i] );
			ExecuteCommand( NULL, szCmd, NULL, 0 );
			FREEDLL( hDLL );

			string	FileName = CPathlib::MakePath( szTempPath, idvinfo.szFileName );

			FILE *fp = NULL;
			if( (fp = fopen( FileName.c_str(), "rb" )) ) {
				// #### File size retrieval
				fseek( fp, 0, SEEK_END );
				*lpdwSize = ftell( fp );
				fseek( fp, 0, SEEK_SET );
				if( *lpdwSize < 17 ) {
					// #### The file size is too small.
					throw	CApp::GetErrorString( IDS_ERROR_SMALLFILE );
				}

				// #### Temporary memory guarantee
				if( !(*ppBuf = (LPBYTE)malloc( *lpdwSize )) ) {
					FCLOSE( fp );
					// #### I can't secure memory.
					throw	CApp::GetErrorString( IDS_ERROR_OUTOFMEMORY );
				}
				// #### Size-based inlining is a technique used by compilers and interpreters to improve program performance by inlining small functions based on their size. The basic idea is that small functions are less likely to be inlined due to the overhead of determining whether or not to inline them, so the compiler can make more informed decisions about which small functions to inline based on their size.  For example, if a function has a fixed size within a certain range (e.g., 100 bytes), the compiler may choose to inline it even if it would normally not be inlined due to its small size. This can help improve program performance by reducing the number of function calls and improving cache behavior.  Size-based inlining is typically implemented using a combination of static analysis and heuristics. The compiler analyzes the source code to determine the size of each function, and then uses heuristics to decide which functions to inline based on their size. For example, the compiler may choose to inline functions that are less than a certain size threshold (e.g., 100 bytes) more frequently than larger functions.  Size-based inlining can be applied at different levels of optimization, from simple heuristics to more advanced techniques such as profile-guided inlining. Profile-guided inlining uses profiling information to guide the inlining decision, choosing to inline functions that are most frequently called and have the largest impact on performance.  Overall, size-based inlining is a powerful technique for improving program performance by making more informed decisions about which small functions to inline based on their size.
				if( fread( *ppBuf, *lpdwSize, 1, fp ) != 1 ) {
					FCLOSE( fp );
					FREE( *ppBuf );
					// #### The file could not be loaded.
					throw	CApp::GetErrorString( IDS_ERROR_READ );
				}
				FCLOSE( fp );
				DeleteFile( FileName.c_str() );
			} else {
				// #### I cannot open the xxx file.
				LPCSTR	szErrStr = CApp::GetErrorString( IDS_ERROR_OPEN );
				sprintf( szErrorString, szErrStr, fname );
				throw	szErrorString;
			}
			return	TRUE;
		}
	}
	FREEDLL( hDLL );

	return	FALSE;
}
// Archive
