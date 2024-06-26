//
// #### Passive Library Class
//
#ifndef	__CPATHLIB_INCLUDED__
#define	__CPATHLIB_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <shlobj.h>
#include <shlwapi.h>

#include <string>
using namespace std;

class	CPathlib
{
public:
	// #### Get the path of a file based on its full path and file name
	static	string	SplitPath( LPCSTR lpszPath );
	// #### Obtaining a file name from a path and filename (without extension).
	static	string	SplitFname( LPCSTR lpszPath );
	// #### Get file name from path and filename (with extension) 
	static	string	SplitFnameExt( LPCSTR lpszPath );
	// #### extract file extension from path and filename
	static	string	SplitExt( LPCSTR lpszPath );

	// #### +
	static	string	MakePath( LPCSTR lpszPath, LPCSTR lpszFname );
	// #### Extension
	static	string	MakePathExt( LPCSTR lpszPath, LPCSTR lpszFname, LPCSTR lpszExt );

	// #### Create
	// #### If the additional path is absolute, then it will be added directly. If the additional path is relative, then it will be appended to the base path.
	static	string	CreatePath( LPCSTR lpszBasePath, LPCSTR lpszPath );

	// #### folder selection
	static	BOOL	SelectFolder( HWND hWnd, LPCSTR lpszTitle, LPSTR lpszFolder );

protected:
	static INT CALLBACK BffCallback( HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam );
private:
};

#endif	// !__CPATHLIB_INCLUDED__

