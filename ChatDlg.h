//
// #### Chat Dialogue Class
//
#ifndef	__CCHATDLG_INCLUDED__
#define	__CCHATDLG_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
using namespace std;

#include "Wnd.h"

class	CChatDlg : public CWnd
{
public:
	// Override from CWnd
	BOOL	Create( HWND hWndParent );
	void	Destroy();

	// Set Message
	void	SetEditText();
protected:
	// Message map
	DLG_MESSAGE_MAP()
	DLGMSG		OnInitDialog( DLGMSGPARAM );
	DLGMSG		OnDestroy( DLGMSGPARAM );
	DLGMSG		OnClose( DLGMSGPARAM );
	DLGMSG		OnActivate( DLGMSGPARAM );
	DLGMSG		OnSetCursor( DLGMSGPARAM );
	DLGMSG		OnSize( DLGMSGPARAM );

	DLGMSG		OnControlColorStatic( DLGMSGPARAM );

	DLGMSG		OnCopyData( DLGMSGPARAM );

	DLGCMD		OnMessageFocus( DLGCMDPARAM );

	DLGCMD		OnOK( DLGCMDPARAM );
	DLGCMD		OnCancel( DLGCMDPARAM );
	DLGCMD		OnSend( DLGCMDPARAM );
	//

	// #### Position
	RECT		m_rcClient;	// #### client area
	RECT		m_rcMessage;	// #### Message Frame
	RECT		m_rcEdit;	// #### Editing Frame
	RECT		m_rcButton;	// #### Send button

private:
};

#endif	// !__CCHATDLG_INCLUDED__

