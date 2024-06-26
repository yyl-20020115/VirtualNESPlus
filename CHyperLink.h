#ifndef	__CHYPERLINK_INCLUDED__
#define	__CHYPERLINK_INCLUDED__
#include <Windows.h>
//
// #### By subdividing the class of statistical control into smaller subcategories (sub-classification), we can provide better support for hyperlinks between related documents or web pages.
//
// Usage: CHyperLink	hyperlink;
// hyperlink.Attach( GetDlgItem( hWnd, IDC_URL ) );
//
class	CHyperLink
{
public:
	CHyperLink();
	~CHyperLink();

	// #### attach to control
	BOOL Attach( HWND hWnd, LPCSTR lpLabel = NULL, LPCSTR lpHyperLink = NULL );

	BOOL	Detach();

	BOOL	SetLabel( LPCSTR lpLabel );

	BOOL	SetHyperLink( LPCSTR lpHyperLink );

protected:
	BOOL	CalcLabelRectangle();

	static	LRESULT CALLBACK HyperLinkProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	HWND	m_hWnd;
	HFONT	m_hFont;
	HFONT	m_hOldFont;
	HCURSOR	m_hCursor;
	WNDPROC	m_lpfnOldWndProc;

	RECT	m_rcLabel;

	LPSTR	m_lpLabel;
	LPSTR	m_lpHyperLink;
private:
};

#endif	// !__CHYPERLINK_INCLUDED__
