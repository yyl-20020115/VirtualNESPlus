//
// #### Data Card Barcode Dialog Class.
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commctrl.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include <list>
#include <vector>
#include <string>
using namespace std;

#include "typedef.h"
#include "macro.h"

#include "VirtualNESres.h"
#include "DebugOut.h"
#include "App.h"
#include "Pathlib.h"
#include "Config.h"

#include "Wnd.h"
#include "DatachBarcodeDlg.h"

#include "EmuThread.h"
#include "nes.h"
#include "mapper.h"


BOOL	CDatachBarcodeDlg::Create( HWND hWndParent )
{
	// #### Parent is sitting at the desktop.
	m_hWnd = ::CreateDialogParam( CApp::GetPlugin(), MAKEINTRESOURCE(IDD_EXT_BARCODEBATTLER),
				NULL, g_DlgProc, (LPARAM)this );
	if( !m_hWnd )
		return	FALSE;

	// #### When you add a modal dialog list to a modeless dialog list, the effect is remarkable.
	CWndList::Add( this );

	return	TRUE;
}

void	CDatachBarcodeDlg::Destroy()
{
	if( m_hWnd ) {
		// #### position saved
		::GetWindowRect( m_hWnd, &Config.general.rcBarcodePos );

		// #### Remove the modal dialog list from the scene.
		CWndList::Del( this );
		::DestroyWindow( m_hWnd );
		m_hWnd = NULL;
	}
}

DLGMSG	CDatachBarcodeDlg::OnInitDialog( DLGMSGPARAM )
{
	DEBUGOUT( "CDatachBarcodeDlg::OnInitDialog\n" );

	// #### Position correction
	if( Config.general.rcBarcodePos.right-Config.general.rcBarcodePos.left != 0
	 && Config.general.rcBarcodePos.bottom-Config.general.rcBarcodePos.top != 0 ) {
		::SetWindowPos( m_hWnd, HWND_NOTOPMOST, Config.general.rcBarcodePos.left, Config.general.rcBarcodePos.top,
				0, 0, SWP_NOSIZE | SWP_NOZORDER );
	}

	::SendDlgItemMessage( m_hWnd, IDC_EBB_CODE, EM_SETLIMITTEXT, 13, 0 );
	CTRLENABLE( IDC_EBB_TRANSFER, FALSE );

	::srand( (unsigned)::time( NULL ) );

	return	TRUE;
}

DLGMSG	CDatachBarcodeDlg::OnActivate( DLGMSGPARAM )
{
	DEBUGOUT( "CDatachBarcodeDlg::OnActivate\n" );

	if( LOWORD(wParam) == WA_INACTIVE ) {
		DEBUGOUT( "CDatachBarcodeDlg::OnActivate:Inactive\n" );
		::PostMessage( CApp::GetHWnd(), WM_VNS_SHORTCUTENABLE, (WPARAM)TRUE, 0 );
	} else {
		DEBUGOUT( "CDatachBarcodeDlg::OnActivate:Active\n" );
		::PostMessage( CApp::GetHWnd(), WM_VNS_SHORTCUTENABLE, (WPARAM)FALSE, 0 );
	}

	return	FALSE;
}

DLGMSG	CDatachBarcodeDlg::OnDestroy( DLGMSGPARAM )
{
	return	TRUE;
}

DLGMSG	CDatachBarcodeDlg::OnClose( DLGMSGPARAM )
{
	DEBUGOUT( "CDatachBarcodeDlg::OnClose\n" );
	::ShowWindow( m_hWnd, SW_HIDE ); // #### hidden
	return	TRUE;
}

DLGCMD	CDatachBarcodeDlg::OnCancel( DLGCMDPARAM )
{
	DEBUGOUT( "CDatachBarcodeDlg::OnCancel\n" );
	::ShowWindow( m_hWnd, SW_HIDE ); // #### Just hide it.
}

DLGCMD	CDatachBarcodeDlg::OnCodeinput( DLGCMDPARAM )
{
	DEBUGOUT( "CDatachBarcodeDlg::OnCodeinput\n" );

        LRESULT	len = ::SendDlgItemMessage( m_hWnd, IDC_EBB_CODE, WM_GETTEXTLENGTH, 0, 0 );
	CTRLENABLE( IDC_EBB_TRANSFER, ((len==8)||(len==13))?TRUE:FALSE );
}

DLGCMD	CDatachBarcodeDlg::OnCodeTransfer( DLGCMDPARAM )
{
	DEBUGOUT( "CDatachBarcodeDlg::OnCodeTransfer\n" );

	if( !Emu.IsRunning() )
		return;

	BYTE	code[14], *p;
	p = code;
	INT	len = ::GetDlgItemText( m_hWnd, IDC_EBB_CODE, (LPTSTR)code, 14 );

	Emu.EventParam2( CEmuThread::EV_BARCODE, (LONG_PTR)code, (LONG_PTR)len );
}

DLGCMD	CDatachBarcodeDlg::OnCodeCreate( DLGCMDPARAM )
{
	DEBUGOUT( "CDatachBarcodeDlg::OnCodeCreate\n" );

	BYTE	code[14];
	INT	digit, sum;

	sum = 0;
	for( INT i = 0; i < 12; i++ ) {
		digit = ::rand()%10;
		code[i] = '0'+digit;
		sum += digit*((i&1)?3:1);
	}
	code[12] = '0'+((10-(sum%10))%10);
	code[13] = '\0';

	::SetDlgItemText( m_hWnd, IDC_EBB_CODE, (LPCTSTR)code );
}

// #### Messages
DLG_MESSAGE_BEGIN(CDatachBarcodeDlg)
DLG_ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
DLG_ON_MESSAGE(WM_ACTIVATE, OnActivate)
DLG_ON_MESSAGE(WM_DESTROY, OnDestroy)
DLG_ON_MESSAGE(WM_CLOSE, OnClose)
// #### command
DLG_COMMAND_BEGIN()
DLG_ON_COMMAND(IDCANCEL, OnCancel)
//DLG_ON_COMMAND( IDC_EBB_CODE, OnCodeinput )
DLG_ON_COMMAND(IDC_EBB_TRANSFER, OnCodeTransfer)
DLG_ON_COMMAND(IDC_EBB_RANDOM, OnCodeCreate)

DLG_ON_COMMAND_NOTIFY(IDC_EBB_CODE, EN_CHANGE, OnCodeinput)

DLG_COMMAND_END()
// #### New Type
DLG_NOTIFY_BEGIN()
//DLG_ON_NOTIFY( IDC_LCH_LIST, LVN_KEYDOWN, OnKeyDownListView )
DLG_NOTIFY_END()
DLG_MESSAGE_END()
