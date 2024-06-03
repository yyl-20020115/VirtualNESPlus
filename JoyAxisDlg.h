//
// ジョイスティック軸設定ダイアログクラス
//
#ifndef	__CJOYAXISDLG_INCLUDED__
#define	__CJOYAXISDLG_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
using namespace std;

#include "Wnd.h"
#include "Config.h"

class	CJoyAxisDlg : public CWnd
{
public:
	// Override from CWnd
	INT_PTR	DoModal( HWND hWndParent );

protected:
	void	OnJoySettingSetup( INT nID );

	// Message map
	DLG_MESSAGE_MAP()
	DLGMSG		OnInitDialog( DLGMSGPARAM );

	DLGMSG		OnTimer( DLGMSGPARAM );
	DLGNOTIFY	OnCheckChange( DLGCMDPARAM );
	DLGNOTIFY	OnSelectChange( DLGCMDPARAM );

	DLGCMD		OnOK( DLGCMDPARAM );
	DLGCMD		OnCancel( DLGCMDPARAM );
	DLGCMD		OnDefault( DLGCMDPARAM );
	//

	LONG_PTR	m_JoySel;
	UINT_PTR	m_TimerID;
	WORD	m_JoyAxisSetting[16];
private:
};

#endif	// !__CJOYAXISDLG_INCLUDED__

