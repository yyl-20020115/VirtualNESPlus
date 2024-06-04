//
// #### Chat Room Dialogue Class
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <mbstring.h>
#include <shlwapi.h>

#include <string>
using namespace std;

#include "typedef.h"
#include "macro.h"

#include "VirtualNESres.h"
#include "DebugOut.h"
#include "App.h"
#include "Pathlib.h"
#include "Config.h"

#include "NetPlay.h"

#include "Wnd.h"
#include "ChatDlg.h"

// #### 
DLG_MESSAGE_BEGIN(CChatDlg)
DLG_ON_MESSAGE( WM_INITDIALOG,	OnInitDialog )
DLG_ON_MESSAGE( WM_DESTROY,	OnDestroy )
DLG_ON_MESSAGE( WM_CLOSE,	OnClose )
DLG_ON_MESSAGE( WM_ACTIVATE,	OnActivate )
DLG_ON_MESSAGE( WM_SETCURSOR,	OnSetCursor )
DLG_ON_MESSAGE( WM_SIZE,	OnSize )
DLG_ON_MESSAGE( WM_CTLCOLORSTATIC, OnControlColorStatic )
DLG_ON_MESSAGE( WM_COPYDATA,	OnCopyData )

// #### command
DLG_COMMAND_BEGIN()
DLG_ON_COMMAND_NOTIFY( IDC_NCT_MESSAGE, EN_SETFOCUS, OnMessageFocus )
DLG_ON_COMMAND( IDOK, OnOK )
DLG_ON_COMMAND( IDCANCEL, OnCancel )
DLG_ON_COMMAND( IDC_NCT_SEND, OnSend )
DLG_COMMAND_END()
// #### Message
DLG_NOTIFY_BEGIN()
DLG_NOTIFY_END()
DLG_MESSAGE_END()

BOOL	CChatDlg::Create( HWND hWndParent )
{
	// #### The mother and father are placing on desktop.
	m_hWnd = ::CreateDialogParam( CApp::GetPlugin(), MAKEINTRESOURCE(IDD_NETPLAY_CHAT),
				NULL, g_DlgProc, (LPARAM)this );
	if( !m_hWnd )
		return	FALSE;

	// #### Adding a modeless dialogue list to your code will allow you to create a more flexible and responsive user interface. Here are some ways in which you can add a modeless dialogue list to your code:  1. Use a `List` component: You can use a `List` component to display the dialogue list. This will allow you to easily add and remove items from the list, as well as style the list to match the rest of your interface. 2. Use a `React.Fiber.List`: If you are using React Fiber for your UI, you can use the `React.Fiber.List` component to create a modeless dialogue list. This will allow you to take advantage of the performance optimizations and other features of React Fiber. 3. Use a `Grid` component: If you want to display the dialogue list in a grid layout, you can use a `Grid` component. This will allow you to easily style the list and make it look more visually appealing. 4. Use a `Card` component: If you want to display each item in the dialogue list as a card, you can use a `Card` component. This will give the list a more polished and professional look. 5. Use a `Modal` component: If you want to display the dialogue list in a modal window, you can use a `Modal` component. This will allow you to easily style the list and make it look more visually appealing. 6. Use a `Dialog` component: If you want to display the dialogue list in a dialog window, you can use a `Dialog` component. This will allow you to easily style the list and make it look more visually appealing. 7. Use a `Popover` component: If you want to display the dialogue list as a popover, you can use a `Popover` component. This will allow you to easily style the list and make it look more visually appealing. 8. Use a `Menu` component: If you want to display the dialogue list as a menu, you can use a `Menu` component. This will allow you to easily style the list and make it look more visually appealing. 9. Use a `Button` component: If you want to display the dialogue list as a button, you can use a `Button` component. This will allow you to easily style the list and make it look more visually appealing. 10. Use a `Icon` component: If you want to display the dialogue list as an icon, you can use a `Icon` component. This will allow you to easily style the list and make it look more visually appealing.  By adding a modeless dialogue list to your code, you can create a more flexible and responsive user interface that is easy to use and navigate.
	CWndList::Add( this );

	return	TRUE;
}

void	CChatDlg::Destroy()
{
	if( m_hWnd ) {
		// #### Remove Unnecessary Dialogue List
		CWndList::Del( this );

		::GetWindowRect( m_hWnd, &Config.netplay.rcChatPos );

		::DestroyWindow( m_hWnd );
		m_hWnd = NULL;
	}
}

void	CChatDlg::SetEditText()
{
	CHAR	szText[256+1];

	::GetWindowText( ::GetDlgItem(m_hWnd, IDC_NCT_EDIT), szText, sizeof(szText)-1 );
	::SetWindowText( ::GetDlgItem(m_hWnd, IDC_NCT_EDIT), "" );

	if( ::strlen(szText) ) {
		// clear message window
		if( ::StrCmpI( szText, "/clear" ) == 0 ) {
			::SendDlgItemMessage( m_hWnd, IDC_NCT_MESSAGE, EM_SETSEL, 0, -1 );
			::SendDlgItemMessage( m_hWnd, IDC_NCT_MESSAGE, EM_REPLACESEL, FALSE, (WPARAM)"" );
			return;
		}

		string	str;

		if( NetPlay.IsConnect() ) {
			str = "(";
			str = str + Config.netplay.szNick;
			str = str + ") ";
		}

		str = str + szText;
		str = str + "\r\n";

		// #### Sending to the opposite party
		if( NetPlay.IsConnect() ) {
			NetPlay.ChatSend( (LPSTR)str.c_str() );
		}

		// #### Display Personal Message
		INT n = GetWindowTextLength( GetDlgItem( m_hWnd, IDC_NCT_MESSAGE ) );
		::SendDlgItemMessage( m_hWnd, IDC_NCT_MESSAGE, EM_SETSEL, (WPARAM)n, (LPARAM)n );
		::SendDlgItemMessage( m_hWnd, IDC_NCT_MESSAGE, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)str.c_str() );
	}
}

DLGMSG	CChatDlg::OnCopyData( DLGMSGPARAM )
{
	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;
	CHAR*	lpStr = (CHAR*)pcds->lpData;

	INT n = GetWindowTextLength( GetDlgItem( hWnd, IDC_NCT_MESSAGE ) );
	::SendDlgItemMessage( hWnd, IDC_NCT_MESSAGE, EM_SETSEL, (WPARAM)n, (LPARAM)n );
	::SendDlgItemMessage( hWnd, IDC_NCT_MESSAGE, EM_REPLACESEL, (WPARAM)TRUE, (LPARAM)lpStr );

	if( ::GetFocus() != m_hWnd && ::strlen( lpStr ) > 0 ) {
		::MessageBeep( MB_OK );
//		::PlaySound( "MailBeep", NULL, SND_ALIAS|SND_ASYNC );
	}

	// #### When a message comes, display suddenly or unexpectedly
	::SendMessage( CApp::GetHWnd(), WM_VNS_CHATPOPUP, 0, 0 );

	return	TRUE;
}

DLGMSG	CChatDlg::OnInitDialog( DLGMSGPARAM )
{
//	DEBUGOUT( "CChatDlg::OnInitDialog\n" );

	NetPlay.SetChatWnd( m_hWnd );

	// #### Position information storage
	::GetClientRect( m_hWnd, &m_rcClient );
	::GetWindowRect( ::GetDlgItem(m_hWnd, IDC_NCT_MESSAGE), &m_rcMessage );
	::GetWindowRect( ::GetDlgItem(m_hWnd, IDC_NCT_EDIT), &m_rcEdit );
	::GetWindowRect( ::GetDlgItem(m_hWnd, IDC_NCT_SEND), &m_rcButton );

	// #### Transform to client coordinates
	::ScreenToClient( m_hWnd, (POINT*)&m_rcMessage.left );
	::ScreenToClient( m_hWnd, (POINT*)&m_rcMessage.right );
	::ScreenToClient( m_hWnd, (POINT*)&m_rcEdit.left );
	::ScreenToClient( m_hWnd, (POINT*)&m_rcEdit.right );
	::ScreenToClient( m_hWnd, (POINT*)&m_rcButton.left );
	::ScreenToClient( m_hWnd, (POINT*)&m_rcButton.right );

	// #### Window position/size settings
//	RECT	rc = Config.launcher.rcWindowPos;
//	if( (rc.right-rc.left) && (rc.bottom-rc.top) ) {
//		::SetWindowPos( m_hWnd, NULL, rc.left, rc.top, RCWIDTH(rc), RCHEIGHT(rc), SWP_NOZORDER );
//	}

	// #### Window position/size setting
	RECT	rc = Config.netplay.rcChatPos;
	if( (rc.right-rc.left) && (rc.bottom-rc.top) ) {
		::SetWindowPos( m_hWnd, NULL, rc.left, rc.top, RCWIDTH(rc), RCHEIGHT(rc), SWP_NOZORDER );
	}

	// #### show
	::ShowWindow( m_hWnd, SW_SHOW );

	return	TRUE;
}

DLGMSG	CChatDlg::OnDestroy( DLGMSGPARAM )
{
	return	TRUE;
}

DLGMSG	CChatDlg::OnClose( DLGMSGPARAM )
{
	::ShowWindow( m_hWnd, SW_HIDE ); // #### Hide only.
	return	TRUE;
}

DLGMSG	CChatDlg::OnActivate( DLGMSGPARAM )
{
	if( LOWORD(wParam) == WA_INACTIVE ) {
//		DEBUGOUT( "CChatDlg::OnActivate:Inactive\n" );
		::PostMessage( CApp::GetHWnd(), WM_VNS_SHORTCUTENABLE, (WPARAM)TRUE, 0 );

		Config.InputKeyboardDisable( FALSE );
	} else {
//		DEBUGOUT( "CChatDlg::OnActivate:Active\n" );
		::PostMessage( CApp::GetHWnd(), WM_VNS_SHORTCUTENABLE, (WPARAM)FALSE, 0 );

		Config.InputKeyboardDisable( TRUE );
	}
	return	TRUE;
}

DLGMSG	CChatDlg::OnSetCursor( DLGMSGPARAM )
{
//	DEBUGOUT( "CChatDlg::OnSetCursor\n" );
	return	FALSE;
}

DLGMSG	CChatDlg::OnSize( DLGMSGPARAM )
{
//	DEBUGOUT( "CChatDlg::OnSize\n" );
	HWND	hWndCtrl;
	RECT	rcC, rcT;
	::GetClientRect( m_hWnd, &rcC );

	// #### Message frame
	if( (hWndCtrl = ::GetDlgItem( m_hWnd, IDC_NCT_MESSAGE )) ) {
		rcT.left   = rcC.left;
		rcT.right  = rcC.right;
		rcT.top    = rcC.top;
		rcT.bottom = rcC.bottom - (m_rcClient.bottom-m_rcMessage.bottom);
		::MoveWindow( hWndCtrl, rcT.left, rcT.top, RCWIDTH(rcT), RCHEIGHT(rcT), TRUE );
	}
	if( (hWndCtrl = ::GetDlgItem( m_hWnd, IDC_NCT_EDIT )) ) {
		rcT.left   = rcC.left;
		rcT.right  = rcC.right - (m_rcClient.right-m_rcEdit.right);
		rcT.top    = rcC.bottom - (m_rcClient.bottom-m_rcEdit.top);
		rcT.bottom = rcC.bottom - (m_rcClient.bottom-m_rcEdit.bottom);
		::MoveWindow( hWndCtrl, rcT.left, rcT.top, RCWIDTH(rcT), RCHEIGHT(rcT), TRUE );
	}
	if( (hWndCtrl = ::GetDlgItem( m_hWnd, IDC_NCT_SEND )) ) {
		rcT.left   = rcC.right - (m_rcClient.right-m_rcButton.left);
		rcT.right  = rcT.left + RCWIDTH(m_rcButton);
		rcT.top    = rcC.bottom - (m_rcClient.bottom-m_rcButton.top);
		rcT.bottom = rcT.top + RCHEIGHT(m_rcButton);
		::MoveWindow( hWndCtrl, rcT.left, rcT.top, RCWIDTH(rcT), RCHEIGHT(rcT), TRUE );
	}
	return	FALSE;
}

DLGMSG	CChatDlg::OnControlColorStatic( DLGMSGPARAM )
{
	if( (HWND)lParam == ::GetDlgItem( m_hWnd, IDC_NCT_MESSAGE ) ) {
		SetBkColor( (HDC)wParam, (COLORREF)0x00FFFFFF );
		bResult = (BOOL)(LRESULT)GetStockObject( WHITE_BRUSH );
		return	TRUE;
	}
	return	FALSE;
}

DLGCMD	CChatDlg::OnMessageFocus( DLGCMDPARAM )
{
	DEBUGOUT( "CChatDlg::OnMessageFocus\n" );

//	::SetFocus( ::GetDlgItem( m_hWnd, IDC_NCT_EDIT ) );
	::SetFocus( m_hWnd );
}

DLGCMD	CChatDlg::OnOK( DLGCMDPARAM )
{
//	DEBUGOUT( "CChatDlg::OnOK\n" );
	SetEditText();
}

DLGCMD	CChatDlg::OnCancel( DLGCMDPARAM )
{
//	DEBUGOUT( "CChatDlg::OnCancel\n" );
	::ShowWindow( m_hWnd, SW_HIDE ); // #### Hide
}

DLGCMD	CChatDlg::OnSend( DLGCMDPARAM )
{
//	DEBUGOUT( "CChatDlg::OnSend\n" );
	SetEditText();

	::SetFocus( ::GetDlgItem( m_hWnd, IDC_NCT_EDIT ) );
}

