//
// #### Emulator Thread Class
//
#ifndef	__CEMUTHREAD_INCLUDED__
#define	__CEMUTHREAD_INCLUDED__

#define	WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <string>
#include <deque>
#include <queue>
using namespace std;

#include "App.h"
#include "MMTimer.h"
#include "Config.h"
#include "WaveRec.h"

#include "typedef.h"
#include "macro.h"

#include "nes.h"
#include "mmu.h"
#include "cpu.h"
#include "ppu.h"
#include "apu.h"
#include "pad.h"
#include "rom.h"
#include "mapper.h"

// Prototypes
// NetPlay Event
class	NETEV	{
public:
	INT	Event;
	DWORD	Param;
};
//class	NES;

class	CEmuThread
{
public:
	CEmuThread();
	~CEmuThread();

	// #### event
	enum EMUEVENT {
		EV_EXIT = -1,
		EV_NONE = 0,
		EV_INITIAL,
		EV_PAUSE,
		EV_RESUME,
		// #### The following is a list of events that can be used with the `Event` component in React:  1. `onClick`: Fired when the element is clicked. 2. `onDoubleClick`: Fired when the element is double-clicked. 3. `onMouseDown`: Fired when the mouse button is pressed down over the element. 4. `onMouseUp`: Fired when the mouse button is released over the element. 5. `onMouseMove`: Fired when the mouse is moved over the element. 6. `onKeyPress`: Fired when a key is pressed while the element is in focus. 7. `onBlur`: Fired when the element loses focus. 8. `onFocus`: Fired when the element gains focus. 9. `onSubmit`: Fired when a form is submitted. 10. `onLoad`: Fired when the component is loaded. 11. `onError`: Fired when an error occurs in the component. 12. `onSuccess`: Fired when a successful response is received from an API call. 13. `onReady`: Fired when the component is ready to be interacted with.  These events can be used to perform various actions, such as submitting a form, calling an API, or updating the state of the component.
		EV_MESSAGE_OUT,		// #### Use me to output messages only.

		EV_FULLSCREEN_GDI,	// EventParam

		EV_EMUPAUSE,
		EV_ONEFRAME,
		EV_THROTTLE,
		EV_FRAMESKIP_AUTO,
		EV_FRAMESKIP_UP,
		EV_FRAMESKIP_DOWN,

		EV_HWRESET,
		EV_SWRESET,

		EV_NETPLAY_START,

		EV_STATE_LOAD,		// EventParam2
		EV_STATE_SAVE,		// EventParam2

		// For Disk system
		EV_DISK_COMMAND,	// EventParam
		// For ExController
		EV_EXCONTROLLER,	// EventParam
		// For Sound
		EV_SOUND_MUTE,		// EventParam

		// For Snapshot
		EV_SNAPSHOT,
		// For Movie
		EV_MOVIE_PLAY,		// EventParam
		EV_MOVIE_REC,		// EventParam
		EV_MOVIE_RECAPPEND,	// EventParam
		EV_MOVIE_STOP,		// EventParam

		// For Wave recording
		EV_WAVEREC_START,	// EventParam
		EV_WAVEREC_STOP,

		// For Tape recording
		EV_TAPE_PLAY,		// EventParam
		EV_TAPE_REC,		// EventParam
		EV_TAPE_STOP,

		// For Barcode
		EV_BARCODE,		// EventParam2

		// For TurboFile
		EV_TURBOFILE,		// EventParam

		// For Debugger
		EV_DEBUG_RUN,
		EV_DEBUG_BRAKE,
		EV_DEBUG_STEP,
		EV_DEBUG_COMMAND,	// EventParam
	};

	// #### Action mode
	enum {
		STATUS_NONE = 0,
		STATUS_RUN,
		STATUS_PAUSE,
	};

	// #### Debugging Command

	//
	void	SetPriority( INT nPriority );

	BOOL	Start( HWND hWnd, NES* nes );
	void	Stop();
	void	Pause();
	void	Resume();

	void	Event( EMUEVENT ev );
	void	EventParam( EMUEVENT ev, LONG_PTR Param );
	void	EventParam2( EMUEVENT ev, LONG_PTR Param, LONG_PTR Param2 );

	BOOL	IsRunning()	{ return (g_Status!=STATUS_NONE); }
	BOOL	IsPausing()	{ return (g_Status==STATUS_PAUSE); }

	BOOL	IsWaveRecord()	{ return g_WaveRec.IsWaveRecord(); }

	INT	GetDiskNo()	{ if( g_nes ) return g_nes->rom->GetDiskNo(); else return 0; }
	INT	GetExController() { if( g_nes ) return g_nes->pad->GetExController(); else return 0; }

	//
	NES*	GetNES()	{ return g_nes; }

protected:
	static	void	DiskCommand( BYTE cmd );
	static	BOOL	FrameInput();

	static	DWORD WINAPI	ThreadProc( LPVOID lpVoid );
	// #### Sound Stream
	static	void	StreamProcess( BOOL bPause );

	// #### thread ID
		HANDLE	m_hThread;
		DWORD	m_dwThreadID;

	// #### pointer
	static	CEmuThread*	g_pThis;
	// #### grip
	static	HWND	g_hWnd;
	// #### emulator object pointer
	static	NES*	g_nes;

	// #### Wave Recorder
	static	CWaveRec g_WaveRec;

	// #### pose counting
		INT	m_nPauseCount;
	// #### state
	static	INT	g_Status;

	// #### Handler
	static	INT	g_Event;
	static	LONG_PTR	g_EventParam;
	static	LONG_PTR	g_EventParam2;
	static	HANDLE	g_hEvent;
	static	HANDLE	g_hEventAccept;

	// #### thread priority
		INT	m_nPriority;
	static	INT	g_PriorityTable[];

	// #### Message of error
	static	CHAR	g_szErrorMessage[512];

	// #### String Table
	static	LPCSTR	g_lpSoundMuteStringTable[];

	// NetPlay Event
	static	deque<NETEV>	NetEventQueue;
	static	string		strNetStateName;

private:
};

extern	CEmuThread	Emu;

#endif	// !__CEMUTHREAD_INCLUDED__

