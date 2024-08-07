//
// #### Main Window Class
//
//
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <process.h>
#include <shellapi.h>
#include <commdlg.h>
#include <imm.h>
#include <string>
using namespace std;

#include "VirtualNESres.h"
#include "DebugOut.h"
#include "App.h"
#include "Pathlib.h"
#include "Plugin.h"

#include "Wnd.h"
#include "WndHook.h"
#include "MainFrame.h"
#include "Recent.h"
#include "Config.h"

#include "NetPlay.h"

#include "AboutDlg.h"
#include "EmulatorDlg.h"
#include "GraphicsDlg.h"
#include "SoundDlg.h"
#include "ControllerDlg.h"
#include "ShortcutDlg.h"
#include "FolderDlg.h"
#include "LanguageDlg.h"
#include "MovieDlg.h"
#include "GameOptionDlg.h"
#include "JoyAxisDlg.h"
#include "RomInfoDlg.h"
#include "NetPlayDlg.h"
#include "MovieInfoDlg.h"
#include "AviConvDlg.h"

#include "DirectDraw.h"
#include "DirectSound.h"
#include "DirectInput.h"

#include "nes.h"
#include "rom.h"
#include "romdb.h"
#include "pad.h"

void Log(const char* fn, const char* message) {
	FILE* f = fopen(fn, "a");
	if (f != 0) {
		fprintf(f, "%s\n", message);
		fclose(f);
	}
}
std::wstring ToWideString(const std::string& str)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);
	std::wstring wstrTo(size_needed, 0);
	MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);
	return wstrTo;
}



HRESULT CreateProgramShortcut(const wchar_t* szPath, const wchar_t* szLink, const wchar_t* description = 0)
{
	HRESULT hres = CoInitialize(0);
	if (FAILED(hres)) return hres;
	IShellLinkW* psl = 0;
	hres = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&psl);
	if (FAILED(hres)) { CoUninitialize(); return hres; }

	psl->SetPath(szPath);

	if (description != 0) {
		psl->SetDescription(description);
	}

	std::wstring _szPath = szPath;
	size_t _sp = _szPath.find_last_of('\\');
	if (_sp != _szPath.npos) {
		_szPath = _szPath.substr(0, _sp);
	}

	psl->SetWorkingDirectory(_szPath.c_str());

	IPersistFile* ppf = 0;
	hres = psl->QueryInterface(IID_IPersistFile, (void**)&ppf);
	if (FAILED(hres)) { CoUninitialize(); return hres; }

	//(*.lnk)
	hres = ppf->Save(szLink, TRUE);

	ppf->Release();
	psl->Release();
	CoUninitialize();
	return hres;
}


int CheckPath(const std::wstring& strPath)
{
	struct _stat infos = { 0 };

	if (_wstat(strPath.c_str(), &infos) != 0)
	{
		return -1;//Invalid
	}
	else if ((infos.st_mode & S_IFDIR) == S_IFDIR)
	{
		return 0;//Directory
	}
	else if ((infos.st_mode & S_IFREG) == S_IFREG)
	{
		return 1;//File
	}
	else
	{
		return -1;
	}
}



BOOL	CMainFrame::m_bKeyEnable = FALSE;
volatile BOOL	CMainFrame::m_bKeyThreadExit = FALSE;

DWORD	CMainFrame::m_dwKeyTime = 0;
BYTE	CMainFrame::m_KeyBuf[256 + 64 * CDirectInput::DIJOYSTICK_MAX];
BYTE	CMainFrame::m_KeyOld[256 + 64 * CDirectInput::DIJOYSTICK_MAX];
BYTE	CMainFrame::m_KeyCnt[256 * 64 * CDirectInput::DIJOYSTICK_MAX];

CMainFrame::CMainFrame()
	: CWnd()
	, Caption(VIRTUALNES_CAPTION)
	, m_szCommandLine()
	, m_bHasBossKey()
	, m_bActivate()
	, m_bActivateApp()
	, m_bForcus()
	, m_bEnable()
	, m_bCursor()
	, m_uTimerID()
	, m_LastMovedTime()
	, m_bMenu()
	, m_bZoomed()
	, m_WindowRect()
	, m_WindowPlacement()
	, m_ExStyleBackup()
	, m_StyleBackup()
	, m_uKeyTimerID()
	, m_bKeyChecking()
	, Nes()
	, m_nStateSlot()
	, m_LauncherDlg()
	, m_SearchDlg()
	, m_PatternView()
	, m_NameTableView()
	, m_PaletteView()
	, m_MemoryView()
	, m_PaletteEdit()
	, m_DatachBarcodeDlg()
	, m_ChatDlg()
{
#ifdef _WIN64
	this->Caption += " X64";
#else
	this->Caption += " X86";
#endif
}

BOOL	CMainFrame::Create(HWND hWndParent)
{
	// #### NES Object
	Nes = NULL;

	// #### Member variable
	m_hMenu = NULL;

	WNDCLASSEX	wcl;
	ZEROMEMORY(&wcl, sizeof(wcl));
	wcl.cbSize = sizeof(wcl);
	wcl.lpszClassName = "VirtualNESwndclass";
	wcl.lpfnWndProc = g_WndProc;
	//	wcl.style		= CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
	wcl.style = CS_DBLCLKS;
	wcl.cbClsExtra = wcl.cbWndExtra = 0;
	wcl.hInstance = CApp::GetInstance();
	wcl.lpszMenuName = NULL;
	wcl.hIcon =
		wcl.hIconSm = ::LoadIcon(CApp::GetInstance(), MAKEINTRESOURCE(IDI_ICON));
	wcl.hCursor = ::LoadCursor(NULL, IDC_ARROW);
	wcl.hbrBackground = 0;

	if (!RegisterClassEx(&wcl)) {
		//		DEBUGOUT( "RegisterClassEx faild.\n" );
		return	FALSE;
	}

	HWND hWnd = CreateWindowEx(
		WS_EX_CLIENTEDGE,
		VIRTUALNES_WNDCLASS,
		this->Caption.c_str(),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		hWndParent,
		NULL,
		CApp::GetInstance(),
		(LPVOID)this		// #### To conceal this...
	);
	if (!hWnd) {
		//		DEBUGOUT( "CreateWindow faild.\n" );
		return	FALSE;
	}

	// #### Main Window
	CApp::SetHWnd(hWnd);

	// #### status
	m_nStateSlot = 0;

	return	TRUE;
}

void	CMainFrame::Destroy()
{
	if (m_hWnd && IsWindow(m_hWnd)) {
		::DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

BOOL	CMainFrame::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST) {
		//		return	TRUE;
	}
	return	FALSE;
}

void CMainFrame::SetHasBossKey(BOOL hasBossKey)
{
	this->m_bHasBossKey = hasBossKey;
}

BOOL CMainFrame::GetHasBossKey()
{
	return this->m_bHasBossKey;
}

HRESULT MakeDesktopLink(const std::wstring& name = L"Virtual NES Plus.lnk") {
	HRESULT hr = E_FAIL;
	wchar_t path[MAX_PATH] = { 0 };
	SHGetFolderPathW(0, CSIDL_DESKTOPDIRECTORY, 0, 0, path);
	std::wstring link = path;
	link += L"\\";
	link += name;
	if (CheckPath(link) == -1) {
		wchar_t buffer[_MAX_PATH] = { 0 };
		GetModuleFileNameW(0, buffer, _MAX_PATH);
		hr = CreateProgramShortcut(buffer, link.c_str());
	}
	return hr;
}

WNDMSG	CMainFrame::OnCreate(WNDMSGPARAM)
{
	DirectDraw.SetSystemMemory(Config.graphics.bSystemMemory);
	DirectDraw.SetUseHEL(Config.graphics.bUseHEL);

	if (!DirectDraw.InitialDDraw(m_hWnd)) {
		lResult = -1;
		return	TRUE;
	}
	if (!DirectSound.InitialDSound(m_hWnd)) {
		lResult = -1L;
		return	TRUE;
	}
	if (!DirectInput.InitialDInput(m_hWnd, CApp::GetInstance())) {
		lResult = -1L;
		return	TRUE;
	}
	// #### Direct Drawing
	DirectDraw.SetFlipMode(Config.graphics.bSyncDraw);
	DirectDraw.SetAspectMode(Config.graphics.bAspect);
	DirectDraw.SetAllLineMode(Config.graphics.bAllLine);
	DirectDraw.SetMaxZoom(Config.graphics.bFitZoom);
	DirectDraw.SetTVFrameMode(Config.graphics.bTVFrame);
	DirectDraw.SetScanlineMode(Config.graphics.bScanline);
	DirectDraw.SetScanlineColor(Config.graphics.nScanlineColor);

	DirectDraw.SetWindowVSyncMode(Config.graphics.bWindowVSync);

	DirectDraw.SetDoubleSize(Config.graphics.bDoubleSize);
	DirectDraw.SetGraphicsFilter(Config.graphics.nGraphicsFilter);

	// #### DirectDraw destruction failed
	if (!DirectDraw.InitialSurface(FALSE)) {
		lResult = -1L;
		return	TRUE;
	}

	// #### Loading a paletted file.
	if (Config.graphics.bPaletteFile) {
		string	pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.graphics.szPaletteFile);
		DirectDraw.SetPaletteFile(pathstr.c_str());
	}

	// #### DS
	DirectSound.SetSamplingRate(Config.sound.nRate, Config.sound.nBits);
	DirectSound.SetBufferSize(Config.sound.nBufferSize);

	// #### Loading an Extra Sound File
	for (INT i = 0; i < ESF_FILE_MAX; i++) {
		string	pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.extsound.szExtSoundFile[i]);
		DirectSound.LoadEsf(pathstr.c_str(), i);
	}

	// #### Setting DirectInput
	DirectInput.SetJoyAxisMode(Config.general.JoyAxisSetting);

	// #### The construction of a DirectSound buffer.  In DirectX, a DirectSound buffer is a contiguous block of memory that is used to store audio data. To construct a DirectSound buffer, you will need to follow these steps:  1. Create a new memory block: Use the `CreateCommittedResource` function to create a new memory block with the `DXGI_RESOURCE_DIRECTSOUND` type. This will give you a handle to the memory block that can be used to access it in your DirectSound buffer. 2. Set the memory block's size: Use the `SetSize` function to set the size of the memory block. You will need to pass in the number of bytes required for your audio data. 3. Allocate the memory block: Use the `Allocate` function to allocate the memory block. This will give you a handle to the memory block that can be used to access it in your DirectSound buffer. 4. Fill the memory block with audio data: Use the `Fill` function to fill the memory block with your audio data. You will need to pass in the address of the memory block and the number of bytes required for your audio data. 5. Create a DirectSound buffer: Use the `CreateBuffer` function to create a new DirectSound buffer. You will need to pass in the handle to the memory block you created earlier, as well as any other relevant parameters such as the buffer size and the format of the audio data. 6. Associate the buffer with a device: Use the `AssociateBuffer` function to associate the buffer with a DirectSound device. You will need to pass in the handle to the buffer and the handle to the device you want to associate it with. 7. Start playing the buffer: Use the `Play` function to start playing the buffer. You will need to pass in the handle to the buffer and any other relevant parameters such as the rate at which you want to play the audio data.  Here is an example of how you might construct a DirectSound buffer in C++: ``` #include <d3d11.h> #include <dxgi1_6.h>  // Step 1: Create a new memory block DXGI_RESOURCE_DESC resourceDesc = {0}; resourceDesc.type = DXGI_RESOURCE_TYPE_BUFFER; resourceDesc.format = DXGI_FORMAT_PINNED_AUDIO; resourceDesc.width = 2; // samples per channel resourceDesc.height = 1; // channels per sample resourceDesc.depth = 1; // samples per channel resourceDesc.usage = DXGI_USAGE_STREAMING; DWORD *data = new DWORD[resourceDesc.width * resourceDesc.height * resourceDesc.depth]; HRESULT hr = CreateCommittedResource(&resourceDesc, data, &buffer); if (FAILED(hr)) {     // handle error }  // Step 2: Set the memory block's size DWORD size = resourceDesc.width * resourceDesc.height * resourceDesc.depth * sizeof(DWORD); SetSize(buffer, size);  // Step 3: Allocate the memory block hr = Allocate(buffer); if (FAILED(hr)) {     // handle error }  // Step 4: Fill the memory block with audio data for (int i = 0; i < resourceDesc.width * resourceDesc.height * resourceDesc.depth; i++) {     data[i] = (DWORD)i; }  // Step 5: Create a DirectSound buffer DXSOUNDBUFFER bufferDesc = {0}; bufferDesc.format = DXGI_FORMAT_PINNED_AUDIO; bufferDesc.width = resourceDesc.width; bufferDesc.height = resourceDesc.height; bufferDesc.depth = resourceDesc.depth; bufferDesc.usage = DXGI_USAGE_STREAMING; hr = CreateBuffer(buffer, &bufferDesc, NULL, &directSoundBuffer); if (FAILED(hr)) {     // handle error }  // Step 6: Associate the buffer with a device DXSOUNDDEVICE device = {0}; device.format = DXGI_FORMAT_PINNED_AUDIO; device.width = resourceDesc.width; device.height = resourceDesc.height; device.depth = resourceDesc.depth; device.usage = DXGI_USAGE_STREAMING; hr = AssociateBuffer(directSoundBuffer, &device); if (FAILED(hr)) {     // handle error }  // Step 7: Start playing the buffer DWORD samplesPlayed = 0; while (samplesPlayed < resourceDesc.width * resourceDesc.height) {     // Play the next sample     hr = Play(directSoundBuffer, NULL, 0, &samplesPlayed);     if (FAILED(hr)) {         // handle error     } } ``` Note that this is just a basic example of how to construct a DirectSound buffer in C++, and you may need to modify it to fit your specific use case. Additionally, you will need to link against the `d3d11.lib` and `dxgi1_6.lib` libraries in order to use the `CreateCommittedResource`, `SetSize`, `Allocate`, `CreateBuffer`, `AssociateBuffer`, and `Play` functions.
	if (!DirectSound.InitialBuffer()) {
		lResult = -1L;
		return	TRUE;
	}

	// #### The calculation of a Monte Carlo tree is as follows:  1. Initialize the current node as the root node. 2. Generate a random number between 0 and 1. 3. Calculate the probability of being at each child node based on the weight of the node and the randomly generated number. 4. Choose the child node with the highest probability. 5. Recursively apply steps 2-4 until a termination condition is met. 6. Output the final state of the tree as the predicted outcome.  In more detail, the calculation of a Monte Carlo tree involves the following steps:  1. Initialize the current node as the root node. This is the starting point of the tree, and it represents the entire problem domain. 2. Generate a random number between 0 and 1. This number will be used to determine the probability of being at each child node. 3. Calculate the probability of being at each child node based on the weight of the node and the randomly generated number. The weight of a node is a measure of how well the node represents the problem domain, and it is used to determine the probability of being at that node. 4. Choose the child node with the highest probability. This node will be the next node to be explored in the tree. 5. Recursively apply steps 2-4 until a termination condition is met. The termination condition is typically when a stopping criterion is reached, such as a maximum depth of the tree or a minimum number of samples required to reach a conclusion. 6. Output the final state of the tree as the predicted outcome. This output represents the solution to the problem, and it is based on the probabilities of being at each node in the tree.  By following these steps, a Monte Carlo tree can be constructed to solve a wide range of problems, including decision-making, planning, and optimization. The key advantage of Monte Carlo trees is that they can handle complex, non-linear problems with multiple possible outcomes, making them a powerful tool for solving real-world problems.
	DirectDraw.CalcPaletteTable();

	// #### online play
	NetPlay.Initialize(m_hWnd);

	// #### Menu load
	m_hMenu = CApp::LoadMenu(IDR_MENU);
	CApp::SetMenu(m_hMenu);
	::SetMenu(m_hWnd, m_hMenu);
	m_bMenu = TRUE;		// #### Menu display flag

	// #### Add Accelerator Keys to Menu
	OnRebuildMenu();

	// #### Setting the window position
	RECT	rc = Config.general.rcWindowPos;
	if (!((rc.right - rc.left) <= 0 || (rc.bottom - rc.top) <= 0)) {
		if ((m_bZoomed = Config.general.bWindowZoom)) {
			CApp::SetCmdShow(SW_SHOWMAXIMIZED);
		}
		// #### Restore window size to what it was saved as.
		m_WindowRect = rc;
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rc.left, rc.top,
			rc.right - rc.left, rc.bottom - rc.top, 0);
	}
	else {
		m_bZoomed = FALSE;
		// #### Adjust default size
		OnSetWindowSize();
	}

	// #### Save Window Style
	m_StyleBackup = ::GetWindowLong(m_hWnd, GWL_STYLE);
	m_ExStyleBackup = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);

	// #### Input Method Editors (IMEs) are not allowed: D
	::ImmAssociateContext(m_hWnd, NULL);

	// #### Authorization for Drag&Drop
	::DragAcceptFiles(m_hWnd, TRUE);

	// #### flag-type
	m_bActivate = TRUE;
	m_bActivateApp = TRUE;
	m_bForcus = TRUE;
	m_bCursor = TRUE;
	m_bEnable = TRUE;
	m_bKeyEnable = TRUE;
	m_bKeyChecking = FALSE;
	m_LastMovedTime = 0;

	m_uTimerID = ::SetTimer(m_hWnd, 0x0001, 1000, NULL);
	//	m_uKeyTimerID = ::SetTimer( m_hWnd, 0x0100, 30, NULL );	// 30ms

	m_dwKeyTime = 0;

	// Key Thread
	::memset(m_KeyBuf, 0x00, sizeof(m_KeyBuf));
	::memset(m_KeyOld, 0x00, sizeof(m_KeyBuf));
	::memset(m_KeyCnt, 0x00, sizeof(m_KeyCnt));

	::_beginthread(KeyThreadProc, 0, NULL);

	ZEROMEMORY(m_KeyBuf, sizeof(m_KeyBuf));
	ZEROMEMORY(m_KeyCnt, sizeof(m_KeyCnt));

#if	0
	// For Command Lines
	if (::strlen(CApp::GetCmdLine()) > 0) {
		LPSTR	pCmd = CApp::GetCmdLine();
		if (pCmd[0] == '"') {	// Shell execute!!
			ZEROMEMORY(m_szCommandLine, sizeof(m_szCommandLine));
			::memcpy(m_szCommandLine, pCmd + 1, ::strlen(pCmd) - 2);
			m_szCommandLine[::strlen(m_szCommandLine)] = '\0';
		}
		else {
			::strcpy(m_szCommandLine, pCmd);
		}
		::PostMessage(m_hWnd, WM_VNS_COMMANDLINE, 0, 0L);
	}
#endif
	this->SetHasBossKey(
		::RegisterHotKey(m_hWnd, VIRTUAL_NES_BOSS_KEY_ID, VIRTUAL_NES_BOSS_KEY_MODS, VIRTUAL_NES_BOSS_KEY_LETTER)
	);

	return	FALSE;
}

WNDMSG	CMainFrame::OnClose(WNDMSGPARAM)
{
	if (this->m_bHasBossKey) {
		::UnregisterHotKey(this->m_hWnd, VIRTUAL_NES_BOSS_KEY_ID);
	}
	//	DEBUGOUT( "CMainFrame::OnClose\n" );

		// #### network play
	NetPlay.Release();

	// #### Emulation ended.
	Emu.Stop();
	DELETEPTR(Nes);

	// #### Keyboard finished.
	m_bKeyThreadExit = TRUE;

	// #### The timer has ended.
	::KillTimer(m_hWnd, m_uTimerID);
	//	::KillTimer( m_hWnd, m_uKeyTimerID );
	m_uTimerID = 0;
	m_uKeyTimerID = 0;

	// #### abandoned interface
	::DestroyWindow(m_hWnd);

	// #### Search dialog ended
	m_SearchDlg.Destroy();

	// #### Lunchtime ended
	m_LauncherDlg.Destroy();

	// #### Chat ended.
	m_ChatDlg.Destroy();

	// #### Pattern View Ended
	m_PatternView.Destroy();
	m_NameTableView.Destroy();
	m_PaletteView.Destroy();
	m_MemoryView.Destroy();

	// #### Barcode Input Dialog Ended
	m_DatachBarcodeDlg.Destroy();

	// #### Palette editing has ended.
	m_PaletteEdit.Destroy();

	// #### Window position save
	Config.general.bWindowZoom = m_bZoomed;
	Config.general.rcWindowPos = m_WindowRect;

	::DragAcceptFiles(m_hWnd, FALSE);

	return	FALSE;
}

WNDMSG	CMainFrame::OnDestroy(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnDestroy\n" );

	::PostQuitMessage(0);
	return	FALSE;
}

WNDMSG	CMainFrame::OnGetMinMaxInfo(WNDMSGPARAM)
{
	MINMAXINFO* lpMMI = (MINMAXINFO*)lParam;

	// #### Minimum size only
	lpMMI->ptMinTrackSize.x = 128;
	lpMMI->ptMinTrackSize.y = 128;
	//	lpMMI->ptMaxTrackSize.x = 65535;
	//	lpMMI->ptMaxTrackSize.y = 65535;

	return	TRUE;
}

WNDMSG	CMainFrame::OnActivate(WNDMSGPARAM)
{
	//DEBUGOUT( "WA_ACTIVATE: Minimized:%s  Prev:%08X  This:%08X\n", HIWORD(wParam)?"TRUE":"FALSE", lParam, m_hWnd );

#if	1
	if (HIWORD(wParam) || (LOWORD(wParam) == WA_INACTIVE && !lParam)) {
		if (m_bActivate) {
			//		DEBUGOUT( "Inactivate.\n" );
			m_bActivate = FALSE;
			// #### Background operation during online play: do not pause
			if (!(Config.emulator.bBackground || NetPlay.IsConnect())) {
				Emu.Pause();
			}
		}
		// #### Disabling DirectInput
		DirectInput.Unacquire();
	}
	else if (!lParam) {
		//		DEBUGOUT( "Activate.\n" );
		m_bActivate = TRUE;
		// #### When operating in the background or playing online, do not rewind.
		if (!(Config.emulator.bBackground || NetPlay.IsConnect())) {
			Emu.Resume();
		}
		// ####  DirectInput works well 
		DirectInput.Acquire();
	}
#else
	if (LOWORD(wParam) == WA_INACTIVE) {
		//		DEBUGOUT( "WM_ACTIVATE: WA_INACTIVE\n" );
		////		DEBUGOUT( "CMainFrame::OnActivate:Inactive\n" );
		m_bActivate = FALSE;
		////		m_bActivate = TRUE;
	}
	else if (LOWORD(wParam) == WA_ACTIVE) {
		//		DEBUGOUT( "WM_ACTIVATE: WA_ACTIVE\n" );
		////		DEBUGOUT( "CMainFrame::OnActivate:Active\n" );
		m_bActivate = TRUE;
	}
	else if (LOWORD(wParam) == WA_CLICKACTIVE) {
		//		DEBUGOUT( "WM_ACTIVATE: WA_CLICKACTIVE\n" );
		////		DEBUGOUT( "CMainFrame::OnActivate:ClickActive\n" );
		m_bActivate = TRUE;
	}
#endif
	return	FALSE;
	}

WNDMSG	CMainFrame::OnActivateApp(WNDMSGPARAM)
{
	//DEBUGOUT( "wParam:%08X  lParam:%08X\n", wParam, lParam );

#if	0
	if ((BOOL)wParam) {
		//		DEBUGOUT( "CMainFrame::OnActivateApp:Active\n" );
		m_bActivateApp = TRUE;
		// #### Do not reduce the background activity while online.
		if (!(Config.emulator.bBackground || NetPlay.IsConnect())) {
			Emu.Resume();
		}
		DirectInput.Acquire();
	}
	else {
		//		DEBUGOUT( "CMainFrame::OnActivateApp:Inactive\n" );
		if (m_bActivateApp) {
			m_bActivateApp = FALSE;
			// #### Background operation during online play should not be paused.
			if (!(Config.emulator.bBackground || NetPlay.IsConnect())) {
				Emu.Pause();
			}
		}
		DirectInput.Unacquire();
	}
#else
	if ((BOOL)wParam) {
		//		DEBUGOUT( "CMainFrame::OnActivateApp:Active\n" );
		m_bActivateApp = TRUE;
		if (!m_bActivate) {
			m_bActivate = TRUE;
			// #### Background operation should not be turned on during net play.
			if (!(Config.emulator.bBackground || NetPlay.IsConnect())) {
				Emu.Resume();
			}
		}
		DirectInput.Acquire();
	}
	else {
		//		DEBUGOUT( "CMainFrame::OnActivateApp:Inactive\n" );
		m_bActivateApp = FALSE;
		if (m_bActivate) {
			m_bActivate = FALSE;
			// #### Do not leave the background running during net play.
			if (!(Config.emulator.bBackground || NetPlay.IsConnect())) {
				Emu.Pause();
			}
		}
		DirectInput.Unacquire();
	}
#endif
	return	FALSE;
		}

WNDMSG	CMainFrame::OnShortCutEnable(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnShortCutmode Req=%s\n", (BOOL)wParam?"TRUE":"FALSE" );

	if ((BOOL)wParam) {
		if (m_bEnable) {
			m_bKeyEnable = TRUE;
			//DEBUGOUT( "CMainFrame::OnShortCutmode TRUE\n" );
		}
	}
	else {
		m_bKeyEnable = FALSE;
		//DEBUGOUT( "CMainFrame::OnShortCutmode FALSE\n" );
	}
	//	m_bKeyEnable = (BOOL)wParam;

#if	0
	if ((BOOL)wParam) {
		if (--m_nKeyDisableCount < 0) {
			m_bKeyEnable = TRUE;
		}
	}
	else {
		m_nKeyDisableCount++;
		m_bKeyEnable = FALSE;
	}
#endif
	return	FALSE;
}

WNDMSG	CMainFrame::OnEnable(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnEnable = %s\n", (BOOL)wParam?"TRUE":"FALSE" );

	if ((BOOL)wParam)
		Emu.Resume();
	else
		Emu.Pause();

	m_bEnable = (BOOL)wParam;
	m_bKeyEnable = (BOOL)wParam;

	return	FALSE;
}

WNDMSG	CMainFrame::OnEnterMenuLoop(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnEnterMenuLoop\n" );
	m_bKeyEnable = FALSE;
	Emu.Pause();
	return	FALSE;
}

WNDMSG	CMainFrame::OnExitMenuLoop(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnExitMenuLoop\n" );
	m_bKeyEnable = TRUE;
	Emu.Resume();
	return	FALSE;
}

WNDMSG	CMainFrame::OnSetFocus(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnSetFocus\n" );
	m_bForcus = TRUE;
	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			if (!m_bMenu) {
				Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, FALSE);
			}
		}
	}
	return	FALSE;
}

WNDMSG	CMainFrame::OnKillFocus(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnKillFocus\n" );
	m_bForcus = FALSE;
	return	FALSE;
}

WNDMSG	CMainFrame::OnInitMenu(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnInitMenu\n" );
	CRecent::UpdateMenu(CApp::GetMenu());
	DrawMenuBar(m_hWnd);
	return	FALSE;
}

WNDMSG	CMainFrame::OnInitMenuPopup(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnInitMenuPopup\n" );
		// System menu
	if ((BOOL)HIWORD(lParam))
		return	FALSE;

	//	DEBUGOUT( "SubMenu=%08X uPos=%d bSys=%d\n", wParam, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam) );

	HMENU	hMenu = (HMENU)wParam;

	INT	MenuCount = ::GetMenuItemCount(hMenu);
	for (INT i = 0; i < MenuCount; i++) {
		OnUpdateMenu(hMenu, ::GetMenuItemID(hMenu, i));
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnPaint(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnPaint\n" );
	HDC	hDC;
	PAINTSTRUCT	ps;
	hDC = ::BeginPaint(m_hWnd, &ps);
	if (!Emu.IsRunning()) {
		RECT	rc;
		::GetClientRect(m_hWnd, &rc);
		::SetBkColor(hDC, 0x00000000);
		::ExtTextOut(hDC, 0, 0, ETO_OPAQUE, &rc, NULL, 0, NULL);
	}
	else if (Emu.IsPausing()) {
		DirectDraw.OnScreenDraw();
	}
	::EndPaint(m_hWnd, &ps);

	return	TRUE;
}

WNDMSG	CMainFrame::OnDisplayChange(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnDisplayChange\n" );

		// #### Change or switch to ALT + TAB while in full screen.
	if (!Config.general.bScreenMode) {
		// #### Dangerous threads require special handling.
		Emu.Pause();
		DirectDraw.OnChangeDisplayMode();

		// #### redraw
		::InvalidateRect(m_hWnd, NULL, TRUE);

		// #### thread
		Emu.Resume();
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnPaletteChanged(WNDMSGPARAM)
{
	if ((HWND)wParam == m_hWnd)
		return	FALSE;

	DirectDraw.RealizePalette();
	return	FALSE;
}

WNDMSG	CMainFrame::OnQueryNewPalette(WNDMSGPARAM)
{
	DirectDraw.RealizePalette();
	return	TRUE;
}

WNDMSG	CMainFrame::OnMenuChar(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnMenuChar\n" );
		// #### Kin-kin is annoying, so I'll pretend to be tired and close my eyes...
	lResult = MAKELONG(0, MNC_CLOSE);
	return	TRUE;
}

WNDMSG	CMainFrame::OnKeyDown(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnKeyDown\n" );

	if (Config.general.bScreenMode) {
		if (wParam == VK_ESCAPE && (lParam & (1 << 30)) == 0) {
			OnChangeMenu(!m_bMenu);
			return	TRUE;
		}
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnSize(WNDMSGPARAM)
{
	// #### maximization display equal.
	// #### It's also possible to use WM_SYSCOMMAND, but the caption will not be displayed when double-clicking on it...
	switch (wParam) {
	case	SIZE_MAXIMIZED:
		m_bZoomed = TRUE;
		{
			WINDOWPLACEMENT wpl = { 0 };
			wpl.length = sizeof(wpl);
			::GetWindowPlacement(m_hWnd, &wpl);
			m_WindowRect = wpl.rcNormalPosition;	// #### The original position to copy.
		}
		break;
	case	SIZE_RESTORED:
		m_bZoomed = FALSE;
		break;
	default:
		break;
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnSysCommand(WNDMSGPARAM)
{
	// #### in an emulator
	if (Emu.IsRunning()) {
		// #### Screen saver startup suppression
		if (wParam == SC_SCREENSAVE) {
			//			DEBUGOUT( "CMainFrame::OnSysCommand  SC_SCREENSAVE\n" );
			lResult = 1L;
			return	TRUE;
		}
		// #### Display power off suppress
		if (wParam == SC_MONITORPOWER) {
			//			DEBUGOUT( "CMainFrame::OnSysCommand  SC_MONITORPOWER\n" );
			lResult = 1L;
			return	TRUE;
		}
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnExitSizeMove(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnExitSizeMove\n" );

	if (!Config.general.bScreenMode) {
		::GetWindowRect(m_hWnd, &m_WindowRect);
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnSetCursor(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnSetCursor\n" );
	if (LOWORD(lParam) == HTCLIENT) {
		if (Emu.IsRunning()) {
			//			if( !Emu.IsPausing() && !Emu.IsEmuPausing() ) {
			if (!Emu.IsPausing()) {
				if (DirectDraw.GetZapperMode()) {
					m_bCursor = TRUE;
					::SetCursor(NULL);
					lResult = 1L;
					return	TRUE;
				}
				else {
					m_bCursor = FALSE;
					m_LastMovedTime = ::timeGetTime();
				}
			}
			else {
				m_bCursor = TRUE;
			}
		}
		else {
			m_bCursor = TRUE;
		}
	}
	else {
		m_bCursor = TRUE;
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnTimer(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnTimer\n" );
	//	DEBUGOUT( "CMainFrame::OnTimer bCursor=%s bForcus=%s\n", m_bCursor?"TRUE":"FALSE", m_bForcus?"TRUE":"FALSE" );

	if (wParam == 0x0001) {
		// #### Mouse cursor timer
		RECT	rc;
		POINT	pt;
		::GetWindowRect(m_hWnd, &rc);
		::GetCursorPos(&pt);

		if (!m_bCursor && m_bForcus && pt.x >= rc.left && pt.x <= rc.right && pt.y >= rc.top && pt.y <= rc.bottom) {
			if (Emu.IsRunning() && !Emu.IsPausing()) {
				if ((::timeGetTime() - m_LastMovedTime) > 1500) {
					::SetCursor(NULL);
				}
			}
		}
	}
	else if (wParam == 0x0100) {
		// #### Keycheck timer
			// #### When the active window is not being used or a dialogue box appears, cancel.
	////		if( m_bActivate && m_bKeyEnable )
	////		if( m_bKeyEnable )
	//			OnKeyControl();
	}

	return	FALSE;
}

WNDMSG	CMainFrame::OnCopyData(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnCopyData\n" );

	COPYDATASTRUCT* pcds = (COPYDATASTRUCT*)lParam;

	OnEmulationStart((LPSTR)pcds->lpData, FALSE);

	return	TRUE;
}

WNDMSG	CMainFrame::OnErrorMessage(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnErrorMessage\n" );

		// #### Chat ended.
	m_ChatDlg.Destroy();

	// #### Search dialog ended
	m_SearchDlg.Destroy();

	// #### Pattern View Ended
	m_PatternView.Destroy();
	m_NameTableView.Destroy();
	m_PaletteView.Destroy();
	m_MemoryView.Destroy();

	// #### Barcode input dialog has ended.
	m_DatachBarcodeDlg.Destroy();

	// #### Disconnecting from NetPlay
	NetPlay.Disconnect();

	// #### Simulation ended
	Emu.Stop();
	DELETEPTR(Nes);

	// #### Change captions
	::SetWindowText(m_hWnd, this->Caption.c_str());

	if (Config.general.bScreenMode) {
		OnFullScreenGDI(TRUE);
	}

	::MessageBox(m_hWnd, (LPCTSTR)lParam, "ERROR", MB_ICONERROR | MB_OK);
	::InvalidateRect(m_hWnd, NULL, TRUE);

	if (Config.general.bScreenMode) {
		OnChangeMenu(TRUE);
	}

	return	TRUE;
}

WNDCMD	CMainFrame::OnExit(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnExit\n" );

	::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
}

WNDCMD	CMainFrame::OnHelp(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnHelp\n" );

	string	sHelp = CPathlib::MakePath(CApp::GetModulePath(), "virtualnesplus.chm");

	// #### Why use ShellExecute for a substitute?
	::ShellExecute(HWND_DESKTOP, "open", sHelp.c_str(), NULL, NULL, SW_SHOWNORMAL);
	//	::HtmlHelp( m_hWnd, "virtuanes.chm", HH_DISPLAY_TOPIC, NULL ); 
}

WNDCMD	CMainFrame::OnAbout(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnAbout\n" );
	CAboutDlg dlg;
	dlg.DoModal(m_hWnd);
}


WNDCMD	CMainFrame::OnFileOpen(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnFileOpen\n" );

	if (Emu.IsRunning()) {
		Emu.Pause();
	}

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			OnFullScreenGDI(TRUE);
		}
	}

	OPENFILENAME	ofn;
	CHAR	szFile[_MAX_PATH] = { 0 };

	ZEROMEMORY(szFile, sizeof(szFile));
	ZEROMEMORY(&ofn, sizeof(ofn));

	string	pathstr;
	string oldpathstr;
	if (Config.path.bRomPath) {
		pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szRomPath);
	}
	else {
		pathstr = CApp::GetModulePath();
	}

	CHAR	szTitle[256] = { 0 };
	CApp::LoadString(IDS_UI_OPENROM, szTitle, sizeof(szTitle));
	ofn.lpstrTitle = szTitle;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All Support Types\0*.nes;*.fds;*.nsf;*.lzh;*.zip;*.rar;*.cab\0"
		"NES ROM (*.nes)\0*.nes\0Disk Image (*.fds)\0*.fds\0"
		"NES Music File (*.nsf)\0*.nsf\0Archive File\0*.lzh;*.zip;*.rar;*.cab\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_READONLY | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = pathstr.c_str();

	if (::GetOpenFileName(&ofn)) {
		OnEmulationStart(szFile, FALSE);
		if (Config.general.bScreenMode && Emu.IsRunning()) {
			OnChangeMenu(FALSE);
		}
	}
	else {
		if (Emu.IsRunning()) {
			OnFullScreenGDI(FALSE);
		}
	}
	while (Emu.IsPausing()) {
		Emu.Resume();
	}
}

WNDCMD	CMainFrame::OnFileClose(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnFileClose\n" );

		// #### Chat ended
	m_ChatDlg.Destroy();

	// #### Search dialog ended
	m_SearchDlg.Destroy();

	// #### Pattern View Ended
	m_PatternView.Destroy();
	m_NameTableView.Destroy();
	m_PaletteView.Destroy();
	m_MemoryView.Destroy();

	// #### Barcode Input Dialog Ended
	m_DatachBarcodeDlg.Destroy();

	if (Emu.IsRunning()) {
		Emu.Stop();
		DELETEPTR(Nes);

		// #### Change the caption
		::SetWindowText(m_hWnd, this->Caption.c_str());

		// #### redraw
		::InvalidateRect(m_hWnd, NULL, TRUE);
	}

	if (Config.general.bScreenMode) {
		OnChangeMenu(TRUE);
	}
}

WNDMSG	CMainFrame::OnCommandLine(WNDMSGPARAM)
{
	OnEmulationStart((LPSTR)lParam, FALSE);

	return	TRUE;
}
void CenterChildWindow(HWND hChild, HWND hParent) {
	// 获取父窗口的客户区尺寸
	RECT rectParent;
	GetClientRect(hParent, &rectParent);

	// 获取子窗口的尺寸
	RECT rectChild;
	GetWindowRect(hChild, &rectChild);
	int widthChild = rectChild.right - rectChild.left;
	int heightChild = rectChild.bottom - rectChild.top;

	// 计算子窗口新的位置
	int x = rectParent.left + (rectParent.right - rectParent.left - widthChild) / 2;
	int y = rectParent.top + (rectParent.bottom - rectParent.top - heightChild) / 2;

	// 设置子窗口的位置
	SetWindowPos(hChild, NULL, x, y, 0, 0, SWP_NOSIZE);
}
WNDMSG	CMainFrame::OnLauncherCommand(WNDMSGPARAM)
{
	if (Config.general.bScreenMode) {
		if (m_LauncherDlg.m_hWnd) {
			//::SetWindowPos(m_LauncherDlg.m_hWnd, 
			//	HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			CenterChildWindow(m_LauncherDlg.m_hWnd, this->m_hWnd);
		}
	}
	::SetForegroundWindow(m_hWnd);

	OnEmulationStart((LPSTR)lParam, FALSE);

	return	TRUE;
}

WNDMSG	CMainFrame::OnNetPlay(WNDMSGPARAM)
{
	DEBUGOUT("CMainFrame::OnNetPlay\n");
	lResult = NetPlay.WndProc(hWnd, wParam, lParam);
	return	TRUE;
}

WNDMSG	CMainFrame::OnNetPlayClose(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnNetPlayClose\n" );
	NetPlay.SetMsgWnd(NULL);
	NetPlay.Disconnect();

	LPCSTR	szErrStr = CApp::GetErrorString(IDS_ERROR_NETWORKDISCONNECT);
	::PostMessage(m_hWnd, WM_VNS_ERRORMSG, 0, (LPARAM)szErrStr);
	return	TRUE;
}

WNDMSG	CMainFrame::OnNetPlayError(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnNetPlayError\n" );
	NetPlay.SetMsgWnd(NULL);
	NetPlay.Disconnect();

	LPCSTR	szErrStr = CApp::GetErrorString(IDS_ERROR_NETWORKERROR);
	::PostMessage(m_hWnd, WM_VNS_ERRORMSG, 0, (LPARAM)szErrStr);
	return	TRUE;
}

WNDMSG	CMainFrame::OnNetPlayChatPopup(WNDMSGPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnNetPlayChatPopup\n" );
	DEBUGOUT("CMainFrame::OnNetPlayChatPopup\n");
	if (!NetPlay.IsConnect())
		return	TRUE;

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, TRUE);
		}
		else {
			DirectDraw.SetFullScreenGDI(TRUE);
		}
		// #### If the dialogue is not on the screen, it will move to the center :)
		RECT	rcParent, rc;
		::GetWindowRect(m_hWnd, &rcParent);
		::GetWindowRect(m_ChatDlg.m_hWnd, &rc);

		if ((rc.right < rcParent.left) || (rc.left > rcParent.right)
			|| (rc.bottom < rcParent.top) || (rc.top > rcParent.bottom)) {
			INT x = rcParent.left + (rcParent.right - rcParent.left) / 2 - (rc.right - rc.left) / 2;
			INT y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - (rc.bottom - rc.top) / 2;
			::SetWindowPos(m_ChatDlg.m_hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else {
			::SetWindowPos(m_ChatDlg.m_hWnd, NULL, 0, 0, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		::SetWindowPos(m_ChatDlg.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	else {
		::SetWindowPos(m_ChatDlg.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW | SWP_NOACTIVATE);
	}

	return	TRUE;
}

WNDCMD	CMainFrame::OnRecentOpen(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnRecentOpen ID=%d\n", uID-ID_MRU_FILE0 );
	//	DEBUGOUT( "Fname: \"%s\"\n", CRecent::GetName( (INT)uID-ID_MRU_FILE0 ) );

	OnEmulationStart(CRecent::GetName((INT)uID - ID_MRU_FILE0), FALSE);
}

WNDCMD	CMainFrame::OnRecentOpenPath(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnRecentOpenPath ID=%d\n", uID-ID_MRU_FILE0 );
	//	DEBUGOUT( "Fname: \"%s\"\n", CRecent::GetPath( (INT)uID-ID_MRU_FILE0 ) );

	if (Emu.IsRunning()) {
		Emu.Pause();
	}

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			OnFullScreenGDI(TRUE);
		}
	}

	OPENFILENAME	ofn;
	CHAR	szFile[_MAX_PATH];

	ZEROMEMORY(szFile, sizeof(szFile));
	ZEROMEMORY(&ofn, sizeof(ofn));

	CHAR	szTitle[256];
	CApp::LoadString(IDS_UI_OPENROM, szTitle, sizeof(szTitle));
	ofn.lpstrTitle = szTitle;

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "All Support Types\0*.nes;*.fds;*.nsf;*.lzh;*.zip;*.rar;*.cab\0"
		"NES ROM (*.nes)\0*.nes\0Disk Image (*.fds)\0*.fds\0"
		"NES Music File (*.nsf)\0*.nsf\0Archive File\0*.lzh;*.zip;*.rar;*.cab\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_READONLY | OFN_HIDEREADONLY | OFN_EXPLORER | OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = CRecent::GetPath((INT)uID - ID_MRU_PATH0);


	if (::GetOpenFileName(&ofn)) {
		OnEmulationStart(szFile, FALSE);
	}
	else {
		if (Emu.IsRunning()) {
			OnFullScreenGDI(FALSE);
		}
	}
	while (Emu.IsPausing()) {
		Emu.Resume();
	}
}

WNDMSG	CMainFrame::OnDropFiles(WNDMSGPARAM)
{
	DEBUGOUT("CMainFrame::OnDropFiles\n");
	::SetForegroundWindow(m_hWnd);

	CHAR szFile[_MAX_PATH];
	::DragQueryFile((HDROP)wParam, 0, szFile, _MAX_PATH);
	::DragFinish((HDROP)wParam);

	INT	ret;
	if ((ret = ROM::IsRomFile(szFile)) >= 0) {
		DEBUGOUT("ROM File CHK=%d\n", ret);
		if (ret == IDS_ERROR_ILLEGALHEADER) {
			if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
				return	TRUE;

			OnEmulationStart(szFile, TRUE);
			return	TRUE;
		}
		else if (ret == 0) {
			OnEmulationStart(szFile, TRUE);
			return	TRUE;
		}
	}

	if (Emu.IsRunning() && !NetPlay.IsConnect()) {
		if ((ret = NES::IsStateFile(szFile, Nes->rom)) >= 0) {
			DEBUGOUT("State File CHK=%d\n", ret);
			if (ret == IDS_ERROR_ILLEGALSTATECRC) {
				if (Config.emulator.bCrcCheck) {
					if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
						return	TRUE;
				}
			}
			Emu.EventParam2(CEmuThread::EV_STATE_LOAD, (LONG_PTR)szFile, -1);
		}
		else
			if ((ret = NES::IsMovieFile(szFile, Nes->rom)) >= 0) {
				DEBUGOUT("Movie File CHK=%d\n", ret);
				if (ret == IDS_ERROR_ILLEGALMOVIEOLD) {
					::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONHAND | MB_OK);
					return	TRUE;
				}
				else
					if (ret == IDS_ERROR_ILLEGALMOVIEVER) {
						if (Config.emulator.bCrcCheck) {
							if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
								return	TRUE;
						}
					}
					else
						if (ret == IDS_ERROR_ILLEGALMOVIECRC) {
							if (Config.emulator.bCrcCheck) {
								if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
									return	TRUE;
							}
						}
				Emu.EventParam(CEmuThread::EV_MOVIE_PLAY, (LONG_PTR)szFile);
			}
	}

	return	TRUE;
}

void	CMainFrame::OnEmulationStart(LPCSTR szFile, BOOL bChecked)
{
	// #### Search Dialog Ended
	m_SearchDlg.Destroy();

	// #### Barcode input dialog ended.
	m_DatachBarcodeDlg.Destroy();

	// #### Pattern viewer finished.
//	m_PatternView.Destroy();
//	m_NameTableView.Destroy();
//	m_PaletteView.Destroy();
//	m_MemoryView.Destroy();

	// #### Emulation completed.
//	Emu.Stop();
//	DELETEPTR( Nes );

	try {
		if (!bChecked) {
			INT	ret;
			if ((ret = ROM::IsRomFile(szFile)) != 0) {
				// #### Fatal error
				if (ret == IDS_ERROR_OPEN) {
					// #### I can't open the xxx file.
					::wsprintf(szErrorString, CApp::GetErrorString(ret), szFile);
					throw	szErrorString;
				}
				if (ret == IDS_ERROR_READ) {
					throw	CApp::GetErrorString(ret);
				}
				if (ret == IDS_ERROR_UNSUPPORTFORMAT) {
					throw	CApp::GetErrorString(ret);
				}

				// #### Checkbox
				if (ret == IDS_ERROR_ILLEGALHEADER) {
					if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
						return;
				}
				//			} else {
				//				throw	CApp::GetErrorString( IDS_ERROR_UNSUPPORTFORMAT );
			}
		}

		// #### Emulation ended.
		Emu.Stop();
		DELETEPTR(Nes);

		if ((Nes = new NES(szFile))) {
			CRecent::Add(szFile);

			if (Config.general.bScreenMode) {
				DirectDraw.SetFullScreenGDI(FALSE);
				OnChangeMenu(FALSE);
			}
			else {
				if (Config.emulator.bLoadFullscreen) {
					::PostMessage(m_hWnd, WM_COMMAND, (WPARAM)ID_FULLSCREEN, (LPARAM)0);
				}
			}

			// #### Change the caption
			{
				string	str = this->Caption + " - " + Nes->rom->GetRomName();
				::SetWindowText(m_hWnd, str.c_str());
			}

			// #### Emulation Thread Start
			Emu.Start(m_hWnd, Nes);
		}
		else {
			// #### I can't open the xxx file.
			LPCSTR	szErrStr = CApp::GetErrorString(IDS_ERROR_OPEN);
			sprintf(szErrorString, szErrStr, szFile);
			throw	szErrorString;
		}
	}
	catch (CHAR* str) {
		::strcpy(szErrorString, str);
		PostMessage(m_hWnd, WM_VNS_ERRORMSG, 0, (LPARAM)szErrorString);
#ifndef	_DEBUG
	}
	catch (...) {
		// #### Uncertain error occurred.
		::strcpy(szErrorString, CApp::GetErrorString(IDS_ERROR_UNKNOWN));
		PostMessage(m_hWnd, WM_VNS_ERRORMSG, 0, (LPARAM)szErrorString);
#endif
	}
}

WNDCMD	CMainFrame::OnRomInfo(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnRomInfo\n" );

	if (!Emu.IsRunning() || !Nes)
		return;

	CRomInfoDlg dlg;

	// #### Setting up the member.
	::strcpy(dlg.m_szName, Nes->rom->GetRomName());
	dlg.m_nMapper = Nes->rom->GetMapperNo();
	dlg.m_nPRG = Nes->rom->GetPROM_SIZE();
	dlg.m_nCHR = Nes->rom->GetVROM_SIZE();
	dlg.m_bMirror = Nes->rom->IsVMIRROR();
	dlg.m_bSram = Nes->rom->IsSAVERAM();
	dlg.m_b4Screen = Nes->rom->Is4SCREEN();
	dlg.m_bTrainer = Nes->rom->IsTRAINER();
	dlg.m_bVSUnisystem = Nes->rom->IsVSUNISYSTEM();

	if (Nes->rom->GetMapperNo() < 256 && Nes->rom->GetMapperNo() != 20) {
		dlg.m_dwCRC = Nes->rom->GetPROM_CRC();
		dlg.m_dwCRCALL = Nes->rom->GetROM_CRC();
		dlg.m_dwCRCCHR = Nes->rom->GetVROM_CRC();
	}

	if (!m_bMenu)
		OnFullScreenGDI(TRUE);
	dlg.DoModal(m_hWnd);
	if (!m_bMenu)
		OnFullScreenGDI(FALSE);
}

WNDCMD	CMainFrame::OnWaveRecord(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnWaveRecord\n" );

	if (!Emu.IsRunning() || !Nes)
		return;

	if (Emu.IsWaveRecord()) {
		// #### If you stop the recording, it will be over.
		Emu.Event(CEmuThread::EV_WAVEREC_STOP);
	}
	else {
		// #### First time
		string	pathstr, tempstr;
		if (Config.path.bWavePath) {
			pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szWavePath);
			::CreateDirectory(pathstr.c_str(), NULL);
		}
		else {
			pathstr = Nes->rom->GetRomPath();
		}
		tempstr = CPathlib::MakePathExt(pathstr.c_str(), Nes->rom->GetRomName(), "wav");
		DEBUGOUT("Path: %s\n", tempstr.c_str());

		OPENFILENAME	ofn;
		CHAR	szFile[_MAX_PATH];

		::strcpy(szFile, tempstr.c_str());
		ZEROMEMORY(&ofn, sizeof(ofn));

		CHAR	szTitle[256];
		CApp::LoadString(IDS_UI_WAVERECORD, szTitle, sizeof(szTitle));
		ofn.lpstrTitle = szTitle;
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = m_hWnd;
		ofn.lpstrFile = szFile;
		ofn.lpstrDefExt = "wav";
		ofn.nMaxFile = sizeof(szFile);
		ofn.lpstrFilter = "Wave Files(*.wav)\0*.wav\0All Files(*.*)\0*.*\0";
		ofn.nFilterIndex = 1;
		ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;
		ofn.lpstrInitialDir = pathstr.c_str();

		if (!m_bMenu)
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, TRUE);

		if (::GetSaveFileName(&ofn)) {
			Emu.EventParam(CEmuThread::EV_WAVEREC_START, (LONG_PTR)szFile);
		}

		if (!m_bMenu)
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, FALSE);
	}
}

WNDCMD	CMainFrame::OnLauncher(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnLauncher\n" );

	if (!m_LauncherDlg.m_hWnd) {
		m_LauncherDlg.Create(NULL);
	}

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, TRUE);
		}
		else {
			DirectDraw.SetFullScreenGDI(TRUE);
		}
		// #### Move the dialogue to the center :)
		RECT	rcParent, rc;
		::GetWindowRect(m_hWnd, &rcParent);
		if (m_LauncherDlg.m_hWnd != 0)
		{
			::GetWindowRect(m_LauncherDlg.m_hWnd, &rc);
			INT x = rcParent.left + (rcParent.right - rcParent.left) / 2 - (rc.right - rc.left) / 2;
			INT y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - (rc.bottom - rc.top) / 2;
			::SetWindowPos(m_LauncherDlg.m_hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
			::SetWindowPos(m_LauncherDlg.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	else {
		if (m_LauncherDlg.m_hWnd != 0)
			::SetWindowPos(m_LauncherDlg.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	if (m_LauncherDlg.m_hWnd != 0)
		::ShowWindow(m_LauncherDlg.m_hWnd, SW_SHOW);
}

WNDCMD	CMainFrame::OnNetPlayConnect(WNDCMDPARAM)
{
	if (!Emu.IsRunning() || !Nes)
		return;
	if (Nes->IsMoviePlay() || Nes->IsMovieRec())
		return;
	if (NetPlay.IsConnect())
		return;

	Emu.Pause();

	if (!m_bMenu)
		OnFullScreenGDI(TRUE);

	CNetPlayDlg	dlg;
	if (dlg.DoModal(m_hWnd) == IDOK) {
		if (!m_ChatDlg.m_hWnd) {
			m_ChatDlg.Create(NULL);
		}
		if (m_ChatDlg.m_hWnd != 0) {
			::ShowWindow(m_ChatDlg.m_hWnd, SW_SHOW);
		}

		Emu.Event(CEmuThread::EV_NETPLAY_START);

		// test
		if (!Config.emulator.bBackground) {
			Emu.Resume();
		}
	}

	if (!m_bMenu)
		OnFullScreenGDI(FALSE);

	Emu.Resume();
}

WNDCMD	CMainFrame::OnNetPlayDisconnect(WNDCMDPARAM)
{
	if (!NetPlay.IsConnect())
		return;

	Emu.Pause();
	NetPlay.Disconnect();
	Emu.Resume();
}

WNDCMD	CMainFrame::OnNetPlayChat(WNDCMDPARAM)
{
	DEBUGOUT("CMainFrame::OnNetPlayChat\n");
	if (!NetPlay.IsConnect())
		return;

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, TRUE);
		}
		else {
			DirectDraw.SetFullScreenGDI(TRUE);
		}
		// #### Dialog moves to the center of the screen if it is outside :)
		RECT	rcParent, rc;
		::GetWindowRect(m_hWnd, &rcParent);
		::GetWindowRect(m_ChatDlg.m_hWnd, &rc);

		if ((rc.right < rcParent.left) || (rc.left > rcParent.right)
			|| (rc.bottom < rcParent.top) || (rc.top > rcParent.bottom)) {
			INT x = rcParent.left + (rcParent.right - rcParent.left) / 2 - (rc.right - rc.left) / 2;
			INT y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - (rc.bottom - rc.top) / 2;
			::SetWindowPos(m_ChatDlg.m_hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		else {
			::SetWindowPos(m_ChatDlg.m_hWnd, NULL, 0, 0, -1, -1, SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
		}
		::SetWindowPos(m_ChatDlg.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	}
	else {
		::SetWindowPos(m_ChatDlg.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
	}
	//	::ShowWindow( m_ChatDlg.m_hWnd, SW_SHOW );
}

WNDCMD	CMainFrame::OnSearch(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnSearch\n" );

	if (!Emu.IsRunning() || !Nes)
		return;

	if (!m_SearchDlg.m_hWnd) {
		m_SearchDlg.Create(NULL);
	}

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, TRUE);
		}
		else {
			DirectDraw.SetFullScreenGDI(TRUE);
		}
		// #### Move the dialogue to the center :)
		RECT	rcParent, rc;
		::GetWindowRect(m_hWnd, &rcParent);
		if (m_SearchDlg.m_hWnd != 0) {
			::GetWindowRect(m_SearchDlg.m_hWnd, &rc);
			INT x = rcParent.left + (rcParent.right - rcParent.left) / 2 - (rc.right - rc.left) / 2;
			INT y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - (rc.bottom - rc.top) / 2;
			::SetWindowPos(m_SearchDlg.m_hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			::SetWindowPos(m_SearchDlg.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	else {
		if (m_SearchDlg.m_hWnd != 0)
			::SetWindowPos(m_SearchDlg.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	if (m_SearchDlg.m_hWnd != 0) {
		::ShowWindow(m_SearchDlg.m_hWnd, SW_SHOW);
	}
}

WNDCMD	CMainFrame::OnCheat(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnCheat\n" );

	if (!Emu.IsRunning() || !Nes)
		return;

	//	if( m_SearchDlg.m_hWnd ) {
	//		if( ::IsWindowVisible( m_SearchDlg.m_hWnd ) )
	//			return;
	//	}

	if (!m_bMenu)
		OnFullScreenGDI(TRUE);

	CCheatCodeDlg	dlg;

	dlg.DoModal(m_hWnd);

	if (!m_bMenu)
		OnFullScreenGDI(FALSE);
}

WNDCMD	CMainFrame::OnGenie(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnGenie\n" );

	if (!Emu.IsRunning() || !Nes)
		return;

	Emu.Pause();

	string	pathstr, tempstr;
	if (Config.path.bCheatPath) {
		pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szCheatPath);
		::CreateDirectory(pathstr.c_str(), NULL);
	}
	else {
		pathstr = Nes->rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt(pathstr.c_str(), Nes->rom->GetRomName(), "gen");
	DEBUGOUT("Path: %s\n", tempstr.c_str());

	OPENFILENAME	ofn;
	CHAR	szFile[_MAX_PATH];

	::strcpy(szFile, tempstr.c_str());
	ZEROMEMORY(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "GameGeine Files(*.gen)\0*.gen\0All Files(*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = pathstr.c_str();

	CHAR	szTitle[256];
	CApp::LoadString(IDS_UI_LOADGENIECODE, szTitle, sizeof(szTitle));
	ofn.lpstrTitle = szTitle;
	if (::GetOpenFileName(&ofn)) {
		Nes->GenieLoad(szFile);
	}

	Emu.Resume();
}

WNDCMD	CMainFrame::OnCheatCommand(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnCheatCommand\n" );

	if (!Emu.IsRunning() || !Nes)
		return;

	if (uID == ID_CHEAT_ENABLE) {
		Nes->SetCheatCodeAllFlag(TRUE, TRUE);
	}
	else
		if (uID == ID_CHEAT_DISABLE) {
			Nes->SetCheatCodeAllFlag(FALSE, TRUE);
		}
}

WNDCMD	CMainFrame::OnDatachBacode(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnDatachBacode\n" );

	if (!Emu.IsRunning() || !Nes)
		return;

	if (!m_DatachBarcodeDlg.m_hWnd) {
		m_DatachBarcodeDlg.Create(NULL);
	}

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, TRUE);
		}
		else {
			DirectDraw.SetFullScreenGDI(TRUE);
		}
		// #### move dialog
		RECT	rcParent, rc;
		::GetWindowRect(m_hWnd, &rcParent);
		if (m_DatachBarcodeDlg.m_hWnd != 0)
		{
			::GetWindowRect(m_DatachBarcodeDlg.m_hWnd, &rc);
			INT x = rcParent.left + (rcParent.right - rcParent.left) / 2 - (rc.right - rc.left) / 2;
			INT y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - (rc.bottom - rc.top) / 2;
			::SetWindowPos(m_DatachBarcodeDlg.m_hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			::SetWindowPos(m_DatachBarcodeDlg.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	else {
		if (m_DatachBarcodeDlg.m_hWnd != 0)
			::SetWindowPos(m_DatachBarcodeDlg.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}
	if (m_DatachBarcodeDlg.m_hWnd != 0)
		::ShowWindow(m_DatachBarcodeDlg.m_hWnd, SW_SHOW);
}


WNDCMD	CMainFrame::OnDipSwitch(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnDipSwitch\n" );
	CDipSwitchDlg dlg;
	if (dlg.DoModal(m_hWnd) == IDOK) {
	}
}

WNDCMD	CMainFrame::OnEmulatorCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnEmulatorCfg\n" );
	CEmulatorDlg dlg;
	if (dlg.DoModal(m_hWnd) == IDOK) {
		Emu.SetPriority((INT)Config.emulator.nPriority);
	}
}

WNDCMD	CMainFrame::OnGraphicsCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnGraphicsCfg\n" );
	CGraphicsDlg dlg;
	if (dlg.DoModal(m_hWnd) == IDOK) {
		Emu.Pause();

		// #### Rebuilt
		BOOL	bRebuildDirectDraw = FALSE;
		BOOL	bRebuildSurface = FALSE;
		BOOL	bMenuOFF = FALSE;

		if (DirectDraw.GetUseHEL() != Config.graphics.bUseHEL)
			bRebuildDirectDraw = TRUE;
		if (DirectDraw.GetSystemMemory() != Config.graphics.bSystemMemory)
			bRebuildSurface = TRUE;

		// #### Setting up DirectDraw:  DirectDraw is a graphics library that allows you to create 2D and 3D graphics in Windows applications. Here are the steps to set it up:  1. Install the DirectX SDK: The DirectX SDK includes the necessary files and tools to create DirectDraw applications. You can download the SDK from the Microsoft website. 2. Create a new project: Create a new project in your preferred programming language (C++, C#, etc.) and add the necessary headers and libraries for DirectDraw. 3. Initialize DirectDraw: Call the `DDrawCreate` function to initialize DirectDraw and create a device context. This will allow you to draw graphics on the screen. 4. Set up the display: Define the display dimensions, bits per pixel, and other display settings using the `DDrawSetDisplayMode` function. 5. Create a surface: Use the `DDrawCreateSurface` function to create a surface for drawing graphics. This can be a 2D or 3D surface, depending on your needs. 6. Draw graphics: Use the `DDrawDraw` function to draw graphics on the surface. You can use various shapes, lines, and other graphical elements to create your desired image. 7. Clean up: Once you're done drawing, call the `DDrawDestroySurface` function to free resources and clean up after yourself.  Here is an example of how to set up DirectDraw in C++: ```cpp #include <d3dx9.h>  int main() {     // Initialize DirectDraw     HRESULT hr = DD_OK;     IDirect3DDevice9 *device = NULL;     UINT width = 640, height = 480;     DWORD bitsPerPixel = 32;     DWORD displayFrequency = 70; // Hz      hr = DDrawCreate(&device, width, height, bitsPerPixel, displayFrequency);     if (FAILED(hr)) {         return 1;     }      // Set up the display     hr = DDrawSetDisplayMode(device, width, height, bitsPerPixel, displayFrequency);     if (FAILED(hr)) {         return 1;     }      // Create a surface     IDirect3DSurface9 *surface = NULL;     hr = DDrawCreateSurface(device, width, height, bitsPerPixel, D3DFMT_UNKNOWN, &surface);     if (FAILED(hr)) {         return 1;     }      // Draw graphics     IDirect3DDevice9 *drawDevice = NULL;     hr = device->QueryInterface(IID_IDirect3DDevice9, (void **)&drawDevice);     if (FAILED(hr)) {         return 1;     }     surface->GetBuffer(sizeof(RECT), &rect);     RECT clipRect = *((RECT *)&rect);     drawDevice->SetClipStatus(D3DCLIP_VERT | D3DCLIP_HORZ);     drawDevice->Clear(0, 0, width, height, 0, 0, D3DFMT_UNKNOWN, 0, NULL);     drawDevice->BeginScene();     // Draw your graphics here     drawDevice->EndScene();      // Clean up     surface->Release();     device->Release();     return 0; } ```
		DirectDraw.SetFlipMode(Config.graphics.bSyncDraw);
		DirectDraw.SetAspectMode(Config.graphics.bAspect);
		DirectDraw.SetAllLineMode(Config.graphics.bAllLine);
		DirectDraw.SetMaxZoom(Config.graphics.bFitZoom);
		DirectDraw.SetTVFrameMode(Config.graphics.bTVFrame);
		DirectDraw.SetScanlineMode(Config.graphics.bScanline);
		DirectDraw.SetScanlineColor(Config.graphics.nScanlineColor);

		DirectDraw.SetUseHEL(Config.graphics.bUseHEL);
		DirectDraw.SetDoubleSize(Config.graphics.bDoubleSize);
		DirectDraw.SetSystemMemory(Config.graphics.bSystemMemory);

		DirectDraw.SetWindowVSyncMode(Config.graphics.bWindowVSync);

		if (Config.general.bScreenMode) {
			// #### If there has been a change to the screen mode, please let me know.
			if (!DirectDraw.IsNowDisplayMode(Config.graphics.dwDisplayWidth,
				Config.graphics.dwDisplayHeight,
				Config.graphics.dwDisplayDepth,
				Config.graphics.dwDisplayRate)) {
				if (!bRebuildDirectDraw)
					DirectDraw.BeginDisplayChange();

				// #### Screen mode setting
				DirectDraw.SetDisplayMode(Config.graphics.dwDisplayWidth,
					Config.graphics.dwDisplayHeight,
					Config.graphics.dwDisplayDepth,
					Config.graphics.dwDisplayRate);

				DirectDraw.SetScreenMode(Config.general.bScreenMode);
				if (!bRebuildDirectDraw)
					DirectDraw.EndDisplayChange();

				::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),
					::GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);

				// #### pallet
				::PostMessage(m_hWnd, WM_QUERYNEWPALETTE, 0, 0);

				// #### Surfing will not be done twice.
				bRebuildSurface = FALSE;

				bMenuOFF = TRUE;
			}
		}
		else {
			OnSetWindowSize();
		}
		if (bRebuildDirectDraw) {
			DirectDraw.ReleaseDDraw();
			DirectDraw.InitialDDraw(m_hWnd);
			DirectDraw.InitialSurface(Config.general.bScreenMode);
			bMenuOFF = TRUE;
		}
		else
			if (bRebuildSurface) {
				DirectDraw.ReleaseSurface();
				DirectDraw.InitialSurface(Config.general.bScreenMode);
				bMenuOFF = TRUE;
			}

		if (Config.general.bScreenMode && bMenuOFF) {
			if (Emu.IsRunning()) {
				OnChangeMenu(FALSE);
			}
		}

		Emu.Resume();
	}
}

WNDCMD	CMainFrame::OnSoundCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnSoundCfg\n" );
	CSoundDlg dlg;

	DWORD	rate, bits;
	DirectSound.GetSamplingRate(rate, bits);
	BOOL	bSoundOn = DirectSound.IsStreamPlay();

	if (dlg.DoModal(m_hWnd) == IDOK) {
		if (Config.sound.nRate != (INT)rate
			|| Config.sound.nBits != (INT)bits
			|| Config.sound.nBufferSize != DirectSound.GetBufferSize()) {
			Emu.Pause();
			if (bSoundOn) {
				DirectSound.StreamStop();
			}

			DirectSound.ReleaseBuffer();

			DirectSound.SetSamplingRate(Config.sound.nRate, Config.sound.nBits);
			DirectSound.SetBufferSize(Config.sound.nBufferSize);

			if (DirectSound.InitialBuffer()) {
				if (Nes) {
					Nes->SoundSetup();
				}
				if (bSoundOn) {
					DirectSound.StreamPlay();
					DirectSound.StreamPause();
				}
			}
			Emu.Resume();
		}
	}
}

WNDCMD	CMainFrame::OnControllerCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnControllerCfg\n" );
	CControllerDlg dlg;

	CWndHook::SetFiltering(TRUE);
	dlg.DoModal(m_hWnd);
	CWndHook::SetFiltering(FALSE);
}

WNDCMD	CMainFrame::OnShortcutCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnShortcutCfg\n" );
	CShortcutDlg dlg;

	CWndHook::SetFiltering(TRUE);

	if (dlg.DoModal(m_hWnd) == IDOK) {
		::SetMenu(m_hWnd, NULL);
		::DestroyMenu(m_hMenu);
		// #### Re-loading the menu
		m_hMenu = CApp::LoadMenu(IDR_MENU);
		CApp::SetMenu(m_hMenu);
		::SetMenu(m_hWnd, m_hMenu);
		OnRebuildMenu();
	}

	CWndHook::SetFiltering(FALSE);
}

WNDCMD	CMainFrame::OnFolderCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnFolderCfg\n" );
	CFolderDlg dlg;
	dlg.DoModal(m_hWnd);
}

WNDCMD	CMainFrame::OnLanguageCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnLanguageCfg\n" );
	CLanguageDlg dlg;

	if (dlg.DoModal(m_hWnd) == IDOK) {
		// #### If the launcher is running, then close it.
		BOOL	bLauncher = FALSE;
		BOOL	bLauncherVisible = FALSE;
		if (m_LauncherDlg.m_hWnd && ::IsWindow(m_LauncherDlg.m_hWnd)) {
			bLauncherVisible = ::IsWindowVisible(m_LauncherDlg.m_hWnd);
			bLauncher = TRUE;
			m_LauncherDlg.Destroy();
		}

		// #### Chat dialogue is closing.
		BOOL	bChat = FALSE;
		BOOL	bChatVisible = FALSE;
		if (m_ChatDlg.m_hWnd && ::IsWindow(m_ChatDlg.m_hWnd)) {
			bChatVisible = ::IsWindowVisible(m_ChatDlg.m_hWnd);
			bChat = TRUE;
			m_ChatDlg.Destroy();
		}

		// #### The search dialog is closing.
		if (m_SearchDlg.m_hWnd && ::IsWindow(m_SearchDlg.m_hWnd)) {
			m_SearchDlg.Destroy();
		}

		// #### The barcode input dialog is closing.
		if (m_DatachBarcodeDlg.m_hWnd && ::IsWindow(m_DatachBarcodeDlg.m_hWnd)) {
			m_DatachBarcodeDlg.Destroy();
		}

		// #### menu disposal
		::SetMenu(m_hWnd, NULL);
		::DestroyMenu(m_hMenu);

		// #### Opening the Original Plugin
		::FreeLibrary(CApp::GetPlugin());
		// #### New Plugin Load
		HINSTANCE hPlugin;
		if (!(hPlugin = ::LoadLibrary(CPlugin::GetPluginPath()))) {
			::MessageBox(m_hWnd, "Language plug-in load failed.", "VirtualNES", MB_ICONERROR | MB_OK);
			::PostMessage(m_hWnd, WM_CLOSE, 0, 0);
			return;
		}
		CApp::SetPlugin(hPlugin);
		// #### Menu reloading
		m_hMenu = CApp::LoadMenu(IDR_MENU);
		CApp::SetMenu(m_hMenu);
		::SetMenu(m_hWnd, m_hMenu);

		OnRebuildMenu();

		// #### Re-starting the launcher
		if (bLauncher) {
			m_LauncherDlg.Create(NULL);
			if (bLauncherVisible) {
				::ShowWindow(m_LauncherDlg.m_hWnd, SW_SHOW);
			}
		}

		// #### Chat Reboot
		if (bChat) {
			m_ChatDlg.Create(NULL);
			if (bChatVisible) {
				::ShowWindow(m_ChatDlg.m_hWnd, SW_SHOW);
			}
		}

		::SetForegroundWindow(m_hWnd);
	}
}

WNDCMD	CMainFrame::OnMovieCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnMovieCfg\n" );
	CMovieDlg dlg;
	dlg.DoModal(m_hWnd);
}

WNDCMD	CMainFrame::OnGameOptionCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnGameOptionCfg\n" );
	CGameOptionDlg dlg;
	dlg.DoModal(m_hWnd);
}

WNDCMD	CMainFrame::OnJoyAxisCfg(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnJoyAxisCfg\n" );
	CJoyAxisDlg dlg;
	dlg.DoModal(m_hWnd);

	DirectInput.SetJoyAxisMode(Config.general.JoyAxisSetting);
}

WNDCMD	CMainFrame::OnPaletteEditCfg(WNDCMDPARAM)
{
	DEBUGOUT("CMainFrame::OnPaletteEditCfg\n");

	if (!m_PaletteEdit.m_hWnd) {
		m_PaletteEdit.Create(NULL);
	}

	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, TRUE);
		}
		else {
			DirectDraw.SetFullScreenGDI(TRUE);
		}
		// #### Move the dialogue to the center :)
		RECT	rcParent, rc;
		::GetWindowRect(m_hWnd, &rcParent);
		if (m_PaletteEdit.m_hWnd != 0) {
			::GetWindowRect(m_PaletteEdit.m_hWnd, &rc);
			INT x = rcParent.left + (rcParent.right - rcParent.left) / 2 - (rc.right - rc.left) / 2;
			INT y = rcParent.top + (rcParent.bottom - rcParent.top) / 2 - (rc.bottom - rc.top) / 2;
			::SetWindowPos(m_PaletteEdit.m_hWnd, NULL, x, y, -1, -1, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

			::SetWindowPos(m_PaletteEdit.m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		}
	}
	else {
		if (m_PaletteEdit.m_hWnd != 0)::SetWindowPos(m_PaletteEdit.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
	}

	if (m_PaletteEdit.m_hWnd != 0)
		::ShowWindow(m_PaletteEdit.m_hWnd, SW_SHOW);
}

WNDCMD	CMainFrame::OnFullScreen(WNDCMDPARAM)
{
	DEBUGOUT("CMainFrame::OnFullScreen\n");

	m_bKeyEnable = FALSE;

	// #### Hazardous
	Emu.Pause();

	Config.general.bScreenMode = !Config.general.bScreenMode;
	//	OnChangeMenu( FALSE );

	if (!Config.general.bScreenMode) {
		// FullScreen to Window mode
		DirectDraw.BeginDisplayChange();
		// #### Return to the original position
		::SetWindowLong(m_hWnd, GWL_STYLE, m_StyleBackup);
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, m_ExStyleBackup);
		::SetWindowPlacement(m_hWnd, &m_WindowPlacement);
	}
	else {
		// Window to FullScreen mode
			// #### Preserving the Position
	//		m_bZoomed = ::IsZoomed( m_hWnd );
		m_WindowPlacement.length = sizeof(m_WindowPlacement);
		::GetWindowPlacement(m_hWnd, &m_WindowPlacement);

		m_StyleBackup = ::GetWindowLong(m_hWnd, GWL_STYLE);
		m_ExStyleBackup = ::GetWindowLong(m_hWnd, GWL_EXSTYLE);
		::SetWindowLong(m_hWnd, GWL_STYLE, WS_POPUP);
		::SetWindowLong(m_hWnd, GWL_EXSTYLE, 0);

		// #### Screen mode setting
		DirectDraw.SetDisplayMode(Config.graphics.dwDisplayWidth,
			Config.graphics.dwDisplayHeight,
			Config.graphics.dwDisplayDepth,
			Config.graphics.dwDisplayRate);

		DirectDraw.BeginDisplayChange();
	}

	//	OnChangeMenu( FALSE );

	DirectDraw.SetScreenMode(Config.general.bScreenMode);
	DirectDraw.EndDisplayChange();

	if (!Config.general.bScreenMode) {
		if (m_LauncherDlg.m_hWnd && ::IsWindow(m_LauncherDlg.m_hWnd)) {
			::SetWindowPos(m_LauncherDlg.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		}

		OnChangeMenu(TRUE);
		::SetForegroundWindow(m_hWnd);

		::RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
	}
	else {
		if (Emu.IsRunning()) {
			OnChangeMenu(FALSE);
		}
		else {
			OnChangeMenu(TRUE);
		}
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, ::GetSystemMetrics(SM_CXSCREEN),
			::GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
	}

	// #### Hazardous
	Emu.Resume();

	m_bKeyEnable = TRUE;
}

WNDCMD	CMainFrame::OnZoom(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnZoom %d\n", uID-ID_ZOOMx1 );

		// #### Full screen mode is ignored.
	if (Config.general.bScreenMode)
		return;

	Config.general.nScreenZoom = (INT)(uID - ID_ZOOMx1);
	OnSetWindowSize();

	CHAR	szStr[64];
	::wsprintf(szStr, "Screen Zoom *%d", uID - ID_ZOOMx1 + 1);
	Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)szStr);
}

WNDCMD	CMainFrame::OnViewCommand(WNDCMDPARAM)
{
	if (!Emu.IsRunning() || !Nes)
		return;

	switch (uID) {
	case	ID_VIEW_PATTERN:
		if (!m_PatternView.m_hWnd) {
			m_PatternView.Create(HWND_DESKTOP);
		}
		if (m_PatternView.m_hWnd != 0)
			::SetWindowPos(m_PatternView.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		break;
	case	ID_VIEW_NAMETABLE:
		if (!m_NameTableView.m_hWnd) {
			m_NameTableView.Create(HWND_DESKTOP);
		}
		if (m_NameTableView.m_hWnd != 0)
			::SetWindowPos(m_NameTableView.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		break;
	case	ID_VIEW_PALETTE:
		if (!m_PaletteView.m_hWnd) {
			m_PaletteView.Create(HWND_DESKTOP);
		}
		if (m_PaletteView.m_hWnd != 0)
			::SetWindowPos(m_PaletteView.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		break;

	case	ID_VIEW_MEMORY:
		if (!m_MemoryView.m_hWnd) {
			m_MemoryView.Create(HWND_DESKTOP);
		}
		if (m_MemoryView.m_hWnd != 0)
			::SetWindowPos(m_MemoryView.m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		break;
	default:
		break;
	}
}

WNDCMD	CMainFrame::OnEmuCommand(WNDCMDPARAM)
{
	//	DEBUGOUT( "CMainFrame::OnEmuCommand %d\n", uID );

	switch (uID) {
	case	ID_HWRESET:
		Emu.Event(CEmuThread::EV_HWRESET);
		break;
	case	ID_SWRESET:
		Emu.Event(CEmuThread::EV_SWRESET);
		break;
	case	ID_PAUSE:
		Emu.Event(CEmuThread::EV_EMUPAUSE);
		break;

	case	ID_ONEFRAME:
		Emu.Event(CEmuThread::EV_ONEFRAME);
		break;

	case	ID_THROTTLE:
		Emu.Event(CEmuThread::EV_THROTTLE);
		break;
	case	ID_FRAMESKIP_UP:
		Emu.Event(CEmuThread::EV_FRAMESKIP_UP);
		break;
	case	ID_FRAMESKIP_DOWN:
		Emu.Event(CEmuThread::EV_FRAMESKIP_DOWN);
		break;
	case	ID_FRAMESKIP_AUTO:
		Emu.Event(CEmuThread::EV_FRAMESKIP_AUTO);
		break;

	case	ID_STATE_UP:
	{
		CHAR	szStr[64];
		if (++m_nStateSlot > 10 - 1) {
			m_nStateSlot = 0;
		}
		::wsprintf(szStr, "State Slot #%d", m_nStateSlot);
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)szStr);
	}
	break;
	case	ID_STATE_DOWN:
	{
		CHAR	szStr[64];
		if (--m_nStateSlot < 0) {
			m_nStateSlot = 10 - 1;
		}
		::wsprintf(szStr, "State Slot #%d", m_nStateSlot);
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)szStr);
	}
	break;

	case	ID_STATE_SLOT0:	case	ID_STATE_SLOT1:
	case	ID_STATE_SLOT2:	case	ID_STATE_SLOT3:
	case	ID_STATE_SLOT4:	case	ID_STATE_SLOT5:
	case	ID_STATE_SLOT6:	case	ID_STATE_SLOT7:
	case	ID_STATE_SLOT8:	case	ID_STATE_SLOT9:
		m_nStateSlot = (INT)(uID - ID_STATE_SLOT0);
		{
			CHAR	szStr[64];
			::wsprintf(szStr, "State Slot #%d", m_nStateSlot);
			Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)szStr);
		}
		break;

	case	ID_DISK_EJECT:
	case	ID_DISK_0A: case	ID_DISK_0B:
	case	ID_DISK_1A: case	ID_DISK_1B:
	case	ID_DISK_2A: case	ID_DISK_2B:
	case	ID_DISK_3A: case	ID_DISK_3B:
		Emu.EventParam(CEmuThread::EV_DISK_COMMAND, (LONG)(uID - ID_DISK_EJECT));
		break;

	case	ID_MUTE_0: case	ID_MUTE_1: case	ID_MUTE_2: case	ID_MUTE_3:
	case	ID_MUTE_4: case	ID_MUTE_5: case	ID_MUTE_6: case	ID_MUTE_7:
	case	ID_MUTE_8: case	ID_MUTE_9: case	ID_MUTE_A: case	ID_MUTE_B:
	case	ID_MUTE_C: case	ID_MUTE_D: case	ID_MUTE_E: case	ID_MUTE_F:
		Emu.EventParam(CEmuThread::EV_SOUND_MUTE, (LONG)(uID - ID_MUTE_0));
		break;

	case	ID_EXCTR_NONE:
	case	ID_EXCTR_PADDLE:
	case	ID_EXCTR_HYPERSHOT:
	case	ID_EXCTR_ZAPPER:
	case	ID_EXCTR_KEYBOARD:
	case	ID_EXCTR_CRAZYCLIMBER:
	case	ID_EXCTR_SPACESHADOWGUN:
	case	ID_EXCTR_FAMILYTRAINER_A:
	case	ID_EXCTR_FAMILYTRAINER_B:
	case	ID_EXCTR_MAHJANG:
	case	ID_EXCTR_EXCITINGBOXING:
	case	ID_EXCTR_OEKAKIDS_TABLET:
	case	ID_EXCTR_TURBOFILE:
	case	ID_EXCTR_VSUNISYSTEM:
	case	ID_EXCTR_VSUNISYSTEM_ZAPPER:
		Emu.EventParam(CEmuThread::EV_EXCONTROLLER, uID - ID_EXCTR_NONE);
		break;

	case	ID_TURBOFILE_BANK0:
	case	ID_TURBOFILE_BANK1:
	case	ID_TURBOFILE_BANK2:
	case	ID_TURBOFILE_BANK3:
		Emu.EventParam(CEmuThread::EV_TURBOFILE, uID - ID_TURBOFILE_BANK0);
		break;

	case	ID_SNAPSHOT:
		Emu.Event(CEmuThread::EV_SNAPSHOT);
		break;

	case	ID_TVASPECT:
		Config.graphics.bAspect = !Config.graphics.bAspect;
		Emu.Pause();
		DirectDraw.SetAspectMode(Config.graphics.bAspect);
		if (!Config.general.bScreenMode)
			OnSetWindowSize();
		Emu.Resume();
		break;
	case	ID_SCANLINE:
		Config.graphics.bScanline = !Config.graphics.bScanline;
		Emu.Pause();
		DirectDraw.SetScanlineMode(Config.graphics.bScanline);
		Emu.Resume();
		break;
	case	ID_ALLLINE:
		Config.graphics.bAllLine = !Config.graphics.bAllLine;
		Emu.Pause();
		DirectDraw.SetAllLineMode(Config.graphics.bAllLine);
		if (!Config.general.bScreenMode)
			OnSetWindowSize();
		Emu.Resume();
		break;
	case	ID_ALLSPRITE:
		Config.graphics.bAllSprite = !Config.graphics.bAllSprite;
		break;
	case	ID_SYNCDRAW:
		Config.graphics.bSyncDraw = !Config.graphics.bSyncDraw;
		Emu.Pause();
		DirectDraw.SetFlipMode(Config.graphics.bSyncDraw);
		Emu.Resume();
		break;
	case	ID_FITSCREEN:
		Config.graphics.bFitZoom = !Config.graphics.bFitZoom;
		Emu.Pause();
		DirectDraw.SetMaxZoom(Config.graphics.bFitZoom);
		Emu.Resume();
		break;

	case	ID_TVFRAME:
		Config.graphics.bTVFrame = !Config.graphics.bTVFrame;
		Emu.Pause();
		DirectDraw.SetTVFrameMode(Config.graphics.bTVFrame);
		Emu.Resume();
		break;
	case	ID_FPSDISP:
		Config.graphics.bFPSDisp = !Config.graphics.bFPSDisp;
		break;

	case	ID_LEFTCLIP:
		Config.graphics.bLeftClip = !Config.graphics.bLeftClip;
		break;

	case	ID_FILTER_NONE:
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)"Filter: None");
		goto	_gohell;
	case	ID_FILTER_2XSAI:
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)"Filter: 2xSaI");
		goto	_gohell;
	case	ID_FILTER_SUPER2XSAI:
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)"Filter: Super2xSaI");
		goto	_gohell;
	case	ID_FILTER_SUPEREAGLE:
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)"Filter: SuperEagle");
		goto	_gohell;
	case	ID_FILTER_SCALE2X:
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)"Filter: Scale2x");
		goto	_gohell;
	case	ID_FILTER_HQ2X:
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)"Filter: hq2x");
		goto	_gohell;
	case	ID_FILTER_LQ2X:
		Emu.EventParam(CEmuThread::EV_MESSAGE_OUT, (LONG_PTR)"Filter: lq2x");
		//			goto	_gohell;
	_gohell:
		Config.graphics.nGraphicsFilter = (INT)(uID - ID_FILTER_NONE);
		Emu.Pause();
		DirectDraw.SetGraphicsFilter(Config.graphics.nGraphicsFilter);
		Emu.Resume();
		break;

	case	ID_AUTOIPS:
		Config.emulator.bAutoIPS = !Config.emulator.bAutoIPS;
		break;

	default:
		break;
	}
}

WNDCMD	CMainFrame::OnStateCommand(WNDCMDPARAM)
{
	if (!Emu.IsRunning())
		return;

	CHAR	st[16];
	::wsprintf(st, "st%1X", m_nStateSlot);

	string	pathstr, tempstr;
	if (Config.path.bStatePath) {
		pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szStatePath);
		::CreateDirectory(pathstr.c_str(), NULL);
		DEBUGOUT("Path: %s\n", pathstr.c_str());
	}
	else {
		pathstr = Nes->rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt(pathstr.c_str(), Nes->rom->GetRomName(), st);
	DEBUGOUT("Path: %s\n", tempstr.c_str());

	if (uID == ID_STATE_LOAD) {
		INT	ret;
		if ((ret = NES::IsStateFile(tempstr.c_str(), Nes->rom)) >= 0) {
			if (ret == IDS_ERROR_ILLEGALSTATECRC) {
				if (Config.emulator.bCrcCheck) {
					if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
						return;
				}
			}
		}
		Emu.EventParam2(CEmuThread::EV_STATE_LOAD, (LONG_PTR)tempstr.c_str(), m_nStateSlot);
	}
	if (uID == ID_STATE_SAVE) {
		Emu.EventParam2(CEmuThread::EV_STATE_SAVE, (LONG_PTR)tempstr.c_str(), m_nStateSlot);
	}
}

WNDCMD	CMainFrame::OnStateCommand2(WNDCMDPARAM)
{
	if (!Emu.IsRunning())
		return;

	BOOL	bLoad = FALSE;
	INT	slot = 0;
	if (uID >= ID_QUICKLOAD_SLOT0 && uID <= ID_QUICKLOAD_SLOT9) {
		bLoad = TRUE;
		slot = uID - ID_QUICKLOAD_SLOT0;
	}
	else {
		bLoad = FALSE;
		slot = uID - ID_QUICKSAVE_SLOT0;
	}

	CHAR	st[16];
	::wsprintf(st, "st%1X", slot);

	string	pathstr, tempstr;
	if (Config.path.bStatePath) {
		pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szStatePath);
		::CreateDirectory(pathstr.c_str(), NULL);
		DEBUGOUT("Path: %s\n", pathstr.c_str());
	}
	else {
		pathstr = Nes->rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt(pathstr.c_str(), Nes->rom->GetRomName(), st);
	DEBUGOUT("Path: %s\n", tempstr.c_str());

	if (bLoad) {
		INT	ret;
		if ((ret = NES::IsStateFile(tempstr.c_str(), Nes->rom)) >= 0) {
			if (ret == IDS_ERROR_ILLEGALSTATECRC) {
				if (Config.emulator.bCrcCheck) {
					if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
						return;
				}
			}
		}
		Emu.EventParam2(CEmuThread::EV_STATE_LOAD, (LONG_PTR)tempstr.c_str(), slot);
	}
	if (!bLoad) {
		Emu.EventParam2(CEmuThread::EV_STATE_SAVE, (LONG_PTR)tempstr.c_str(), slot);
	}
}

WNDCMD	CMainFrame::OnMovieCommand(WNDCMDPARAM)
{
	if (!Emu.IsRunning())
		return;

	if (uID == ID_MOVIE_STOP) {
		Emu.Event(CEmuThread::EV_MOVIE_STOP);
		return;
	}

	Emu.Pause();

	if (Config.general.bScreenMode && !m_bMenu) {
		OnFullScreenGDI(TRUE);
	}

	string	pathstr, tempstr;
	if (Config.path.bMoviePath) {
		pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szMoviePath);
		::CreateDirectory(pathstr.c_str(), NULL);
	}
	else {
		pathstr = Nes->rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt(pathstr.c_str(), Nes->rom->GetRomName(), "vmv");
	DEBUGOUT("Path: %s\n", tempstr.c_str());

	OPENFILENAME	ofn;
	CHAR	szFile[_MAX_PATH];

	::strcpy(szFile, tempstr.c_str());
	ZEROMEMORY(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrDefExt = "vmv";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Movie Files(*.vmv)\0*.vmv\0All Files(*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = pathstr.c_str();

	CHAR	szTitle[256];

	if (uID == ID_MOVIE_PLAY) {
		CApp::LoadString(IDS_UI_PLAYMOVIE, szTitle, sizeof(szTitle));
		ofn.lpstrTitle = szTitle;
		if (::GetOpenFileName(&ofn)) {
			INT	ret;
			if ((ret = NES::IsMovieFile(szFile, Nes->rom)) >= 0) {
				if (ret == IDS_ERROR_ILLEGALMOVIEOLD) {
					::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONHAND | MB_OK);
					goto	_Movie_Play_Failed;
				}
				else
					if (ret == IDS_ERROR_ILLEGALMOVIEVER) {
						if (Config.emulator.bCrcCheck) {
							if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
								goto	_Movie_Play_Failed;
						}
					}
					else
						if (ret == IDS_ERROR_ILLEGALMOVIECRC) {
							if (Config.emulator.bCrcCheck) {
								if (::MessageBox(m_hWnd, CApp::GetErrorString(ret), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
									goto	_Movie_Play_Failed;
							}
						}

				Emu.EventParam(CEmuThread::EV_MOVIE_PLAY, (LONG_PTR)szFile);
			}
		_Movie_Play_Failed:;
		}
	}
	if (uID == ID_MOVIE_REC) {
		ofn.Flags |= OFN_OVERWRITEPROMPT;
		CApp::LoadString(IDS_UI_RECMOVIE, szTitle, sizeof(szTitle));
		ofn.lpstrTitle = szTitle;
		if (::GetSaveFileName(&ofn)) {
			Emu.EventParam(CEmuThread::EV_MOVIE_REC, (LONG_PTR)szFile);
		}
	}
	if (uID == ID_MOVIE_REC_APPEND) {
		CApp::LoadString(IDS_UI_APPENDMOVIE, szTitle, sizeof(szTitle));
		ofn.lpstrTitle = szTitle;
		if (::GetOpenFileName(&ofn)) {
			INT	ret;
			if ((ret = NES::IsMovieFile(szFile, Nes->rom)) >= 0) {
				if (ret == IDS_ERROR_ILLEGALMOVIEOLD) {
					::MessageBox(m_hWnd, CApp::GetErrorString(IDS_ERROR_ILLEGALMOVIEOLD_A), "VirtualNES", MB_ICONHAND | MB_OK);
					goto	_Movie_Append_Failed;
				}
				else
					if (ret == IDS_ERROR_ILLEGALMOVIEVER) {
						if (Config.emulator.bCrcCheck) {
							if (::MessageBox(m_hWnd, CApp::GetErrorString(IDS_ERROR_ILLEGALMOVIEVER_A), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
								goto	_Movie_Append_Failed;
						}
					}
					else
						if (ret == IDS_ERROR_ILLEGALMOVIECRC) {
							if (Config.emulator.bCrcCheck) {
								if (::MessageBox(m_hWnd, CApp::GetErrorString(IDS_ERROR_ILLEGALMOVIECRC_A), "VirtualNES", MB_ICONWARNING | MB_YESNO) != IDYES)
									goto	_Movie_Append_Failed;
							}
						}

				Emu.EventParam(CEmuThread::EV_MOVIE_RECAPPEND, (LONG_PTR)szFile);
			}
			else {
				// #### Newly created and the same.
				Emu.EventParam(CEmuThread::EV_MOVIE_REC, (LONG_PTR)szFile);
			}
		}
	_Movie_Append_Failed:;
	}

	if (Config.general.bScreenMode && !m_bMenu) {
		OnFullScreenGDI(FALSE);
	}

	Emu.Resume();
}

void	CMainFrame::OnMovieInfo(WNDCMDPARAM)
{
	if (!Emu.IsRunning() || !Nes)
		return;

	if (!(Nes->IsMoviePlay() || Nes->IsMovieRec()))
		return;

	CMovieInfoDlg dlg;

	// #### Setting Members
	Nes->GetMovieInfo(dlg.m_wRecVersion, dlg.m_wVersion, dlg.m_dwFrames, dlg.m_dwRerecordTimes);

	if (!m_bMenu)
		OnFullScreenGDI(TRUE);
	dlg.DoModal(m_hWnd);
	if (!m_bMenu)
		OnFullScreenGDI(FALSE);
}

void	CMainFrame::OnMovieConv(WNDCMDPARAM)
{
	if (Emu.IsRunning())
		return;

	CAviConvDlg	dlg;

	if (!m_bMenu)
		OnFullScreenGDI(TRUE);

	if (!Config.general.bScreenMode) {
		//		::ShowWindow( m_hWnd, SW_HIDE );
	}

	CCfgSound	ConfigSave = Config.sound;
	dlg.DoModal(m_hWnd);
	Config.sound = ConfigSave;

	if (!Config.general.bScreenMode) {
		//		::ShowWindow( m_hWnd, SW_SHOW );
	}

	if (!m_bMenu)
		OnFullScreenGDI(FALSE);
}

WNDCMD	CMainFrame::OnTapeCommand(WNDCMDPARAM)
{
	if (!Emu.IsRunning())
		return;

	if (uID == ID_TAPE_STOP) {
		Emu.Event(CEmuThread::EV_TAPE_STOP);
		return;
	}

	Emu.Pause();

	if (Config.general.bScreenMode && !m_bMenu) {
		OnFullScreenGDI(TRUE);
	}

	string	pathstr, tempstr;
	if (Config.path.bSavePath) {
		pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szSavePath);
		::CreateDirectory(pathstr.c_str(), NULL);
	}
	else {
		pathstr = Nes->rom->GetRomPath();
	}
	tempstr = CPathlib::MakePathExt(pathstr.c_str(), Nes->rom->GetRomName(), "vtp");
	DEBUGOUT("Path: %s\n", tempstr.c_str());

	OPENFILENAME	ofn;
	CHAR	szFile[_MAX_PATH];

	::strcpy(szFile, tempstr.c_str());
	ZEROMEMORY(&ofn, sizeof(ofn));

	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = m_hWnd;
	ofn.lpstrFile = szFile;
	ofn.lpstrDefExt = "vtp";
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Tape Files(*.vtp)\0*.vtp\0All Files(*.*)\0*.*\0";
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST;
	ofn.lpstrInitialDir = pathstr.c_str();

	CHAR	szTitle[256];

	if (uID == ID_TAPE_PLAY) {
		CApp::LoadString(IDS_UI_PLAYTAPE, szTitle, sizeof(szTitle));
		ofn.lpstrTitle = szTitle;
		if (::GetOpenFileName(&ofn)) {
			Emu.EventParam(CEmuThread::EV_TAPE_PLAY, (LONG_PTR)szFile);
		}
	}
	if (uID == ID_TAPE_REC) {
		ofn.Flags |= OFN_OVERWRITEPROMPT;
		CApp::LoadString(IDS_UI_RECTAPE, szTitle, sizeof(szTitle));
		ofn.lpstrTitle = szTitle;
		if (::GetSaveFileName(&ofn)) {
			Emu.EventParam(CEmuThread::EV_TAPE_REC, (LONG_PTR)szFile);
		}
	}

	if (Config.general.bScreenMode && !m_bMenu) {
		OnFullScreenGDI(FALSE);
	}

	Emu.Resume();
}

void	CMainFrame::OnFullScreenGDI(BOOL bFlag)
{
	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, (INT)bFlag);
		}
	}
}

void	CMainFrame::OnShowCursor(BOOL bFlag)
{
	if (!bFlag) {
		if (m_bCursor) {
			while (::ShowCursor(FALSE) >= 0);
			m_bCursor = FALSE;
		}
	}
	else {
		if (!m_bCursor) {
			while (::ShowCursor(TRUE) < 0);
			m_bCursor = TRUE;
		}
	}
}

void	CMainFrame::OnChangeMenu(BOOL bFlag)
{
	if (Config.general.bScreenMode) {
		if (Emu.IsRunning()) {
			Emu.EventParam(CEmuThread::EV_FULLSCREEN_GDI, (INT)bFlag);
		}
		else {
			DirectDraw.SetFullScreenGDI(bFlag);
		}
	}

	if (m_bMenu) {
		if (!bFlag) {
			::SetMenu(m_hWnd, NULL);
			m_bMenu = FALSE;
		}
	}
	else {
		if (bFlag) {
			::SetMenu(m_hWnd, m_hMenu);
			m_bMenu = TRUE;
			::DrawMenuBar(m_hWnd);
		}
	}

	//	OnShowCursor( m_bMenu );
}

void	CMainFrame::OnSetWindowSize()
{
	// #### Only when in window mode
	if (Config.general.bScreenMode)
		return;

	LONG	width, height;

	if (!Config.graphics.bAspect)  width = CDirectDraw::SCREEN_WIDTH;
	else				width = (LONG)((CDirectDraw::SCREEN_WIDTH) * 1.25);
	if (!Config.graphics.bAllLine) height = CDirectDraw::SCREEN_HEIGHT - 16;
	else				height = CDirectDraw::SCREEN_HEIGHT;

	width *= Config.general.nScreenZoom + 1;
	height *= Config.general.nScreenZoom + 1;

	RECT	rcW, rcC;
	::GetWindowRect(m_hWnd, &rcW);
	::GetClientRect(m_hWnd, &rcC);

	rcW.right += width - (rcC.right - rcC.left);
	rcW.bottom += height - (rcC.bottom - rcC.top);
	::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rcW.left, rcW.top,
		rcW.right - rcW.left, rcW.bottom - rcW.top, SWP_NOZORDER);

	// #### When the menu is folded, it becomes smaller in height.
	::GetClientRect(m_hWnd, &rcC);
	if ((rcC.bottom - rcC.top) != height) {
		::GetWindowRect(m_hWnd, &rcW);
		::GetClientRect(m_hWnd, &rcC);

		rcW.right += width - (rcC.right - rcC.left);
		rcW.bottom += height - (rcC.bottom - rcC.top);
		::SetWindowPos(m_hWnd, HWND_NOTOPMOST, rcW.left, rcW.top,
			rcW.right - rcW.left, rcW.bottom - rcW.top, SWP_NOZORDER);
	}


	// #### When changing the size to the maximum during display, remove the maximum display and redraw the buttons.
	LONG	style = ::GetWindowLong(m_hWnd, GWL_STYLE);
	style &= ~WS_MAXIMIZE;
	::SetWindowLong(m_hWnd, GWL_STYLE, style);
	::RedrawWindow(m_hWnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);

	// #### adjust the position and save
	m_bZoomed = FALSE;
	::GetWindowRect(m_hWnd, &m_WindowRect);
}

void	CMainFrame::OnRebuildMenu()
{
	CHAR	szMenuString[256];
	string	str;
	for (INT i = 0; CConfig::ShortcutKeyID[i * 3 + 0]; i++) {
		::GetMenuString(m_hMenu, CConfig::ShortcutKeyID[i * 3 + 0], szMenuString, 256, MF_BYCOMMAND);

		INT	id, code;
		id = CConfig::ShortcutKeyID[i * 3 + 2];
		code = Config.shortcut.nShortCut[id];
		if (code != 0) {
			string	temp = szMenuString;
			temp = temp + "\t";
			temp = temp + Config.ShortcutToKeyName(code);

			::ModifyMenu(m_hMenu, CConfig::ShortcutKeyID[i * 3 + 0], MF_BYCOMMAND | MF_STRING,
				CConfig::ShortcutKeyID[i * 3 + 0], temp.c_str());
		}
	}
}

void	CMainFrame::OnUpdateMenu(HMENU hMenu, UINT uID)
{
	BOOL	bEnable = FALSE;
	BOOL	bCheck = FALSE;
	BOOL	bEmu = Emu.IsRunning();
	BOOL	bScn = Config.general.bScreenMode;

	switch (uID) {
	case	ID_CLOSE:
	case	ID_ROMINFO:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_NETPLAY_CONNECT:
		if (bEmu)
			bEnable = !(Nes->IsMoviePlay() || Nes->IsMovieRec() || Nes->rom->IsNSF());
		else
			bEnable = FALSE;
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | ((bEnable && bEmu && NetPlay.IsNetPlay() && !NetPlay.IsConnect()) ? MF_ENABLED : MF_GRAYED));
		break;
	case	ID_NETPLAY_DISCONNECT:
	case	ID_NETPLAY_CHAT:
		if (bEmu)
			bEnable = !(Nes->IsMoviePlay() || Nes->IsMovieRec() || Nes->rom->IsNSF());
		else
			bEnable = FALSE;
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | ((bEnable && bEmu && NetPlay.IsNetPlay() && NetPlay.IsConnect()) ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_WAVERECORD:
		bCheck = bEmu && Emu.IsWaveRecord();
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
		break;

	case	ID_HWRESET:
	case	ID_SWRESET:
	case	ID_PAUSE:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;
	case	ID_STATE_LOAD:
	case	ID_STATE_SAVE:
		if (Nes) {
			bEnable = bEmu && !Nes->rom->IsNSF();
		}
		else {
			bEnable = bEmu;
		}
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_STATE_SLOT0:		case	ID_STATE_SLOT1:
	case	ID_STATE_SLOT2:		case	ID_STATE_SLOT3:
	case	ID_STATE_SLOT4:		case	ID_STATE_SLOT5:
	case	ID_STATE_SLOT6:		case	ID_STATE_SLOT7:
	case	ID_STATE_SLOT8:		case	ID_STATE_SLOT9:
	case	ID_QUICKLOAD_SLOT0:	case	ID_QUICKLOAD_SLOT1:
	case	ID_QUICKLOAD_SLOT2:	case	ID_QUICKLOAD_SLOT3:
	case	ID_QUICKLOAD_SLOT4:	case	ID_QUICKLOAD_SLOT5:
	case	ID_QUICKLOAD_SLOT6:	case	ID_QUICKLOAD_SLOT7:
	case	ID_QUICKLOAD_SLOT8:	case	ID_QUICKLOAD_SLOT9:
	case	ID_QUICKSAVE_SLOT0:	case	ID_QUICKSAVE_SLOT1:
	case	ID_QUICKSAVE_SLOT2:	case	ID_QUICKSAVE_SLOT3:
	case	ID_QUICKSAVE_SLOT4:	case	ID_QUICKSAVE_SLOT5:
	case	ID_QUICKSAVE_SLOT6:	case	ID_QUICKSAVE_SLOT7:
	case	ID_QUICKSAVE_SLOT8:	case	ID_QUICKSAVE_SLOT9:
		// #### Date and Time Display (Facerolls).
	{
		CHAR szMenuString[256] = { 0 };
		CHAR* pToken = 0;
		const UCHAR seps[] = " \t\0";	// #### Separator

		// #### Mary Smith.
		for (int i = 0; CConfig::ShortcutKeyID[i * 3 + 0] != uID; i++) {

			::GetMenuString(m_hMenu, CConfig::ShortcutKeyID[i * 3 + 0], szMenuString, 256, MF_BYCOMMAND);

			if ((pToken = (CHAR*)_mbstok((UCHAR*)szMenuString, seps))) {
				string	str = pToken;

				if (Emu.IsRunning() && Nes) {
					CHAR	temp[256];
					if (uID >= ID_STATE_SLOT0 && uID <= ID_STATE_SLOT9)
						::wsprintf(temp, "st%1X", CConfig::ShortcutKeyID[i * 3 + 0] - ID_STATE_SLOT0);
					else if (uID >= ID_QUICKLOAD_SLOT0 && uID <= ID_QUICKLOAD_SLOT9)
						::wsprintf(temp, "st%1X", CConfig::ShortcutKeyID[i * 3 + 0] - ID_QUICKLOAD_SLOT0);
					else
						::wsprintf(temp, "st%1X", CConfig::ShortcutKeyID[i * 3 + 0] - ID_QUICKSAVE_SLOT0);

					string	pathstr, tempstr;
					if (Config.path.bStatePath) {
						pathstr = CPathlib::CreatePath(CApp::GetModulePath(), Config.path.szStatePath);
						::CreateDirectory(pathstr.c_str(), NULL);
					}
					else {
						pathstr = Nes->rom->GetRomPath();
					}
					tempstr = CPathlib::MakePathExt(pathstr.c_str(), Nes->rom->GetRomName(), temp);

					HANDLE	hFile = ::CreateFile(tempstr.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					if (hFile != INVALID_HANDLE_VALUE) {
						BY_HANDLE_FILE_INFORMATION fi;
						FILETIME	filetime;
						SYSTEMTIME	systime;
						::GetFileInformationByHandle(hFile, &fi);
						::FileTimeToLocalFileTime(&fi.ftLastWriteTime, &filetime);
						::FileTimeToSystemTime(&filetime, &systime);
						::wsprintf(temp, "%04d/%02d/%02d %02d:%02d:%02d", systime.wYear, systime.wMonth, systime.wDay, systime.wHour, systime.wMinute, systime.wSecond);
						::CloseHandle(hFile);

						str = str + " ";
						str = str + temp;
					}
					else {
						str = str + "[N/A]";
					}
					}

				INT	code = Config.shortcut.nShortCut[CConfig::ShortcutKeyID[i * 3 + 2]];
				if (code) {
					str = str + "\t";
					str = str + Config.ShortcutToKeyName(code);
				}
				//NOTICE: should not modify menu
#if 0
				//::ModifyMenu(m_hMenu, CConfig::ShortcutKeyID[i * 3 + 0], MF_BYCOMMAND | MF_STRING,
				//	CConfig::ShortcutKeyID[i * 3 + 0], str.c_str());
#endif
				str.clear();
				}
			}

		if (!(uID >= ID_STATE_SLOT0 && uID <= ID_STATE_SLOT9)) {
			::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		}

		if (uID >= ID_STATE_SLOT0 && uID <= ID_STATE_SLOT9) {
			::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (m_nStateSlot == (INT)(uID - ID_STATE_SLOT0) ? MF_CHECKED : MF_UNCHECKED));
		}
		}
	break;

	case	ID_ZOOMx1: case	ID_ZOOMx2:
	case	ID_ZOOMx3: case	ID_ZOOMx4:
		if (bScn) ::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | MF_GRAYED);
		else	   ::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | MF_ENABLED);
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (Config.general.nScreenZoom == (uID - ID_ZOOMx1) ? MF_CHECKED : MF_UNCHECKED));
		break;

	case	ID_FILTER_NONE:
	case	ID_FILTER_2XSAI:
	case	ID_FILTER_SUPER2XSAI:
	case	ID_FILTER_SUPEREAGLE:
	case	ID_FILTER_SCALE2X:
	case	ID_FILTER_HQ2X:
	case	ID_FILTER_LQ2X:
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (Config.graphics.nGraphicsFilter == (uID - ID_FILTER_NONE) ? MF_CHECKED : MF_UNCHECKED));
		break;

	case	ID_DISK_EJECT:
	case	ID_DISK_0A: case	ID_DISK_0B:
	case	ID_DISK_1A: case	ID_DISK_1B:
	case	ID_DISK_2A: case	ID_DISK_2B:
	case	ID_DISK_3A: case	ID_DISK_3B:
		if (Nes) {
			INT no = Nes->GetDiskNo();
			bEnable = bEmu && !(Nes->rom->GetMapperNo() != 20 || Nes->rom->IsNSF() || (!no || (INT)(uID - ID_DISK_EJECT) > no));
		}
		else {
			bEnable = bEmu;
		}
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_MOVIE_PLAY:
	case	ID_MOVIE_REC:
	case	ID_MOVIE_REC_APPEND:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | ((bEmu && !NetPlay.IsConnect()) ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_MOVIE_STOP:
	case	ID_MOVIE_INFO:
		if (Nes) {
			bEnable = Nes->IsMoviePlay() || Nes->IsMovieRec();
		}
		else {
			bEnable = bEmu;
		}
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_MOVIE_CONVERT:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (!bEmu ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_TAPE_PLAY:
	case	ID_TAPE_REC:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | ((bEmu && !NetPlay.IsConnect()) ? MF_ENABLED : MF_GRAYED));
		break;
	case	ID_TAPE_STOP:
		if (Nes) {
			bEnable = Nes->IsTapePlay() || Nes->IsTapeRec();
		}
		else {
			bEnable = bEmu;
		}
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_BARCODEBATTLER:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | ((bEmu && !NetPlay.IsConnect()) ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_VSUNISYSTEM_DIPSWITCH:
		bEnable = bEmu &&
			((Nes->pad->GetExController() == PAD::EXCONTROLLER_VSUNISYSTEM)
				|| (Nes->pad->GetExController() == PAD::EXCONTROLLER_VSZAPPER));
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | ((bEmu && !NetPlay.IsConnect()) ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_SNAPSHOT:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_EXCTR_NONE:
	case	ID_EXCTR_PADDLE:
	case	ID_EXCTR_HYPERSHOT:
	case	ID_EXCTR_ZAPPER:
	case	ID_EXCTR_KEYBOARD:
	case	ID_EXCTR_CRAZYCLIMBER:
	case	ID_EXCTR_TOPRIDER:
	case	ID_EXCTR_SPACESHADOWGUN:
	case	ID_EXCTR_FAMILYTRAINER_A:
	case	ID_EXCTR_FAMILYTRAINER_B:
	case	ID_EXCTR_EXCITINGBOXING:
	case	ID_EXCTR_MAHJANG:
	case	ID_EXCTR_OEKAKIDS_TABLET:
	case	ID_EXCTR_TURBOFILE:
	case	ID_EXCTR_VSUNISYSTEM:
	case	ID_EXCTR_VSUNISYSTEM_ZAPPER:
		if (Nes) {
			if (Nes->pad->GetExController() == (uID - ID_EXCTR_NONE))
				bCheck = TRUE;
			else
				bCheck = FALSE;
		}
		else {
			if (uID == ID_EXCTR_NONE)
				bCheck = TRUE;
			else
				bCheck = FALSE;
		}
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
		break;

	case	ID_TURBOFILE_BANK0:
	case	ID_TURBOFILE_BANK1:
	case	ID_TURBOFILE_BANK2:
	case	ID_TURBOFILE_BANK3:
		if (Nes) {
			bEnable = bEmu && (Nes->pad->GetExController() == PAD::EXCONTROLLER_TURBOFILE);
			if (bEnable) {
				if (Nes->GetTurboFileBank() == (uID - ID_TURBOFILE_BANK0))
					bCheck = TRUE;
				else
					bCheck = FALSE;
			}
		}
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (bCheck ? MF_CHECKED : MF_UNCHECKED));
		break;

	case	ID_FULLSCREEN:
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (bScn ? MF_CHECKED : MF_UNCHECKED));
		break;
	case	ID_FPSDISP:
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (Config.graphics.bFPSDisp ? MF_CHECKED : MF_UNCHECKED));
		break;
	case	ID_TVFRAME:
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (Config.graphics.bTVFrame ? MF_CHECKED : MF_UNCHECKED));
		break;

	case	ID_CFG_GAMEOPTION:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_SEARCH:
	case	ID_CHEAT:
	case	ID_CHEAT_ENABLE:
	case	ID_CHEAT_DISABLE:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;
#if	0
	case	ID_CHEAT:
		if (m_SearchDlg.m_hWnd)
			bEnable = bEmu && !::IsWindowVisible(m_SearchDlg.m_hWnd);
		else
			bEnable = bEmu;
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEnable ? MF_ENABLED : MF_GRAYED));
		break;
#endif
	case	ID_GENIE:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_AUTOIPS:
		::CheckMenuItem(hMenu, uID, MF_BYCOMMAND | (Config.emulator.bAutoIPS ? MF_CHECKED : MF_UNCHECKED));
		break;

	case	ID_VIEW_PATTERN:
	case	ID_VIEW_NAMETABLE:
	case	ID_VIEW_PALETTE:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;

	case	ID_VIEW_MEMORY:
	case	ID_VIEW_WATCH:
		::EnableMenuItem(hMenu, uID, MF_BYCOMMAND | (bEmu ? MF_ENABLED : MF_GRAYED));
		break;

	default:
		break;
			}
		}
BYTE	CMainFrame::KeyInp[256 + 64 * CDirectInput::DIJOYSTICK_MAX + 0x10000] = { 0 };

void	CMainFrame::OnKeyControl()
{
	// #### to decline
	if ((::timeGetTime() - m_dwKeyTime) < 20)
		return;
	m_dwKeyTime = ::timeGetTime();

	::memset(KeyInp, 0x00, sizeof(KeyInp));

	if (!m_bKeyEnable) {
		// #### In order not to trigger...
		::memset(m_KeyBuf, 0x80, sizeof(m_KeyBuf));
		//DEBUGOUT( "Key Disable\n" );
		return;
	}

	// #### Trigger & Repetition Generation
	BYTE* pSw = (BYTE*)DirectInput.m_Sw;
	BYTE* pOld = (BYTE*)m_KeyBuf;
	BYTE* pCnt = (BYTE*)m_KeyCnt;
	BYTE* pKey = (BYTE*)KeyInp;

	for (INT i = 0; i < 256 + 64 * 8; i++) {
		pKey[i] = (pSw[i] & 0x80) & ~(pOld[i] & 0x80);
		if (pKey[i]) {
			pCnt[i] = 14;
		}
		else if (pCnt[i] != 0 && (pSw[i] & 0x80)) {
			if (--pCnt[i] == 0) {
				pKey[i] = pSw[i] & 0x80;
				pCnt[i] = 5;
			}
			else {
				pKey[i] = 0;
			}
		}
	}
	::memcpy(m_KeyBuf, DirectInput.m_Sw, sizeof(DirectInput.m_Sw));

	//DEBUGOUT( "LMENU:%02X RMENU:%02X ENTER:%02X\n", m_KeyBuf[DIK_LMENU], m_KeyBuf[DIK_RMENU], KeyInp[DIK_RETURN] );

		// #### Event trigger
	BOOL	bAltOnly = FALSE;
	if (Emu.IsRunning()) {
		// #### Old-fashioned keyboard used by families during a specific period.
		if (Emu.GetExController() == PAD::EXCONTROLLER_KEYBOARD ||
			Emu.GetExController() == PAD::EXCONTROLLER_SUPOR_KEYBOARD)
			bAltOnly = TRUE;
		if (Emu.IsPausing())
			bAltOnly = FALSE;
	}

	WORD	keyex = 0;
	if (m_KeyBuf[DIK_LMENU] || m_KeyBuf[DIK_RMENU])
		keyex = CCfgShortCut::K_ALT;
	if (m_KeyBuf[DIK_LCONTROL] || m_KeyBuf[DIK_RCONTROL])
		keyex = CCfgShortCut::K_CTRL;
	if (m_KeyBuf[DIK_LSHIFT] || m_KeyBuf[DIK_RSHIFT])
		keyex = CCfgShortCut::K_SHIFT;

	WORD* pShortCutKey = Config.shortcut.nShortCut;
	INT* pShortCutKeyID = Config.ShortcutKeyID;

	if (bAltOnly) {
		INT	no;
		WORD	key, key2;
		for (INT i = 0; pShortCutKeyID[i * 3 + 0]; i++) {
			no = pShortCutKeyID[i * 3 + 2];
			key = pShortCutKey[no];
			key2 = pShortCutKey[no + 128];

			if ((key & 0xF000) == CCfgShortCut::K_ALT && keyex == CCfgShortCut::K_ALT && KeyInp[key & 0x0FFF] && (key & 0x0FFF)) {
				::PostMessage(CApp::GetHWnd(), WM_COMMAND, (WPARAM)pShortCutKeyID[i * 3 + 0], (LPARAM)0);
			}
			if ((key2 & 0xF000) == CCfgShortCut::K_ALT && keyex == CCfgShortCut::K_ALT && KeyInp[key2 & 0x0FFF] && (key2 & 0x0FFF)) {
				::PostMessage(CApp::GetHWnd(), WM_COMMAND, (WPARAM)pShortCutKeyID[i * 3 + 0], (LPARAM)0);
			}
		}
	}
	else {
		INT	no;
		WORD	key, key2;
		for (INT i = 0; pShortCutKeyID[i * 3 + 0]; i++) {
			no = pShortCutKeyID[i * 3 + 2];
			key = pShortCutKey[no];
			key2 = pShortCutKey[no + 128];
			if ((key & 0xF000) == keyex && KeyInp[key & 0x0FFF] && (key & 0x0FFF)) {
				::PostMessage(CApp::GetHWnd(), WM_COMMAND, (WPARAM)pShortCutKeyID[i * 3 + 0], (LPARAM)0);
			}
			if ((key2 & 0xF000) == keyex && KeyInp[key2 & 0x0FFF] && (key2 & 0x0FFF)) {
				::PostMessage(CApp::GetHWnd(), WM_COMMAND, (WPARAM)pShortCutKeyID[i * 3 + 0], (LPARAM)0);
			}
		}
	}
}

void _cdecl CMainFrame::KeyThreadProc(LPVOID lpParam)
{
	while (!m_bKeyThreadExit) {
		::Sleep(20);

		// #### Do not try to imitate me outside of the simulation.
		if (Emu.IsRunning()) {
			continue;
		}
		else {
			DirectInput.Poll();
			OnKeyControl();
		}
	}
	_endthread();
}
WNDMSG CMainFrame::OnHotKey(WNDMSGPARAM) {
	if (wParam == VIRTUAL_NES_BOSS_KEY_ID)
	{
		if (::IsWindowVisible(this->m_hWnd)) {
			::ShowWindow(this->m_hWnd, SW_HIDE);
		}
		else {
			::ShowWindow(this->m_hWnd, SW_SHOW);
		}
	}
	return TRUE;
}

// #### Message Map
WND_MESSAGE_BEGIN(CMainFrame)
// #### Sure, I'd be happy to help you translate a message from Japanese to English! Can you please provide the Japanese text you would like me to translate?
WND_ON_MESSAGE(WM_CREATE, OnCreate)
WND_ON_MESSAGE(WM_CLOSE, OnClose)
WND_ON_MESSAGE(WM_DESTROY, OnDestroy)
WND_ON_MESSAGE(WM_GETMINMAXINFO, OnGetMinMaxInfo)
WND_ON_MESSAGE(WM_ACTIVATE, OnActivate)
WND_ON_MESSAGE(WM_ACTIVATEAPP, OnActivateApp)
WND_ON_MESSAGE(WM_VNS_SHORTCUTENABLE, OnShortCutEnable)
WND_ON_MESSAGE(WM_ENABLE, OnEnable)
WND_ON_MESSAGE(WM_ENTERMENULOOP, OnEnterMenuLoop)
WND_ON_MESSAGE(WM_EXITMENULOOP, OnExitMenuLoop)
WND_ON_MESSAGE(WM_SETFOCUS, OnSetFocus)
WND_ON_MESSAGE(WM_KILLFOCUS, OnKillFocus)
WND_ON_MESSAGE(WM_INITMENU, OnInitMenu)
WND_ON_MESSAGE(WM_INITMENUPOPUP, OnInitMenuPopup)
WND_ON_MESSAGE(WM_PAINT, OnPaint)
WND_ON_MESSAGE(WM_DISPLAYCHANGE, OnDisplayChange)
WND_ON_MESSAGE(WM_PALETTECHANGED, OnPaletteChanged)
WND_ON_MESSAGE(WM_QUERYNEWPALETTE, OnQueryNewPalette)
WND_ON_MESSAGE(WM_MENUCHAR, OnMenuChar)
WND_ON_MESSAGE(WM_KEYDOWN, OnKeyDown)
WND_ON_MESSAGE(WM_SIZE, OnSize)
WND_ON_MESSAGE(WM_SYSCOMMAND, OnSysCommand)
WND_ON_MESSAGE(WM_EXITSIZEMOVE, OnExitSizeMove)
WND_ON_MESSAGE(WM_HOTKEY, OnHotKey);

WND_ON_MESSAGE(WM_SETCURSOR, OnSetCursor)
WND_ON_MESSAGE(WM_TIMER, OnTimer)

WND_ON_MESSAGE(WM_COPYDATA, OnCopyData)

WND_ON_MESSAGE(WM_VNS_ERRORMSG, OnErrorMessage)

WND_ON_MESSAGE(WM_DROPFILES, OnDropFiles)
WND_ON_MESSAGE(WM_VNS_COMMANDLINE, OnCommandLine)
WND_ON_MESSAGE(WM_VNS_LAUNCHERCMD, OnLauncherCommand)

WND_ON_MESSAGE(WM_NETPLAY, OnNetPlay)
WND_ON_MESSAGE(WM_NETPLAY_CLOSE, OnNetPlayClose)
WND_ON_MESSAGE(WM_NETPLAY_ERROR, OnNetPlayError)
WND_ON_MESSAGE(WM_VNS_CHATPOPUP, OnNetPlayChatPopup)

// #### Command
WND_COMMAND_BEGIN()
WND_ON_COMMAND(ID_EXIT, OnExit)
WND_ON_COMMAND(ID_HTMLHELP, OnHelp)
WND_ON_COMMAND(ID_ABOUT, OnAbout)
WND_ON_COMMAND(ID_OPEN, OnFileOpen)
WND_ON_COMMAND(ID_CLOSE, OnFileClose)
WND_ON_COMMAND(ID_ROMINFO, OnRomInfo)
WND_ON_COMMAND(ID_WAVERECORD, OnWaveRecord)
WND_ON_COMMAND(ID_LAUNCHER, OnLauncher)
WND_ON_COMMAND_RANGE(ID_MRU_PATH0, ID_MRU_PATH9, OnRecentOpenPath)
WND_ON_COMMAND_RANGE(ID_MRU_FILE0, ID_MRU_FILE9, OnRecentOpen)

WND_ON_COMMAND(ID_NETPLAY_CONNECT, OnNetPlayConnect)
WND_ON_COMMAND(ID_NETPLAY_DISCONNECT, OnNetPlayDisconnect)
WND_ON_COMMAND(ID_NETPLAY_CHAT, OnNetPlayChat)

WND_ON_COMMAND(ID_CFG_EMULATOR, OnEmulatorCfg)
WND_ON_COMMAND(ID_CFG_GRAPHICS, OnGraphicsCfg)
WND_ON_COMMAND(ID_CFG_SOUND, OnSoundCfg)
WND_ON_COMMAND(ID_CFG_CONTROLLER, OnControllerCfg)
WND_ON_COMMAND(ID_CFG_SHORTCUT, OnShortcutCfg)
WND_ON_COMMAND(ID_CFG_FOLDER, OnFolderCfg)
WND_ON_COMMAND(ID_CFG_LANGUAGE, OnLanguageCfg)
WND_ON_COMMAND(ID_CFG_MOVIE, OnMovieCfg)
WND_ON_COMMAND(ID_CFG_GAMEOPTION, OnGameOptionCfg)
WND_ON_COMMAND(ID_CFG_JOYAXIS, OnJoyAxisCfg)
WND_ON_COMMAND(ID_CFG_PALETTE, OnPaletteEditCfg)

WND_ON_COMMAND(ID_SEARCH, OnSearch)
WND_ON_COMMAND(ID_CHEAT, OnCheat)
WND_ON_COMMAND_RANGE(ID_CHEAT_ENABLE, ID_CHEAT_DISABLE, OnCheatCommand)
WND_ON_COMMAND(ID_GENIE, OnGenie)

WND_ON_COMMAND(ID_AUTOIPS, OnEmuCommand)

WND_ON_COMMAND(ID_BARCODEBATTLER, OnDatachBacode)
WND_ON_COMMAND(ID_VSUNISYSTEM_DIPSWITCH, OnDipSwitch)

WND_ON_COMMAND(ID_FULLSCREEN, OnFullScreen)
WND_ON_COMMAND_RANGE(ID_ZOOMx1, ID_ZOOMx4, OnZoom)
WND_ON_COMMAND(ID_HWRESET, OnEmuCommand)
WND_ON_COMMAND(ID_SWRESET, OnEmuCommand)
WND_ON_COMMAND(ID_PAUSE, OnEmuCommand)
WND_ON_COMMAND(ID_THROTTLE, OnEmuCommand)
WND_ON_COMMAND(ID_STATE_UP, OnEmuCommand)
WND_ON_COMMAND(ID_STATE_DOWN, OnEmuCommand)
WND_ON_COMMAND(ID_FPSDISP, OnEmuCommand)
WND_ON_COMMAND(ID_TVASPECT, OnEmuCommand)
WND_ON_COMMAND(ID_TVFRAME, OnEmuCommand)
WND_ON_COMMAND(ID_SCANLINE, OnEmuCommand)
WND_ON_COMMAND(ID_ALLLINE, OnEmuCommand)
WND_ON_COMMAND(ID_ALLSPRITE, OnEmuCommand)
WND_ON_COMMAND(ID_SYNCDRAW, OnEmuCommand)
WND_ON_COMMAND(ID_FITSCREEN, OnEmuCommand)
WND_ON_COMMAND(ID_SNAPSHOT, OnEmuCommand)

WND_ON_COMMAND(ID_LEFTCLIP, OnEmuCommand)
WND_ON_COMMAND(ID_ONEFRAME, OnEmuCommand)

WND_ON_COMMAND(ID_STATE_LOAD, OnStateCommand)
WND_ON_COMMAND(ID_STATE_SAVE, OnStateCommand)

WND_ON_COMMAND(ID_MOVIE_PLAY, OnMovieCommand)
WND_ON_COMMAND(ID_MOVIE_REC, OnMovieCommand)
WND_ON_COMMAND(ID_MOVIE_REC_APPEND, OnMovieCommand)
WND_ON_COMMAND(ID_MOVIE_STOP, OnMovieCommand)
WND_ON_COMMAND(ID_MOVIE_INFO, OnMovieInfo)
WND_ON_COMMAND(ID_MOVIE_CONVERT, OnMovieConv)

WND_ON_COMMAND(ID_TAPE_PLAY, OnTapeCommand)
WND_ON_COMMAND(ID_TAPE_REC, OnTapeCommand)
WND_ON_COMMAND(ID_TAPE_STOP, OnTapeCommand)

WND_ON_COMMAND(ID_FRAMESKIP_AUTO, OnEmuCommand)
WND_ON_COMMAND(ID_FRAMESKIP_UP, OnEmuCommand)
WND_ON_COMMAND(ID_FRAMESKIP_DOWN, OnEmuCommand)

// Viewers
WND_ON_COMMAND(ID_VIEW_PATTERN, OnViewCommand)
WND_ON_COMMAND(ID_VIEW_NAMETABLE, OnViewCommand)
WND_ON_COMMAND(ID_VIEW_PALETTE, OnViewCommand)
WND_ON_COMMAND(ID_VIEW_MEMORY, OnViewCommand)

//
WND_ON_COMMAND_RANGE(ID_FILTER_NONE, ID_FILTER_END, OnEmuCommand)

WND_ON_COMMAND_RANGE(ID_STATE_SLOT0, ID_STATE_SLOT9, OnEmuCommand)
WND_ON_COMMAND_RANGE(ID_DISK_EJECT, ID_DISK_3B, OnEmuCommand)
WND_ON_COMMAND_RANGE(ID_EXCTR_NONE, ID_EXCTR_END, OnEmuCommand)
WND_ON_COMMAND_RANGE(ID_MUTE_0, ID_MUTE_F, OnEmuCommand)
WND_ON_COMMAND_RANGE(ID_TURBOFILE_BANK0, ID_TURBOFILE_BANK3, OnEmuCommand)

WND_ON_COMMAND_RANGE(ID_QUICKLOAD_SLOT0, ID_QUICKLOAD_SLOT9, OnStateCommand2)
WND_ON_COMMAND_RANGE(ID_QUICKSAVE_SLOT0, ID_QUICKSAVE_SLOT9, OnStateCommand2)

WND_COMMAND_END()
WND_MESSAGE_END()

