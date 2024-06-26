//
// #### Language Dialogue Class.
//
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
using namespace std;

#include "VirtualNESres.h"
#include "DebugOut.h"
#include "App.h"
#include "Pathlib.h"

#include "Plugin.h"

#include "Wnd.h"
#include "LanguageDlg.h"


CLanguageDlg::CLanguageDlg()
	:m_nPluginID()
{

}

INT_PTR	CLanguageDlg::DoModal( HWND hWndParent )
{
	return	::DialogBoxParam( CApp::GetPlugin(), MAKEINTRESOURCE(IDD_CFG_LANGUAGE),
				hWndParent, g_DlgProc, (LPARAM)this );
}

DLGMSG	CLanguageDlg::OnInitDialog( DLGMSGPARAM )
{
//	DEBUGOUT( "CLanguageDlg::OnInitDialog\n" );

	m_nPluginID = CPlugin::GetPluginID();

	for( INT i = 0; i < CPlugin::GetPluginNum(); i++ ) {
		::SendDlgItemMessage( m_hWnd, IDC_LNG_LIST, LB_INSERTSTRING, (WPARAM)i, (LPARAM)CPlugin::GetPluginLanguage(i) );
	}
	// #### Default selection
	::SendDlgItemMessage( m_hWnd, IDC_LNG_LIST, LB_SETCURSEL, (WPARAM)m_nPluginID, 0 );

	return	TRUE;
}

DLGCMD	CLanguageDlg::OnOK( DLGCMDPARAM )
{
//	DEBUGOUT( "CLanguageDlg::OnOK\n" );
	LRESULT nID = ::SendDlgItemMessage( m_hWnd, IDC_LNG_LIST, LB_GETCURSEL, 0, 0 );
	if( m_nPluginID != nID ) {
		CPlugin::SetPluginID((INT) nID );
		::EndDialog( m_hWnd, IDOK );
	} else {
		::EndDialog( m_hWnd, IDCANCEL );
	}
}

DLGCMD	CLanguageDlg::OnCancel( DLGCMDPARAM )
{
//	DEBUGOUT( "CLanguageDlg::OnCancel\n" );
	::EndDialog( m_hWnd, IDCANCEL );
}

DLGCMD	CLanguageDlg::OnDefault( DLGCMDPARAM )
{
//	DEBUGOUT( "CLanguageDlg::OnDefault\n" );

	// #### Default selection
	SendDlgItemMessage( m_hWnd, IDC_LNG_LIST, LB_SETCURSEL, (WPARAM)CPlugin::GetPluginID(), 0 );
}

DLG_MESSAGE_BEGIN(CLanguageDlg)
// #### # Message
DLG_ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
// #### Command
DLG_COMMAND_BEGIN()
DLG_ON_COMMAND(IDOK, OnOK)
DLG_ON_COMMAND(IDCANCEL, OnCancel)
DLG_ON_COMMAND(IDDEFAULT, OnDefault)
DLG_COMMAND_END()
DLG_MESSAGE_END()

