//
// #### Message Filtering Support Class
//
#include "DebugOut.h"

#include "App.h"
#include "Wnd.h"
#include "WndHook.h"

// Instance
CWndHook	WndHook;

BOOL	CWndHook::m_bMsgFiltering = FALSE;
BOOL	CWndHook::m_bMsgFilter = FALSE;
HHOOK 	CWndHook::m_hOldMsgFilter = NULL;

// #### Initializing the message filter hook after the window is initialized.
void	CWndHook::Initialize()
{
	m_hOldMsgFilter = ::SetWindowsHookEx( WH_MSGFILTER, (HOOKPROC)MessageFilterProc, NULL, ::GetCurrentThreadId() );
}

void	CWndHook::Release()
{
	if( m_hOldMsgFilter ) {
		::UnhookWindowsHookEx( m_hOldMsgFilter );
		m_hOldMsgFilter = NULL;
	}
}

// #### Message filtering processing (Dialogs are not shown for messages that are common and do not require a response)
LRESULT	CALLBACK CWndHook::MessageFilterProc( INT code, WPARAM wParam, LPARAM lParam )
{
	if( code < 0 ) {
		return	::CallNextHookEx( m_hOldMsgFilter, code, wParam, lParam );
	}

	if( m_bMsgFiltering && code == MSGF_DIALOGBOX ) {
		// #### Already filtering?
		if( m_bMsgFilter )
			return	FALSE;
		m_bMsgFilter = TRUE;
		LPMSG	lpMsg = (LPMSG)lParam;
		if( lpMsg->message >= WM_KEYFIRST && lpMsg->message <= WM_KEYLAST ) {
			if( WalkPreTranslateTree( lpMsg ) ) {
				m_bMsgFilter = FALSE;
				return	1L;
			}
		}
		m_bMsgFilter = FALSE;
	}

	return	::CallNextHookEx( m_hOldMsgFilter, code, wParam, lParam );
//	return	FALSE;
}

BOOL	CWndHook::WalkPreTranslateTree( MSG* lpMsg )
{
	// #### Main Window
	HWND	hWndStop = CApp::GetHWnd();

	// #### Before Translation Message
	for( HWND hWnd = lpMsg->hwnd; hWnd != NULL; hWnd = ::GetParent(hWnd) ) {
		if( hWnd == hWndStop )
			break;
		CWnd*	pWnd = (CWnd*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( pWnd ) {
			if( pWnd->PreTranslateMessage( lpMsg ) )
				return	TRUE;
		}
		if( hWnd == hWndStop )
			break;
	}
	return	FALSE;
}

