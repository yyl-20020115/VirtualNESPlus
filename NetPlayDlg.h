//
// #### Network Play Dialog Class
//
#ifndef	__CNETPLAYDLG_INCLUDED__
#define	__CNETPLAYDLG_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <mmsystem.h>

#include <string>
using namespace std;

#include "Wnd.h"

class	CNetPlayDlg : public CWnd
{
public:
	CNetPlayDlg();
public:
	// Override from CWnd
	virtual INT_PTR	DoModal(HWND hWndParent) override;

protected:
	void	SetStatusMessage(UINT uID);

	void	SetControlEnable(BOOL bEnable);

	void	AddRecentPort(LPCSTR lpszPort);
	void	AddRecentHost(LPCSTR lpszPort);

	// Message map
	DLG_MESSAGE_MAP()
	DLGMSG		OnInitDialog(DLGMSGPARAM);
	DLGMSG		OnNetwordHostByName(DLGMSGPARAM);
	DLGMSG		OnNetworkAccept(DLGMSGPARAM);
	DLGMSG		OnNetworkConnect(DLGMSGPARAM);
	DLGMSG		OnNetworkClose(DLGMSGPARAM);
	DLGMSG		OnNetworkError(DLGMSGPARAM);

	DLGCMD		OnServer(DLGCMDPARAM);
	DLGCMD		OnClient(DLGCMDPARAM);

	DLGCMD		OnConnect(DLGCMDPARAM);
	DLGCMD		OnCancel(DLGCMDPARAM);
	//

	HWND	m_hWndParent;
	UINT	m_uPort;
	LONG_PTR	m_nNetLatency;
private:
};

#endif	// !__CNETPLAYDLG_INCLUDED__

