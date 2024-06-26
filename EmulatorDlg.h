//
// #### Emulation Dialog Class
//
#ifndef	__CEMULATORDLG_INCLUDED__
#define	__CEMULATORDLG_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
using namespace std;

#include "Wnd.h"
#include "Config.h"

class	CEmulatorDlg : public CWnd
{
public:
	CEmulatorDlg();
public:
	// Override from CWnd
	virtual INT_PTR	DoModal(HWND hWndParent) override;
protected:
	void	OnInitialMember();

	// Message map
	DLG_MESSAGE_MAP()
	DLGMSG		OnInitDialog( DLGMSGPARAM );
	DLGMSG		OnHScroll( DLGMSGPARAM );
	DLGCMD		OnOK( DLGCMDPARAM );
	DLGCMD		OnCancel( DLGCMDPARAM );
	DLGCMD		OnDefault( DLGCMDPARAM );
	//

	CCfgEmulator	m_ConfigSave;

private:
};

#endif	// !__CEMULATORDLG_INCLUDED__

