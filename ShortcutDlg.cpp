//
// #### Shortcut Dialog Class
//
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>

#include <string>
using namespace std;

#include "VirtualNESres.h"
#include "DebugOut.h"
#include "App.h"
#include "Pathlib.h"
#include "Config.h"

#include "Wnd.h"
#include "ShortcutDlg.h"

#include "DirectInput.h"

DLG_MESSAGE_BEGIN(CShortcutDlg)
// #### Message
DLG_ON_MESSAGE(WM_INITDIALOG, OnInitDialog)
DLG_ON_MESSAGE(WM_TIMER, OnTimer)
// #### Notification message.
DLG_NOTIFY_BEGIN()
DLG_ON_NOTIFY(IDC_CUT_LIST, NM_DBLCLK, OnListDblClick)
DLG_ON_NOTIFY(IDC_CUT_LIST, LVN_ITEMCHANGED, OnListItemChanged)
DLG_NOTIFY_END()
// #### Order
DLG_COMMAND_BEGIN()
DLG_ON_COMMAND(IDOK, OnOK)
DLG_ON_COMMAND(IDCANCEL, OnCancel)
DLG_ON_COMMAND(IDDEFAULT, OnDefault)
DLG_COMMAND_END()
DLG_MESSAGE_END()

DLGMSG		OnTimer(DLGMSGPARAM);
DLGNOTIFY	OnListDblClick(DLGNOTIFYPARAM);
DLGNOTIFY	OnListItemChanged(DLGNOTIFYPARAM);

CShortcutDlg::CShortcutDlg()
	: m_szWindowTitle()
	, m_szMessage()
	, m_bCancelMode()
	, m_SelectPos()
	, m_SelectID()
	, m_SelectSubID()
	, m_TimerID()
	, m_TimerCount()
	, m_ControlBuf()
	, m_ControlBuf2()
	, m_ConfigSave()
	, m_cBossKeyLetter('Z')
	, m_cBossKeyLetterOriginal('Z')
	, m_hWndParent()

{

}

INT_PTR	CShortcutDlg::DoModal(HWND hWndParent)
{
	m_bCancelMode = FALSE;
	return	(INT)::DialogBoxParam(CApp::GetPlugin(), MAKEINTRESOURCE(IDD_CFG_SHORTCUT),
		m_hWndParent = hWndParent, g_DlgProc, (LPARAM)this);
}

BOOL	CShortcutDlg::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_CHAR && pMsg->hwnd ==::GetDlgItem(m_hWnd,IDC_CUT_BOSS)) {
		CHAR c = toupper((int)pMsg->wParam);
		::SendMessageA(pMsg->hwnd, pMsg->message, c, pMsg->lParam);
		::SendMessageA(pMsg->hwnd, EM_SETSEL, 0, -1);
		return TRUE;
	}
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		//		DEBUGOUT( "CShortcutDlg::PreTranslateMessage WM_KEY????\n" );
		return	m_bCancelMode;
	}
	
	return	FALSE;
}

void CShortcutDlg::SetBossKeyChar(CHAR BossKeyChar)
{
	this->m_cBossKeyLetterOriginal
		= this->m_cBossKeyLetter;
	this->m_cBossKeyLetter  = BossKeyChar;
}

CHAR CShortcutDlg::GetBossKeyChar()
{
	return this->m_cBossKeyLetter;
}
CHAR CShortcutDlg::GetBossKeyCharOriginal() {
	return this->m_cBossKeyLetterOriginal;
}

void	CShortcutDlg::OnInitialMember()
{
	HWND hWndEdit = ::GetDlgItem(m_hWnd, IDC_CUT_BOSS);
	CHAR buffer[2] = { m_cBossKeyLetter,0 };
	buffer[0] = toupper(buffer[0]);
	::SendMessageA(hWndEdit, EM_SETLIMITTEXT, 1,0);
	::SetWindowTextA(hWndEdit, buffer);

	// #### List View
	CHAR	szStr[256];
	HWND	hWndCtrl = ::GetDlgItem(m_hWnd, IDC_CUT_LIST);
	ListView_DeleteColumn(hWndCtrl, 0);
	ListView_DeleteColumn(hWndCtrl, 0);
	ListView_DeleteColumn(hWndCtrl, 0);
	ListView_DeleteAllItems(hWndCtrl);
	ListView_SetItemCount(hWndCtrl, 100);

	// #### Head control setting
	LVCOLUMN	lvColumn = { 0 };
	lvColumn.mask = LVCF_FMT | LVCF_TEXT | LVCF_SUBITEM;
	lvColumn.fmt = LVCFMT_LEFT;

	CApp::LoadString(IDS_CUT_TYPE, szStr, sizeof(szStr));
	lvColumn.iSubItem = 0;
	lvColumn.pszText = szStr;
	ListView_InsertColumn(hWndCtrl, 0, &lvColumn);

	CApp::LoadString(IDS_CUT_KEY, szStr, sizeof(szStr));
	lvColumn.iSubItem = 1;
	lvColumn.pszText = szStr;
	ListView_InsertColumn(hWndCtrl, 1, &lvColumn);

	CApp::LoadString(IDS_CUT_KEY2, szStr, sizeof(szStr));
	lvColumn.iSubItem = 2;
	lvColumn.pszText = szStr;
	ListView_InsertColumn(hWndCtrl, 2, &lvColumn);


	// #### List View Content Setting
	LVITEM	lvItem;
	ZeroMemory(&lvItem, sizeof(lvItem));
	lvItem.mask = LVIF_TEXT;

	for (INT i = 0; CConfig::ShortcutKeyID[i * 3 + 0]; i++) {
		lvItem.iItem = i;
		// #### species
		CApp::LoadString(CConfig::ShortcutKeyID[i * 3 + 1], szStr, sizeof(szStr));
		lvItem.pszText = szStr;
		ListView_InsertItem(hWndCtrl, &lvItem);

		// #### Settings
		if (m_ControlBuf[i] == 0) {
			ListView_SetItemText(hWndCtrl, i, 1, "----");
		}
		else {
			string	str = Config.ShortcutToKeyName(m_ControlBuf[i]);
			ListView_SetItemText(hWndCtrl, i, 1, (LPSTR)str.c_str());
		}
		// #### Setting 2
		if (m_ControlBuf2[i] == 0) {
			ListView_SetItemText(hWndCtrl, i, 2, "----");
		}
		else {
			string	str = Config.ShortcutToKeyName(m_ControlBuf2[i]);
			ListView_SetItemText(hWndCtrl, i, 2, (LPSTR)str.c_str());
		}
	}

	// #### ...because if I don't set the column width before changing the horizontal scrollbar, it will interfere with the display...
	RECT	rc;
	::GetClientRect(hWndCtrl, &rc);

	ListView_SetColumnWidth(hWndCtrl, 0, (rc.right - rc.left) * 10 / 20);
	ListView_SetColumnWidth(hWndCtrl, 1, (rc.right - rc.left) * 5 / 20);
	ListView_SetColumnWidth(hWndCtrl, 2, (rc.right - rc.left) * 5 / 20);
	ListView_SetExtendedListViewStyle(hWndCtrl, LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES);
}

DLGMSG	CShortcutDlg::OnInitDialog(DLGMSGPARAM)
{
	//	DEBUGOUT( "CShortcutDlg::OnInitDialog\n" );

		// #### Setting Save
	m_ConfigSave = Config.shortcut;
	this->SetBossKeyChar(Config.shortcut.nBossKey);
	// #### copy
	for (INT i = 0; CConfig::ShortcutKeyID[i * 3 + 0]; i++) {
		m_ControlBuf[i] = Config.shortcut.nShortCut[CConfig::ShortcutKeyID[i * 3 + 2]];
		m_ControlBuf2[i] = Config.shortcut.nShortCut[CConfig::ShortcutKeyID[i * 3 + 2] + 128];
	}
	// #### Title Save
	::GetWindowText(m_hWnd, m_szWindowTitle, sizeof(m_szWindowTitle));
	// #### Save settings message
	CApp::LoadString(IDS_CUT_TITLE, m_szMessage, sizeof(m_szMessage));

	// #### Initialization
	OnInitialMember();

	m_bCancelMode = FALSE;
	m_SelectPos = -1;
	m_TimerID = 0;
	m_SelectID = 0;

	return	TRUE;
}

DLGMSG	CShortcutDlg::OnTimer(DLGMSGPARAM)
{
	if (wParam == (WPARAM)m_TimerID) {
		m_TimerCount++;

		DirectInput.Poll();

		if (!(m_TimerCount & 0x04)) {
			string	str = m_szWindowTitle;
			str = str + "    ";
			str = str + m_szMessage;
			::SetWindowText(m_hWnd, str.c_str());
		}
		else {
			::SetWindowText(m_hWnd, m_szWindowTitle);
		}

		if (m_TimerCount > 60) {
			// #### time out
			if (m_TimerID) {
				::KillTimer(m_hWnd, m_TimerID);
				m_TimerID = 0;
				m_SelectID = 0;
			}
			m_bCancelMode = FALSE;
			::SetWindowText(m_hWnd, m_szWindowTitle);
		}
		else {
			// Button push?
			HWND	hWndCtrl = ::GetDlgItem(m_hWnd, IDC_CUT_LIST);
			INT	i;
			// Set cancel
			if (DirectInput.m_Sw[DIK_ESCAPE] && !(DirectInput.m_Sw[DIK_LSHIFT] || DirectInput.m_Sw[DIK_RSHIFT])) {
				if (m_SelectSubID == 2) {
					ListView_SetItemText(hWndCtrl, m_SelectID, 2, "----");
					m_ControlBuf2[m_SelectID] = 0;
				}
				else {
					ListView_SetItemText(hWndCtrl, m_SelectID, 1, "----");
					m_ControlBuf[m_SelectID] = 0;
				}
				//				::SetWindowText( hWndCtrl, "----" );
				if (m_TimerID) {
					::KillTimer(m_hWnd, m_TimerID);
					m_TimerID = 0;
					m_SelectID = 0;
				}
				m_bCancelMode = FALSE;
				::SetWindowText(m_hWnd, m_szWindowTitle);
			}
			else {
				string	str;
				WORD	keyex = 0;
				if (!DirectInput.m_Sw[DIK_ESCAPE]) {
					if (DirectInput.m_Sw[DIK_LMENU] || DirectInput.m_Sw[DIK_RMENU]) {
						keyex |= CCfgShortCut::K_ALT;
						str = str + "Alt+";
					}
					if (DirectInput.m_Sw[DIK_LCONTROL] || DirectInput.m_Sw[DIK_RCONTROL]) {
						keyex |= CCfgShortCut::K_CTRL;
						str = str + "Ctrl+";
					}
					if (DirectInput.m_Sw[DIK_LSHIFT] || DirectInput.m_Sw[DIK_RSHIFT]) {
						keyex |= CCfgShortCut::K_SHIFT;
						str = str + "Shift+";
					}
				}

				for (i = 0; i < 256 + 64 * 8; i++) {
					if (i == DIK_LMENU || i == DIK_RMENU
						|| i == DIK_LCONTROL || i == DIK_RCONTROL
						|| i == DIK_LSHIFT || i == DIK_RSHIFT)
						continue;

					if (DirectInput.m_Sw[i] & 0x80) {
						if (DirectInput.SearchKeyName(i)) {
							str = str + DirectInput.SearchKeyName(i);
							if (m_SelectSubID == 2) {
								m_ControlBuf2[m_SelectID] = keyex + i;
								ListView_SetItemText(hWndCtrl, m_SelectID, 2, (LPSTR)str.c_str());
							}
							else {
								m_ControlBuf[m_SelectID] = keyex + i;
								ListView_SetItemText(hWndCtrl, m_SelectID, 1, (LPSTR)str.c_str());
							}

							if (m_TimerID) {
								::KillTimer(m_hWnd, m_TimerID);
								m_TimerID = 0;
								m_SelectID = 0;
							}
							m_bCancelMode = FALSE;
							::SetWindowText(m_hWnd, m_szWindowTitle);
							break;
						}
					}
				}
			}
		}
	}
	return	FALSE;
}

DLGNOTIFY CShortcutDlg::OnListItemChanged(DLGNOTIFYPARAM)
{
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;

	if (!(pNMListView->uOldState & LVIS_SELECTED) && (pNMListView->uNewState & LVIS_SELECTED)) {
		m_SelectPos = pNMListView->iItem;
	}
}

DLGNOTIFY CShortcutDlg::OnListDblClick(DLGNOTIFYPARAM)
{
	NMLISTVIEW* pNMListView = (NMLISTVIEW*)pNMHDR;

	//	DEBUGOUT( "CShortcutDlg::OnListDblClick\n" );

		// #### Not selected yet.
	if (m_SelectPos == -1)
		return;

	LVHITTESTINFO	htInfo;
	ZeroMemory(&htInfo, sizeof(htInfo));
	//	DWORD	pos = ::GetMessagePos();
	//	POINT	pt = *((POINT*)&pos);
	//	::ScreenToClient( ::GetDlgItem( m_hWnd, IDC_CUT_LIST ), &pt );
	//	htInfo.pt.x = pt.x;
	//	htInfo.pt.y = pt.y;
	htInfo.pt.x = pNMListView->ptAction.x;
	htInfo.pt.y = pNMListView->ptAction.y;
	ListView_HitTest(::GetDlgItem(m_hWnd, IDC_CUT_LIST), &htInfo);

	// #### Hit
	if (htInfo.flags & LVHT_NOWHERE) {
		return;
	}

	if (!m_TimerID && pNMListView->iSubItem) {
		m_bCancelMode = TRUE;
		m_SelectID = m_SelectPos;
		m_SelectSubID = pNMListView->iSubItem;
		m_TimerCount = 0;
		m_TimerID = (INT)::SetTimer(m_hWnd, 1, 50, NULL);
		DEBUGOUT("ID=%d/SUBID=%d\n", pNMListView->iItem, pNMListView->iSubItem);
	}
}

DLGCMD	CShortcutDlg::OnOK(DLGCMDPARAM)
{
	//	DEBUGOUT( "CShortcutDlg::OnOK\n" );

	if (m_TimerID) {
		::KillTimer(m_hWnd, m_TimerID);
		m_TimerID = 0;
		m_SelectID = 0;
		m_bCancelMode = FALSE;
	}

	// #### Copy setting.
	for (INT i = 0; CConfig::ShortcutKeyID[i * 3 + 0]; i++) {
		Config.shortcut.nShortCut[CConfig::ShortcutKeyID[i * 3 + 2]] = m_ControlBuf[i];
		Config.shortcut.nShortCut[CConfig::ShortcutKeyID[i * 3 + 2] + 128] = m_ControlBuf2[i];
	}
	char buffer[2] = { 0 };
	HWND hEdit = ::GetDlgItem(m_hWnd, IDC_CUT_BOSS);
	if (hEdit != 0) {
		::GetWindowTextA(hEdit, buffer, sizeof(buffer));
		this->SetBossKeyChar(buffer[0]);
		if (buffer[0] != this->GetBossKeyCharOriginal()) {
			::UnregisterHotKey(m_hWndParent, VIRTUAL_NES_BOSS_KEY_ID);
			::RegisterHotKey(m_hWndParent,
				VIRTUAL_NES_BOSS_KEY_ID,
				VIRTUAL_NES_BOSS_KEY_MODS, this->GetBossKeyChar());
		}
		Config.shortcut.nBossKey = this->GetBossKeyChar();
	}
	::EndDialog(m_hWnd, IDOK);
}

DLGCMD	CShortcutDlg::OnCancel(DLGCMDPARAM)
{
	//	DEBUGOUT( "CShortcutDlg::OnCancel\n" );

	if (m_TimerID) {
		::KillTimer(m_hWnd, m_TimerID);
		m_TimerID = 0;
		m_SelectID = 0;
		m_bCancelMode = FALSE;
	}

	// #### Set up
	Config.shortcut = m_ConfigSave;

	::EndDialog(m_hWnd, IDCANCEL);
}

DLGCMD	CShortcutDlg::OnDefault(DLGCMDPARAM)
{
	//	DEBUGOUT( "CShortcutDlg::OnDefault\n" );

	if (m_TimerID) {
		::KillTimer(m_hWnd, m_TimerID);
		m_TimerID = 0;
		m_SelectID = 0;
		m_bCancelMode = FALSE;
	}

	Config.shortcut.Default();
	this->SetBossKeyChar(Config.shortcut.nBossKey);
	// #### copy
	for (INT i = 0; CConfig::ShortcutKeyID[i * 3 + 0]; i++) {
		m_ControlBuf[i] = Config.shortcut.nShortCut[CConfig::ShortcutKeyID[i * 3 + 2]];
		m_ControlBuf2[i] = Config.shortcut.nShortCut[CConfig::ShortcutKeyID[i * 3 + 2] + 128];
	}
	OnInitialMember();
}

