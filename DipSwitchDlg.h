//
// #### dialog class
//
#ifndef	__CDIPSWITCHDLG_INCLUDED__
#define	__CDIPSWITCHDLG_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
using namespace std;

#include "Wnd.h"

class	CDipSwitchDlg : public CWnd
{
public:
	// Override from CWnd
 	virtual INT_PTR	DoModal( HWND hWndParent ) override;

protected:
	// Message map
	DLG_MESSAGE_MAP()
	DLGMSG		OnInitDialog( DLGMSGPARAM );

	DLGCMD		OnOK( DLGCMDPARAM );
	DLGCMD		OnCancel( DLGCMDPARAM );
	//

	INT	m_nDispCount;

private:
};

#endif	// !__CDIPSWITCHDLG_INCLUDED__

