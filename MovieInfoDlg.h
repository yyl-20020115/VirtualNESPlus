//
// #### Movie Information Dialog Class
//
#ifndef	__CMOVIEINFODLG_INCLUDED__
#define	__CMOVIEINFODLG_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <string>
using namespace std;

#include "Wnd.h"

class	CMovieInfoDlg : public CWnd
{
public:
	CMovieInfoDlg();
public:
	// Override from CWnd
	virtual INT_PTR	DoModal(HWND hWndParent) override;

	WORD	m_wRecVersion;
	WORD	m_wVersion;
	DWORD	m_dwFrames;
	DWORD	m_dwRerecordTimes;

protected:
	// Message map
	DLG_MESSAGE_MAP()
		DLGMSG		OnInitDialog(DLGMSGPARAM);
	DLGCMD		OnOK(DLGCMDPARAM);
	DLGCMD		OnCancel(DLGCMDPARAM);
	//
private:
};

#endif	// !__CMOVIEINFODLG_INCLUDED__

