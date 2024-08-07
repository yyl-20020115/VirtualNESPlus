//
// #### Window Class
//
// #### Without inheritance, there is no meaning.
//
#ifndef	__CWND_INCLUDED__
#define	__CWND_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>

#include <string>
#include <list>
using namespace std;

// prototype
class	CWnd;

// #### A class for processing modeless and other irrelevant data
class	CWndList
{
public:
	CWndList();
	~CWndList();

	static	void	Add( CWnd* pWnd );	// #### add to a list
	static	void	Del( CWnd* pWnd );	// #### Delete from the list

	static	BOOL	IsDialogMessage( LPMSG msg );
protected:
private:
	static	list<CWnd*>	m_WndPtrList;
};

// #### normal window
class	CWnd
{
public:
	CWnd();
	virtual	~CWnd();

	HWND	m_hWnd;
	HMENU	m_hMenu;

	// Override
	virtual	BOOL	Create( HWND hWndParent ) { return FALSE; };
	virtual	void	Destroy() {};

	// Override for ModalDialog
	virtual	INT_PTR	DoModal( HWND hWndParent ) { return 0; }

	// For message filtering
	virtual	BOOL	PreTranslateMessage( MSG* pMsg ) { return FALSE; }
protected:
	void	SetThis();

	// Static
	static	LRESULT	CALLBACK g_WndProc(  HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	static	INT_PTR	CALLBACK g_DlgProc(  HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

	// Override
	virtual	LRESULT	DispatchWnd( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{ return ::DefWindowProc(  hWnd, msg, wParam, lParam ); }

	virtual	BOOL	DispatchDlg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
	{ return FALSE; }
private:
};

// #### message
// #### Class Definition Macro
#define	WND_MESSAGE_MAP()	LRESULT DispatchWnd( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define	WNDMSG			BOOL
#define	WNDCMD			void
#define	WNDNOTIFY		void

#define	WNDMSGPARAM		HWND hWnd, WPARAM wParam, LPARAM lParam, LRESULT& lResult
#define	WNDCMDPARAM		HWND hWnd, UINT uID
#define	WNDNOTIFYPARAM		HWND hWnd, UINT uID, NMHDR* pNMHDR, LRESULT& lResult

// #### Macro Implementation
#define	WND_MESSAGE_BEGIN(cls)	\
	LRESULT cls::DispatchWnd( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {\
		LRESULT	lResult = 0L;\
		switch( msg ) {
#define	WND_ON_MESSAGE(msg,fnc)	\
	case (msg): {\
		if( fnc( hWnd, wParam, lParam, lResult ) )\
			return	lResult;\
	}\
	break;
// #### commands
#define	WND_COMMAND_BEGIN() \
	case	WM_COMMAND: {
#define	WND_ON_COMMAND(id,fnc) \
	if( LOWORD(wParam) == (id) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
#define	WND_ON_COMMAND_RANGE(id,idend,fnc) \
	if( LOWORD(wParam) >= (id) && LOWORD(wParam) <= (idend) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
// Notify commands
#define	WND_ON_COMMAND_NOTIFY(id,nfy,fnc) \
	if( LOWORD(wParam) == (id) && HIWORD(wParam) == (nfy) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
#define	WND_ON_COMMAND_NOTIFY_RANGE(id,idend,nfy,fnc) \
	if( LOWORD(wParam) >= (id) && LOWORD(wParam) <= (idend) && HIWORD(wParam) == (nfy) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
#define	WND_COMMAND_END() \
	return	0L; }; break;

// #### WM_NOTIFY is a Windows message that is used to notify windows of changes in the user interface. The NOTIFY_BEGIN() and NOTIFY_END() macros are used to define a range of messages that are sent when a window is being created, deleted, or modified. These macros can be used in a dialog box's resource script to detect these events and perform any necessary actions.  In English, the translation of WM_NOTIFY and its related macros would be:  WM_NOTIFY: This is a Windows message that alerts windows about changes in the user interface.  NOTIFY_BEGIN(): This macro defines a range of messages that are sent when a window is being created, deleted, or modified.  NOTIFY_END(): This macro marks the end of the range of messages defined by NOTIFY_BEGIN().
#define	WND_NOTIFY_BEGIN() \
	case	WM_NOTIFY: { \
		LRESULT	lResult = 0L;
#define	WND_ON_NOTIFY(id,msg,fnc) \
	if( (UINT)wParam == (id) && ((NMHDR*)lParam)->code == (msg) ) {\
		fnc( ((NMHDR*)lParam)->hwndFrom, (UINT)wParam, (NMHDR*)lParam, lResult );\
		return	(BOOL)lResult; }
#define	WND_ON_NOTIFY_RANGE(id,idend,msg,fnc) \
	if( (UINT)wParam >= (id) && (UINT)wParam <= (idend) && ((NMHDR*)lParam)->code == (msg) ) {\
		fnc( ((NMHDR*)lParam)->hwndFrom, (UINT)wParam, (NMHDR*)lParam, lResult );\
		return	(BOOL)lResult; }
#define	WND_NOTIFY_END() \
	return	FALSE; }; break;

#define	WND_MESSAGE_END() \
	default: break; }\
	return	::DefWindowProc( hWnd, msg, wParam, lParam );\
	}

// #### Your words are really wonderful.
// #### Class Definition Macro
#define	DLG_MESSAGE_MAP()	BOOL DispatchDlg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );

#define	DLGMSG			BOOL
#define	DLGCMD			void
#define	DLGNOTIFY		void

#define	DLGMSGPARAM		HWND hWnd, WPARAM wParam, LPARAM lParam, BOOL& bResult
#define	DLGCMDPARAM		HWND hWnd, UINT uID
#define	DLGNOTIFYPARAM		HWND hWnd, UINT uID, NMHDR* pNMHDR, LRESULT& lResult

// #### macro
#define	DLG_MESSAGE_BEGIN(cls)	\
	BOOL cls::DispatchDlg( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam ) {\
		BOOL	bResult = TRUE;\
		switch( msg ) {
#define	DLG_ON_MESSAGE(msg,fnc)	\
	case (msg): {\
		if( fnc( hWnd, wParam, lParam, bResult ) )\
			return	bResult;\
	}\
	break;
// #### The `COMMAND_BEGIN()` and `COMMAND_END()` macros 
#define	DLG_COMMAND_BEGIN() \
	case	WM_COMMAND: {
#define	DLG_ON_COMMAND(id,fnc) \
	if( LOWORD(wParam) == (id) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
#define	DLG_ON_COMMAND_RANGE(id,idend,fnc) \
	if( LOWORD(wParam) >= (id) && LOWORD(wParam) <= (idend) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
// Notify commands
#define	DLG_ON_COMMAND_NOTIFY(id,nfy,fnc) \
	if( LOWORD(wParam) == (id) && HIWORD(wParam) == (nfy) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
#define	DLG_ON_COMMAND_NOTIFY_RANGE(id,idend,nfy,fnc) \
	if( LOWORD(wParam) >= (id) && LOWORD(wParam) <= (idend) && HIWORD(wParam) == (nfy) ) \
		fnc( (HWND)lParam, LOWORD(wParam) );
#define	DLG_COMMAND_END() \
	return	FALSE; }; break;

// #### The `WM_NOTIFY` message is used to notify the application of a window event. The `WM_NOTIFY` message is sent by the Windows operating system when a change occurs in the state of a window or its child windows.  The `NOTIFY_BEGIN()` and `NOTIFY_END()` macros are used to define the range of messages that will be processed by the application. These macros are typically used to indicate the start and end points of a notification sequence, such as when a window is created, deleted, or modified.  Here is an example of how you might use `NOTIFY_BEGIN()` and `NOTIFY_END()` in your code: ``` #include <windows.h>  // Declare a macro to define the start point of the notification sequence #define NOTIFY_BEGIN() 0x0100  // Declare a macro to define the end point of the notification sequence #define NOTIFY_END() 0x0200  LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {     // Process the message     switch (msg) {         case WM_NOTIFY:             // Check if the notification is within the defined range             if (wParam >= NOTIFY_BEGIN() && wParam <= NOTIFY_END()) {                 // Process the notification                 // ...             }             break;     }     return 0; } ``` In this example, the `NOTIFY_BEGIN()` macro is defined as `0x0100` and the `NOTIFY_END()` macro is defined as `0x0200`. These values are used to define the range of messages that will be processed by the application.  When a notification message is received with a value within the defined range, the application will process the notification. In this case, the application will check if the notification is within the defined range and then process it.  It's important to note that the `WM_NOTIFY` message can have any value within the defined range, so you may want to use a more specific condition to determine whether the notification should be processed.
#define	DLG_NOTIFY_BEGIN() \
	case	WM_NOTIFY: { \
		LRESULT	lResult = 0L;
#define	DLG_ON_NOTIFY(id,msg,fnc) \
	if( (UINT)wParam == (id) && ((NMHDR*)lParam)->code == (msg) ) {\
		fnc( ((NMHDR*)lParam)->hwndFrom, (UINT)wParam, (NMHDR*)lParam, lResult );\
		return	(BOOL)lResult; }
#define	DLG_ON_NOTIFY_RANGE(id,idend,msg,fnc) \
	if( (UINT)wParam >= (id) && (UINT)wParam <= (idend) && ((NMHDR*)lParam)->code == (msg) ) {\
		fnc( ((NMHDR*)lParam)->hwndFrom, (UINT)wParam, (NMHDR*)lParam, lResult );\
		return	(BOOL)lResult; }
#define	DLG_NOTIFY_END() \
	return	FALSE; }; break;

#define	DLG_MESSAGE_END() \
	default: break; }\
	return	FALSE;\
	}

//
// #### Dialog Control Macro
//
#define	BTNCHECK(ctrlid,check)	::CheckDlgButton(m_hWnd,(ctrlid),(check)?BST_CHECKED:BST_UNCHECKED)
#define	IsBTNCHECK(ctrlid)	((::IsDlgButtonChecked(m_hWnd,(ctrlid))==BST_CHECKED)?TRUE:FALSE)

#define	CTRLENABLE(ctrlid,enable) ::EnableWindow(::GetDlgItem(m_hWnd,(ctrlid)),enable);

#endif	// !__CWND_INCLUDED__
