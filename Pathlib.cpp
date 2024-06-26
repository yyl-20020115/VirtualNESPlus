//
// #### Passive Reception Class
//
#include "Pathlib.h"

string	CPathlib::SplitPath( LPCSTR lpszPath )
{
	CHAR	szDrive[ _MAX_DRIVE ];
	CHAR	szDir  [ _MAX_DIR ];
	CHAR	szTemp [ _MAX_PATH+1 ];
	string	path;
	::_splitpath( lpszPath, szDrive, szDir, NULL, NULL );
	::_makepath( szTemp, szDrive, szDir, NULL, NULL );
	path = szTemp;
	return	path;
}

string	CPathlib::SplitFname( LPCSTR lpszPath )
{
	CHAR	szTemp [ _MAX_PATH+1 ];
	string	fname;
	::_splitpath( lpszPath, NULL, NULL, szTemp, NULL );
	fname = szTemp;
	return	fname;
}

string	CPathlib::SplitFnameExt( LPCSTR lpszPath )
{
	CHAR	szFname[ _MAX_FNAME ];
	CHAR	szExt  [ _MAX_EXT ];
	CHAR	szTemp [ _MAX_PATH+1 ];
	string	fname;
	::_splitpath( lpszPath, NULL, NULL, szFname, szExt );
	::_makepath( szTemp, NULL, NULL, szFname, szExt );
	fname = szTemp;
	return	fname;
}

string	CPathlib::SplitExt( LPCSTR lpszPath )
{
	CHAR	szExt[ _MAX_EXT ];
//	CHAR	szTemp[ _MAX_PATH+1 ];
	string	fname;
	::_splitpath( lpszPath, NULL, NULL, NULL, szExt );
	fname = szExt;
	return	fname;
}

string	CPathlib::MakePath( LPCSTR lpszPath, LPCSTR lpszFname )
{
	CHAR	szDrive[ _MAX_DRIVE ];
	CHAR	szDir  [ _MAX_DIR ];
	CHAR	szTemp [ _MAX_PATH+1 ];
	string	path;
	::_splitpath( lpszPath, szDrive, szDir, NULL, NULL );
	::_makepath( szTemp, szDrive, szDir, lpszFname, NULL );
	path = szTemp;
	return	path;
}

string	CPathlib::MakePathExt( LPCSTR lpszPath, LPCSTR lpszFname, LPCSTR lpszExt )
{
	CHAR	szDrive[ _MAX_DRIVE ];
	CHAR	szDir  [ _MAX_DIR ];
	CHAR	szTemp [ _MAX_PATH+1 ];
	string	path;
	::_splitpath( lpszPath, szDrive, szDir, NULL, NULL );
	::_makepath( szTemp, szDrive, szDir, lpszFname, lpszExt );
	path = szTemp;
	return	path;
}

string	CPathlib::CreatePath( LPCSTR lpszBasePath, LPCSTR lpszPath )
{
	CHAR	szPath[ _MAX_PATH ];
	string	path;
	if( ::PathIsRelative( lpszPath ) ) {
		CHAR	szTemp[ _MAX_PATH ];
		::strcpy( szTemp, lpszBasePath );
		::PathAppend( szTemp, lpszPath );
		::PathCanonicalize( szPath, szTemp );
	} else {
		::strcpy( szPath, lpszPath );
	}
	path = szPath;
	return	path;
}

INT CALLBACK	CPathlib::BffCallback( HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam )
{
	if( uMsg == BFFM_INITIALIZED && wParam ) {
		::SendMessage( hWnd, BFFM_SETSELECTION, TRUE, wParam );
	}
	return	TRUE;
}

BOOL	CPathlib::SelectFolder( HWND hWnd, LPCSTR lpszTitle, LPSTR lpszFolder )
{
	BROWSEINFO	bi;
	LPITEMIDLIST	pidl;

	ZeroMemory( &bi, sizeof(bi) );
	bi.hwndOwner = hWnd;
	bi.lpszTitle = lpszTitle;
	bi.ulFlags = BIF_RETURNONLYFSDIRS;
	// For Folder setup
	bi.lpfn = (BFFCALLBACK)BffCallback;

	// #### Why doesn't the default selection appear in Windows 98 when there is a double quote at the end...?
	if( lpszFolder ) {
		if( ::strlen(lpszFolder) > 3 ) {
			if( lpszFolder[::strlen(lpszFolder)-1] == '\\' )
				lpszFolder[::strlen(lpszFolder)-1] = NULL;
		}
		bi.lParam = (LPARAM)lpszFolder;
	} else {
		bi.lParam = NULL;
	}

	string	path;
	if( (pidl = ::SHBrowseForFolder( &bi )) ) {
		path.resize( _MAX_PATH+1 );
		::SHGetPathFromIDList( pidl, lpszFolder );
		if( ::strlen(lpszFolder) > 3 ) {	// #### Excluding the drive name
			::strcat( lpszFolder, "\\" );
		}
		IMalloc* pMalloc = 0;
		HRESULT hr = ::SHGetMalloc( &pMalloc );
		if(hr == S_OK && pMalloc!=0 ) {
			pMalloc->Free( pidl );
			pMalloc->Release();
		}
		return	TRUE;
	}

	return	FALSE;
}

