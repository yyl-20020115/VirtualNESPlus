//
// #### Message Filtering Support Class
//
#ifndef	__CWNDHOOK_INCLUDED__
#define	__CWNDHOOK_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// prototypes
class	CWnd;

class	CWndHook
{
public:
	static	void	Initialize();
	static	void	Release();

	static	void	SetFiltering( BOOL bMode ) { m_bMsgFiltering = bMode; }

	static	BOOL	WalkPreTranslateTree( MSG* lpMsg );

protected:
	static	LRESULT	CALLBACK MessageFilterProc( INT code, WPARAM wParam, LPARAM lParam );

	// #### The flag to filter messages or not
	static	BOOL	m_bMsgFiltering;

	// #### The message is being filtered.
	static	BOOL	m_bMsgFilter;
	// #### original hook procedure,
	static	HHOOK 	m_hOldMsgFilter;
private:
};

#endif	// !__CWNDHOOK_INCLUDED__

