#define	INITGUID
#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <crtdbg.h>

#include <string>
using namespace std;

#include "DebugOut.h"
#include "VirtualNESres.h"

#include "App.h"
#include "Registry.h"
#include "Pathlib.h"
#include "MMTimer.h"

#include "Wnd.h"
#include "WndHook.h"
#include "MainFrame.h"
#include "Plugin.h"
#include "Config.h"
#include "Recent.h"

#include "DirectDraw.h"
#include "DirectSound.h"
#include "DirectInput.h"

#include "SimpleVirusChecker.h"


INT WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow )
{
#if	_DEBUG
	// #### Memory Leak Detection
	int	Flag = _CrtSetDbgFlag( _CRTDBG_REPORT_FLAG );
	Flag |= _CRTDBG_LEAK_CHECK_DF;
	Flag &= ~_CRTDBG_CHECK_ALWAYS_DF;
	_CrtSetDbgFlag( Flag );
#endif

	// #### Easy Virus Check
	if( SimpleVirusChecker() > 0 ) {
		LCID lcid = ::GetUserDefaultLCID();
		//2052
		if(lcid == 2052 ) {
			if( ::MessageBox( NULL, "此计算机可能已经被病毒感染.\n"
						"请尽早进行查杀!\n\n"
						"是否继续执行?", "VirtualNES 简易病毒检测器", MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON2 ) == IDNO )
				return	-1L;
		} else {
			if( ::MessageBox( NULL, "This PC may be infected with a virus program!!!\n"
						"Should become danger, and please do a check to it immediately!!!\n\n"
						"Do execute even it?", "VirtualNES simple virus checker", MB_ICONWARNING|MB_YESNO|MB_DEFBUTTON2 ) == IDNO )
				return	-1L;
		}
	}

	// #### Main Frame Window Object
	CMainFrame	MainFrame;

	// Mutex
	HANDLE	hMutex = NULL;

	// #### Application instance settings.
	CHAR	szPath[_MAX_PATH] = { 0 };
	GetModuleFileName( hInstance, szPath, sizeof(szPath) );
	CApp::SetApplicationPath(szPath);
	string	ModulePath = CPathlib::SplitPath( szPath );
	CApp::SetModulePath( ModulePath.c_str() );

	DEBUGOUT( "Module Path:\"%s\"\n", ModulePath.c_str() );

	CApp::SetInstance( hInstance );
	CApp::SetPrevInstance( hPrevInstance );
	CApp::SetCmdLine( lpCmdLine );
	CApp::SetCmdShow( nCmdShow );

	CRegistry::SetRegistryKey( "VirtualNESPlus.ini" );

	if( !CPlugin::FindPlugin( CApp::GetModulePath() ) ) {
		::MessageBox( NULL, "Language plug-in is not found.", "VirtualNES", MB_ICONERROR|MB_OK );
		goto	_Error_Exit;
	}
	DEBUGOUT( "Plugin Path:\"%s\"\n", CPlugin::GetPluginPath() );
	DEBUGOUT( "Language   :\"%s\"\n", CPlugin::GetPluginLanguage() );
	DEBUGOUT( "LCID       :\"%d\" \"0x%04X\"\n", CPlugin::GetPluginLocaleID(), CPlugin::GetPluginLocaleID() );

	HINSTANCE hPlugin;
	if( !(hPlugin = CPlugin::LoadPlugin()) ) {
		::MessageBox( NULL, "Language plug-in load failed.", "VirtualNES", MB_ICONERROR|MB_OK );
		goto	_Error_Exit;
	}
	CApp::SetPlugin( hPlugin );
	::InitCommonControls();
	// #### Loading Setting
	CRegistry::SetRegistryKey( "VirtualNESPlus.ini" );
	Config.Load();
	CRecent::Load();

	// #### Two-stage start-up prevention
	hMutex = ::CreateMutex( NULL, FALSE, VIRTUALNES_MUTEX );
	if( ::GetLastError() == ERROR_ALREADY_EXISTS ) {
		::CloseHandle( hMutex );
		if( Config.general.bDoubleExecute ) {
			HWND	hWnd = ::FindWindow( VIRTUALNES_WNDCLASS, NULL );
//			HWND	hWnd = ::FindWindow( VIRTUALNES_WNDCLASS, VIRTUALNES_CAPTION );

			CHAR	szTitle[256] = { 0 };
			::GetWindowText( hWnd, szTitle, sizeof(szTitle)-1 );

			// #### Title bar is the same, check if it's the same.
			if( ::strncmp( szTitle, VIRTUALNES_CAPTION, ::strlen(VIRTUALNES_CAPTION) ) == 0 ) {
				// #### Activate the person who is in front of you
				::SetForegroundWindow( hWnd );

				// #### If the command line argument is present, display the file name in the window of the VirtualNES emulator that is currently running.
				// #### to make it work over there
				// #### (Obviously, without the automatic response version, it's not possible.)
				if( ::strlen( lpCmdLine ) > 0 ) {
					CHAR	szCmdLine[_MAX_PATH];
					::strcpy( szCmdLine, lpCmdLine );
					::PathUnquoteSpaces( szCmdLine );

					COPYDATASTRUCT	cds;
					cds.dwData = 0;
					cds.lpData = (void*)szCmdLine;
					cds.cbData = (DWORD)::strlen(szCmdLine) + 1; // #### NULL
					// #### Sending a sequence of characters
					::SendMessage( hWnd, WM_COPYDATA, (WPARAM)NULL, (LPARAM)&cds );
				}

				// #### ended
				goto	_DoubleExecute_Exit;
			}
		}
	}

	if( !MainFrame.Create(NULL) )
		goto	_Error_Exit;
	DEBUGOUT( "CreateWindow ok.\n" );

	// #### Main window display
	::ShowWindow( CApp::GetHWnd(), CApp::GetCmdShow() );
	::UpdateWindow( CApp::GetHWnd() );

	// #### good luck
	CWndHook::Initialize();

	// #### Simultaneous launch
	if( Config.general.bStartupLauncher ) {
		::PostMessage( CApp::GetHWnd(), WM_COMMAND, ID_LAUNCHER, 0 );
	}

	// #### command line
	if( ::strlen( lpCmdLine ) > 0 ) {
		LPSTR	pCmd = lpCmdLine;
		if( lpCmdLine[0] == '"' ) {	// Shell execute!!
			lpCmdLine++;
			if( lpCmdLine[::strlen( lpCmdLine )-1] == '"' ) {
				lpCmdLine[::strlen( lpCmdLine )-1] = '\0';
			}
		}
	}

	if( ::strlen( lpCmdLine ) > 0 ) {
		::PostMessage( CApp::GetHWnd(), WM_VNS_COMMANDLINE, 0, (LPARAM)lpCmdLine );
	}

	MSG	msg;
	BOOL	bRet;
	while( (bRet = ::GetMessage( &msg, NULL, 0, 0 )) != 0 ) {
		// #### Error
		if( bRet == -1 )
			break;
		// #### メインウインドウのメッセージフィルタリング
		if( CApp::GetHWnd() == msg.hwnd ) {
			CWnd* pWnd = (CWnd*)::GetWindowLongPtr( msg.hwnd, GWLP_USERDATA );
			if( pWnd ) {
				if( pWnd->PreTranslateMessage( &msg ) )
					continue;
			}
		}
		if( CWndList::IsDialogMessage( &msg ) )
			continue;
		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}
	// #### good fortune.
	CWndHook::Release();

	// #### Setting Save
	CRegistry::SetRegistryKey( "VirtualNESPlus.ini" );
	Config.Save();
	CRecent::Save();

	// #### DirectX System Discontinued
	DirectDraw.ReleaseDDraw();
	DirectSound.ReleaseDSound();
	DirectInput.ReleaseDInput();

	if( hMutex )
		::ReleaseMutex( hMutex );
	CLOSEHANDLE( hMutex );

_DoubleExecute_Exit:
	::FreeLibrary( CApp::GetPlugin() );

	return	(int)msg.wParam;

_Error_Exit:
	// #### Abandon DirectX system
	DirectDraw.ReleaseDDraw();
	DirectSound.ReleaseDSound();
	DirectInput.ReleaseDInput();

	if( CApp::GetPlugin() ) {
		::FreeLibrary( CApp::GetPlugin() );
	}

	return	-1;
}

