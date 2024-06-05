#include "CHyperLink.h"

CHyperLink::CHyperLink() 
	: m_hWnd(NULL), m_lpLabel(NULL),
	m_hCursor(NULL), m_hFont(NULL),
	m_lpHyperLink(NULL), m_lpfnOldWndProc(NULL)
{
}

CHyperLink::~CHyperLink()
{
	Detach();
}

// #### attach to control

BOOL CHyperLink::Attach(HWND hWnd, LPCSTR lpLabel, LPCSTR lpHyperLink)
{
	if (m_hWnd)
		return	FALSE;	// #### Already attached.

	m_hWnd = hWnd;

	// #### Copy the text
	if (lpLabel) {
		m_lpLabel = new CHAR[::lstrlen(lpLabel) + 1];
		::lstrcpy(m_lpLabel, lpLabel);
		::SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM)m_lpLabel);
	}
	// #### If the label and hyperlink are the same, then the hyperlink is equal to the label.
	if (lpLabel && !lpHyperLink) {
		m_lpHyperLink = new CHAR[::lstrlen(lpLabel) + 1];
		::lstrcpy(m_lpHyperLink, lpLabel);
	}
	// #### Hyperlink may be different in another case.
	if (lpHyperLink) {
		m_lpHyperLink = new CHAR[::lstrlen(lpHyperLink) + 1];
		::lstrcpy(m_lpHyperLink, lpHyperLink);
	}

	// #### Pointers
#ifndef	IDC_HAND
#define	IDC_HAND	MAKEINTRESOURCE(32649)
#endif
	m_hCursor = ::LoadCursor(NULL, IDC_HAND);

	// #### Underline font creation
	HFONT	m_hOldFont = (HFONT)::SendMessage(hWnd, WM_GETFONT, 0, 0);
	LOGFONT	lFont;
	::GetObject(m_hOldFont, sizeof(LOGFONT), &lFont);
	lFont.lfUnderline = TRUE;
	m_hFont = ::CreateFontIndirect(&lFont);

	// #### Calculate the display area of labels.
	CalcLabelRectangle();

	// #### Style change (notifying enabled)
	DWORD	dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
	dwStyle |= SS_NOTIFY;
	::SetWindowLong(hWnd, GWL_STYLE, (LONG)dwStyle);

	// #### topSpeed
	m_lpfnOldWndProc = (WNDPROC)::SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)HyperLinkProc);

	// #### To bury/insert this.
	::SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)this);

	return	TRUE;
}

BOOL CHyperLink::Detach()
{
	if (m_hWnd) {
		// #### subclassification
		if (m_lpfnOldWndProc) {
			::SetWindowLongPtr(m_hWnd, GWLP_WNDPROC, (LONG_PTR)m_lpfnOldWndProc);
			m_lpfnOldWndProc = NULL;
		}

		// #### Font to delete
		if (m_hFont) {
			::DeleteObject((HGDIOBJ)m_hFont);
			m_hFont = NULL;
		}

		// #### Remove character string
		if (m_lpLabel) {
			delete[] m_lpLabel;
			m_lpLabel = NULL;
		}
		if (m_lpHyperLink) {
			delete[] m_lpHyperLink;
			m_lpHyperLink = NULL;
		}
	}

	m_hWnd = NULL;

	return	TRUE;
}

BOOL CHyperLink::SetLabel(LPCSTR lpLabel)
{
	if (m_lpLabel) {
		delete[] m_lpLabel;
	}

	m_lpLabel = new CHAR[::lstrlen(lpLabel) + 1];
	::lstrcpy(m_lpLabel, lpLabel);

	// #### Set a string in the control
	::SendMessage(m_hWnd, WM_SETTEXT, 0, (LPARAM)m_lpLabel);

	// #### Compute the display area of a label.
	CalcLabelRectangle();
	return TRUE;
}

BOOL CHyperLink::SetHyperLink(LPCSTR lpHyperLink)
{
	if (m_lpHyperLink) {
		delete[] m_lpHyperLink;
	}

	m_lpHyperLink = new CHAR[::lstrlen(lpHyperLink) + 1];
	::lstrcpy(m_lpHyperLink, lpHyperLink);
	return TRUE;
}

BOOL CHyperLink::CalcLabelRectangle()
{
	if (!::IsWindow(m_hWnd))
		return	FALSE;

	if (!m_lpLabel)
		return	FALSE;

	RECT	rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_rcLabel = rcClient;

	HDC	hDC = ::GetDC(m_hWnd);
	HFONT	hOldFont = (HFONT)::SelectObject(hDC, m_hFont);

	// #### appearance
	DWORD	dwStyle = ::GetWindowLong(m_hWnd, GWL_STYLE);
	INT	nDrawStyle = DT_LEFT;
	if (dwStyle && SS_CENTER) {
		nDrawStyle = DT_CENTER;
	}
	else if (dwStyle && SS_RIGHT) {
		nDrawStyle = DT_RIGHT;
	}

	// #### Computational Graphic Display of Text and Image Lists
	::DrawText(hDC, m_lpLabel, -1, &m_rcLabel, nDrawStyle | DT_WORDBREAK | DT_CALCRECT);

	::SelectObject(hDC, hOldFont);

	// #### calculate the distance or space based on the style.
	if (dwStyle & SS_CENTER) {
		::OffsetRect(&m_rcLabel, (rcClient.right - m_rcLabel.right) / 2, 0);
	}
	else if (dwStyle & SS_RIGHT) {
		::OffsetRect(&m_rcLabel, rcClient.right - m_rcLabel.right, 0);
	}

	return true;
}

LRESULT CHyperLink::HyperLinkProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	CHyperLink* pHyperLink = reinterpret_cast<CHyperLink*>(
		::GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (msg) {
	case	WM_DESTROY:
	{
		// #### Fix... after release on October 11, 2003.
		WNDPROC	pWndProcOld = pHyperLink->m_lpfnOldWndProc;
		pHyperLink->Detach();
		return	CallWindowProc(pWndProcOld, hWnd, msg, wParam, lParam);
	}
	break;
	case	WM_LBUTTONDOWN: {
		POINT	pt = { LOWORD(lParam), HIWORD(lParam) };
		if (pHyperLink->m_lpLabel && pHyperLink->m_lpHyperLink) {
			if (::PtInRect(&pHyperLink->m_rcLabel, pt)) {
				::ShellExecute(hWnd, NULL, pHyperLink->m_lpHyperLink, NULL, NULL, SW_SHOWNORMAL);
				return	TRUE;
			}
		}
	}
						  break;
	case	WM_MOUSEMOVE: {
		POINT	pt = { LOWORD(lParam), HIWORD(lParam) };
		if (pHyperLink->m_lpLabel && pHyperLink->m_lpHyperLink) {
			if (pHyperLink->m_hCursor && ::PtInRect(&pHyperLink->m_rcLabel, pt)) {
				::SetCursor(pHyperLink->m_hCursor);
				return	TRUE;
			}
		}
	}
						break;
	case	WM_PAINT: {
		PAINTSTRUCT	ps;
		HDC	hDC = ::BeginPaint(hWnd, &ps);
		DWORD	dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
		INT	nDrawStyle = DT_LEFT;
		if (dwStyle && SS_CENTER) {
			nDrawStyle = DT_CENTER;
		}
		else if (dwStyle && SS_RIGHT) {
			nDrawStyle = DT_RIGHT;
		}

		// #### Assimilation
		::SetBkMode(hDC, TRANSPARENT);
		::SetTextColor(hDC, 0x00FF0000);

		// #### Displaying text
		if (pHyperLink->m_lpLabel) {
			HFONT	hOldFont = (HFONT)::SelectObject(hDC, pHyperLink->m_hFont);
			::DrawText(hDC, pHyperLink->m_lpLabel, -1, &pHyperLink->m_rcLabel, nDrawStyle | DT_WORDBREAK);
			::SelectObject(hDC, hOldFont);
		}

		::EndPaint(hWnd, &ps);
	}
					return	TRUE;
	default:
		break;
	}

	return	CallWindowProc(pHyperLink->m_lpfnOldWndProc, hWnd, msg, wParam, lParam);
}
