//
// バージョンダイアログクラス
//
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <string>
using namespace std;

#include "VirtualNESres.h"
#include "DebugOut.h"
#include "App.h"
#include "Pathlib.h"

#include "Wnd.h"
#include "AboutDlg.h"

DLG_MESSAGE_BEGIN(CAboutDlg)
DLG_ON_MESSAGE( WM_INITDIALOG,	OnInitDialog )

DLG_COMMAND_BEGIN()
DLG_ON_COMMAND( IDOK, OnOK )
DLG_ON_COMMAND( IDCANCEL, OnCancel )
DLG_ON_COMMAND( IDC_VER_WEBSITE, OnWebsite )
DLG_COMMAND_END()
DLG_MESSAGE_END()

INT_PTR	CAboutDlg::DoModal( HWND hWndParent )
{
	return	::DialogBoxParam( CApp::GetPlugin(), MAKEINTRESOURCE(IDD_VERSION),
				hWndParent, g_DlgProc, (LPARAM)this );
}

DLGMSG	CAboutDlg::OnInitDialog( DLGMSGPARAM )
{
//	DEBUGOUT( "CAboutDlg::OnInitDialog\n" );

	::SendDlgItemMessage( m_hWnd, IDC_VER_ICON, STM_SETICON,
			      (WPARAM)CApp::LoadIcon( IDI_ICON ), 0 );

	TCHAR	str[256];

	::wsprintf( str, "VirtualNES version %01d.%01d%01d%s",
		    (VIRTUALNES_VERSION&0xF00)>>8,
		    (VIRTUALNES_VERSION&0x0F0)>>4,
		    (VIRTUALNES_VERSION&0x00F),
		    VIRTUALNES_FIXVERSION );

	::SetDlgItemText( m_hWnd, IDC_VER_VERSION, str );

//	::SetDlgItemText( m_hWnd, IDC_VER_WEBSITE, VIRTUALNES_WEBSITE );

	m_Website.Attach( ::GetDlgItem( m_hWnd, IDC_VER_WEBSITE ), VIRTUALNES_WEBSITE );

	string	email, mailto;
	email  = VIRTUALNES_EMAILNAME;
	email += "@";
	email += VIRTUALNES_EMAILDOMAIN;
	mailto = "mailto:" + email;

	m_Email.Attach( ::GetDlgItem( m_hWnd, IDC_VER_EMAIL ), email.c_str(), mailto.c_str() );

	return	TRUE;
}

DLGCMD	CAboutDlg::OnOK( DLGCMDPARAM )
{
//	DEBUGOUT( "CAboutDlg::OnOK\n" );

//	m_Website.Detach();
//	m_Email.Detach();

	::EndDialog( m_hWnd, IDOK );
}

DLGCMD	CAboutDlg::OnCancel( DLGCMDPARAM )
{
//	DEBUGOUT( "CAboutDlg::OnCancel\n" );

//	m_Website.Detach();
//	m_Email.Detach();

	::EndDialog( m_hWnd, IDCANCEL );
}

DLGCMD	CAboutDlg::OnWebsite( DLGCMDPARAM )
{
//	DEBUGOUT( "CAboutDlg::OnCancel\n" );

	::ShellExecute( HWND_DESKTOP, "open", VIRTUALNES_WEBSITE, NULL, NULL, SW_SHOWNORMAL );
//	::ShellExecute( m_hWnd, "open", VIRTUALNES_WEBSITE, NULL, NULL, SW_SHOWNORMAL );
}

