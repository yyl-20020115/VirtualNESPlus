//
// #### Application Support Class
//
#include "VirtualNESres.h"
#include "App.h"

// #### From Japanese to English:  The argument passed from `_tWinMain` is passed directly.  In other words, the function `_tWinMain` receives the command line arguments as they are, without any modification or filtering. This is useful when you want to handle the command line arguments in a specific way, such as parsing them or passing them on to another function.
HINSTANCE CApp::m_hInstance = NULL;
HINSTANCE CApp::m_hPrevInstance = NULL;
LPTSTR	CApp::m_lpCmdLine = NULL;
INT	CApp::m_nCmdShow = 0;

// #### Instance handle of a language resource plugin.
HINSTANCE CApp::m_hPlugin = NULL;
// #### Window Handle of the Main Window
HWND	CApp::m_hWnd = NULL;
// #### Main Window Menu Handle
HMENU	CApp::m_hMenu = NULL;

// #### program path
CHAR	CApp::m_szModulePath[_MAX_PATH];
CHAR	CApp::m_szApplicationPath[_MAX_PATH];

// #### Error string (temporary).
CHAR	szErrorString[256];

// #### Error handling
INT	CApp::m_ErrorStringTableID[] = {
	IDS_ERROR,
	IDS_ERROR_STARTUP,
	IDS_ERROR_UNKNOWN,
	IDS_ERROR_OPEN,
	IDS_ERROR_READ,
	IDS_ERROR_WRITE,
	IDS_ERROR_OUTOFMEMORY,

	IDS_ERROR_ILLEGALOPCODE,
	IDS_ERROR_UNSUPPORTFORMAT,
	IDS_ERROR_INVALIDNESHEADER,
	IDS_ERROR_SMALLFILE,
	IDS_ERROR_UNSUPPORTMAPPER,
	IDS_ERROR_NODISKBIOS,
	IDS_ERROR_UNSUPPORTDISK,
	IDS_ERROR_ILLEGALDISKSIZE,

	IDS_ERROR_ILLEGALMAPPERNO,
	IDS_ERROR_ILLEGALHEADER,

	IDS_ERROR_ILLEGALSTATECRC,
	IDS_ERROR_ILLEGALMOVIEOLD,
	IDS_ERROR_ILLEGALMOVIEVER,
	IDS_ERROR_ILLEGALMOVIECRC,
	IDS_ERROR_ILLEGALMOVIEOLD_A,
	IDS_ERROR_ILLEGALMOVIEVER_A,
	IDS_ERROR_ILLEGALMOVIECRC_A,

	IDS_ERROR_NETWORKDISCONNECT,
	IDS_ERROR_NETWORKERROR,
	0,
};

CHAR	CApp::m_ErrorString[ERRORSTRING_MAX][256];

void	CApp::LoadErrorString()
{
	if( !m_hPlugin )
		return;

	for( INT i = 0; m_ErrorStringTableID[i]; i++ ) {
		LoadString( m_ErrorStringTableID[i], m_ErrorString[i], sizeof(m_ErrorString[i]) );
	}
}

CHAR*	CApp::GetErrorString( INT nID )
{
	for( INT i = 0; m_ErrorStringTableID[i]; i++ ) {
		if( m_ErrorStringTableID[i] == nID )
			return	m_ErrorString[i];
	}
	return	"";	// #### Pass nothing (or null) as a string.
}

