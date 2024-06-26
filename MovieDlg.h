//
// #### Movie Dialogue Class
//
#ifndef	__CMOVIEDLG_INCLUDED__
#define	__CMOVIEDLG_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
using namespace std;

#include "Wnd.h"
#include "Config.h"

class	CMovieDlg : public CWnd
{
public:
	CMovieDlg();
public:
	// Override from CWnd
	virtual INT_PTR	DoModal(HWND hWndParent) override;

protected:
	// Message map
	DLG_MESSAGE_MAP()
	DLGMSG	OnInitDialog( DLGMSGPARAM );
	DLGCMD	OnOK( DLGCMDPARAM );
	DLGCMD	OnCancel( DLGCMDPARAM );
	//

	CCfgMovie	m_ConfigSave;
private:
};

#endif	// !__CMOVIEDLG_INCLUDED__

