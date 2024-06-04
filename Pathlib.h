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
	// #### Sure, here's the translation of the code snippet into English:  Get file name from path and filename (with extension)  In this code snippet, we are given a path and a filename (including extension) as input, and we need to return the file name without the extension. Here's how we can do it:  1. First, we use the `basename()` function to remove the directory path from the input path. The `basename()` function takes a string as input and returns the last component of the string (i.e., the part after the last `/`). So if our input path is `/path/to/file.txt`, then `basename('/path/to/file.txt')` will return `file.txt`. 2. Next, we use the `filename()` function to remove the filename without the extension. The `filename()` function takes a string as input and returns the portion of the string before the last `/`. So if our input filename is `file.txt`, then `filename('file.txt')` will return `file`. 3. Finally, we combine the results of steps 1 and 2 to get the final file name without the extension. In this case, it would be `file`.  Here's the complete code snippet: ```php $path = '/path/to/file.txt'; $file_name = basename($path); $file_extension = filename($path); echo $file_name . ' (' . $file_extension .')'; ``` In this code, we first define the input path `$path` and then use the `basename()` and `filename()` functions to remove the directory path and filename without extension, respectively. We then echo the resulting file name without extension.
	static	string	SplitFnameExt( LPCSTR lpszPath );
	// #### extract file extension from path and filename
	static	string	SplitExt( LPCSTR lpszPath );

	// #### +
	static	string	MakePath( LPCSTR lpszPath, LPCSTR lpszFname );
	// #### 拡張子
	static	string	MakePathExt( LPCSTR lpszPath, LPCSTR lpszFname, LPCSTR lpszExt );

	// #### 作成
	// #### If the additional path is absolute, then it will be added directly. If the additional path is relative, then it will be appended to the base path.
	static	string	CreatePath( LPCSTR lpszBasePath, LPCSTR lpszPath );

	// #### folder selection
	static	BOOL	SelectFolder( HWND hWnd, LPCSTR lpszTitle, LPSTR lpszFolder );

protected:
	static INT CALLBACK BffCallback( HWND hWnd, UINT uMsg, LPARAM lParam, WPARAM wParam );
private:
};

#endif	// !__CPATHLIB_INCLUDED__

