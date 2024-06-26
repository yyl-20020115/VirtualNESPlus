//
// #### Online Class
//
#ifndef	__CNETPLAY_INCLUDED__
#define	__CNETPLAY_INCLUDED__

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <mmsystem.h>
#include <winsock.h>

#include "macro.h"
#include "typedef.h"

#include <string>
using namespace std;

#define	WM_NETPLAY		(WM_APP+100)

#define	WM_NETPLAY_HOSTBYNAME	(WM_APP+101)

#define	WM_NETPLAY_ACCEPT	(WM_APP+110)
#define	WM_NETPLAY_CONNECT	(WM_APP+111)
#define	WM_NETPLAY_CLOSE	(WM_APP+112)
#define	WM_NETPLAY_ERROR	(WM_APP+113)

// for BSD style
#ifndef	INVALID_SOCKET
#define	INVALID_SOCKET	-1
#endif

// for Winsock1.x
#ifndef	SD_RECEIVE
#define	SD_RECEIVE	0
#endif
#ifndef	SD_SEND
#define	SD_SEND		1
#endif
#ifndef	SD_BOTH
#define	SD_BOTH		2
#endif

class	CNetPlay
{
public:
	CNetPlay();
	~CNetPlay();

	// #### opening.
	BOOL	Initialize( HWND hWnd );
	void	Release();

	// #### Are you online?
	BOOL	IsNetPlay() { return m_hWnd?TRUE:FALSE; }
	// #### Underway
	BOOL	IsConnect() { return m_hWnd?m_bConnect:FALSE; }
	// #### In progress
	BOOL	IsServer() { return m_bServer; }

	// #### Communication latency
	void	SetLatency( INT nLatency ) { m_nLatency = nLatency; }
	INT	GetLatency() { return m_nLatency; }

	// #### Setting up a non- asynchronous message window for sending messages.  
	void	SetMsgWnd( HWND hWnd ) { m_hWndMsg = hWnd; }

	// #### Chat Message Receiving Window Settings
	void	SetChatWnd( HWND hWnd ) { m_hWndChat = hWnd; }

	// #### Check if the hostname is an IP address (0:IP -:Error +:Host search)
	INT	ASyncHostCheck( HWND hWnd, const char* lpszHost );
	HRESULT	ASyncWndProc( HWND hWnd, WPARAM wParam, LPARAM lParam );

	// #### connectivity and disconnection
	BOOL	Connect( BOOL bServer, const char* lpszIP, unsigned short Port );
	void	Disconnect();

	// #### Data transmission
	INT	Send( BYTE data );
	// #### data reception
	// #### Waiting for data reception
	// #### Timed out without
	INT	Recv( BYTE& data );
	// #### Time out available
	INT	RecvTime( BYTE& data, unsigned long timeout );

	// #### Incorporation into the Ring Buffer
	BOOL	RecvBuffer();
	// #### -
	INT	BufferCheck();
	// #### obtaining the number of buffer bytes required
	INT	GetRecvBufferSize();

	// #### synchronization
	INT	Sync();
	// #### Player state update
	INT	ModifyPlayer( LPBYTE p1, LPBYTE p2 );

	// #### Chat message transmission
	void	ChatSend( LPCSTR lpStr );

	// #### Message Procedure for Windows
	HRESULT	WndProc( HWND hWnd, WPARAM wParam, LPARAM lParam );

	// #### Communication buffer
	enum {
		// #### Data block size
		SOCKET_BLOCK_SIZE = 8,
		// #### Buffa size
		SOCKET_BUFFER_SIZE = (SOCKET_BLOCK_SIZE*32),
		// #### Buffer Size for Receiving
		SOCKET_RECEIVE_SIZE = (SOCKET_BLOCK_SIZE*8)
	};
protected:
	// #### Member variable
	HWND	m_hWnd;
	HWND	m_hWndMsg;
	HWND	m_hWndASync;
	HWND	m_hWndChat;

	HANDLE	m_hASyncTask;
	CHAR	m_HostEntry[MAXGETHOSTSTRUCT];

	BOOL	m_bServer;
	BOOL	m_bConnect;	// #### linked
	INT	m_nLatency;	// #### buffer size
	INT	m_nFrameStep;	// #### Communication frame rate
	INT	m_nFrameCount;	// #### counter

	// Ring buffer
	INT	m_nRingPtr;
	INT	m_nSendPtr;
	INT	m_nRecvPtr;
	INT	m_nRecvSize;
	BYTE	m_SendBuffer[SOCKET_BUFFER_SIZE];
	BYTE	m_RecvBuffer[SOCKET_BUFFER_SIZE];

	// WINSOCK
	WSADATA	m_WSAdata;
	SOCKET	m_SocketConnect;
	SOCKET	m_SocketData;
	SOCKET	m_SocketChat;

	struct sockaddr_in	m_SAddr_Server;
	struct sockaddr_in	m_SAddr_Client;
private:
};

extern	CNetPlay	NetPlay;

#endif	// !__CNETPLAY_INCLUDED__
