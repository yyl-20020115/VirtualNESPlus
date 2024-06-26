//
// #### Hey, let's open the window class.
//
#include "DebugOut.h"
#include "Wnd.h"

// Instance
CWndList	WndList;

list<CWnd*>	CWndList::m_WndPtrList;

// #### smelly
CWndList::CWndList()
{
//	m_WndPtrList.clear();
}

CWndList::~CWndList()
{
//	if( !m_WndPtrList.empty() )
//		m_WndPtrList.clear();
}

void	CWndList::Add( CWnd* pWnd )
{
	m_WndPtrList.push_back( pWnd );
}

void	CWndList::Del( CWnd* pWnd )
{
	for( list<CWnd*>::iterator it=m_WndPtrList.begin(); it!=m_WndPtrList.end(); ) {
		if( *it == pWnd ) {
			m_WndPtrList.erase(it);
			break;
		} else {
			++it;
		}
	}
}

BOOL	CWndList::IsDialogMessage( LPMSG msg )
{
	if( m_WndPtrList.empty() )
		return	FALSE;

	list<CWnd*>::iterator it=m_WndPtrList.begin();
	while( it != m_WndPtrList.end() ) {
		if( ::IsDialogMessage( (*it)->m_hWnd, msg ) )
			return	TRUE;
		++it;
	}

	return	FALSE;
}

CWnd::CWnd()
{
	m_hWnd = NULL;
	m_hMenu = NULL;
}

CWnd::~CWnd()
{
}

void	CWnd::SetThis()
{
	// #### Send a dispatch request to fill all CWnd windows.
	if( m_hWnd ) {
		::SetWindowLongPtr( m_hWnd, GWLP_USERDATA, (LONG_PTR)this );
	}
}

LRESULT	CALLBACK CWnd::g_WndProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// #### Automated actions before window opening.
	if( msg == WM_CREATE ) {
		LPCREATESTRUCT	lpcs = (LPCREATESTRUCT)lParam;
		CWnd* pWnd = (CWnd*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );
		if( !pWnd ) {
			// #### Embed the cursor in this area.
			::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)lpcs->lpCreateParams );
			// #### Self-window handle.
			pWnd = (CWnd*)lpcs->lpCreateParams;
			pWnd->m_hWnd = hWnd;
		}
	}
	// #### CWnd*
	CWnd* pWnd = (CWnd*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );

	if( pWnd ) {
		return	pWnd->DispatchWnd( hWnd, msg, wParam, lParam );
	} else {
		return	::DefWindowProc(  hWnd, msg, wParam, lParam );
	}
}

INT_PTR	CALLBACK CWnd::g_DlgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
	// #### assign tasks quietly beforehand.
	if( msg == WM_INITDIALOG ) {
		// #### DialogBoxParam
		// #### Undeclared identifier 'this'
		CWnd* pWnd = (CWnd*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );

		if( !pWnd ) {
			::SetWindowLongPtr( hWnd, GWLP_USERDATA, (LONG_PTR)lParam );
			pWnd = (CWnd*)lParam;
		}
		// #### Self-window handle.
		pWnd->m_hWnd = hWnd;

		// #### Move the dialogue to the center :)
		HWND hWndParent = ::GetParent( hWnd );
		if( hWndParent ) {
			RECT	rcParent, rc;
			::GetWindowRect( hWndParent, &rcParent );
			::GetWindowRect( hWnd, &rc );
			INT x = rcParent.left+(rcParent.right-rcParent.left)/2-(rc.right-rc.left)/2;
			INT y = rcParent.top +(rcParent.bottom-rcParent.top)/2-(rc.bottom-rc.top)/2;
//			DEBUGOUT( "X=%d  Y=%d\n", x, y );
			::SetWindowPos( hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE );
		}
	}

	// #### this
	CWnd* pWnd = (CWnd*)::GetWindowLongPtr( hWnd, GWLP_USERDATA );

	if( pWnd ) {
		return	pWnd->DispatchDlg( hWnd, msg, wParam, lParam );
	} else {
		return	FALSE;
	}
}

