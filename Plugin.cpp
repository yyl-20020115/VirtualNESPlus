//
// Language plugin support
//
#include "VirtualNESres.h"

#include "Pathlib.h"
#include "DebugOut.h"

#include "Plugin.h"

#pragma	comment(lib, "version.lib")

typedef	INT(__cdecl* VIRTUALNES_GETVLPVERSION)();
typedef	void(__cdecl* VIRTUALNES_GETVLPLANGUAGE)(LPSTR, SIZE_T);
typedef LCID(__cdecl* VIRTUALNES_GETVLPLOCALEID)();

INT	CPlugin::m_PluginID = 0;
INT	CPlugin::m_PluginNum = 0;
CPlugin::PLUGIN	CPlugin::m_Plugin[PLUGIN_MAX];

HINSTANCE CPlugin::LoadPlugin()
{
	return	LoadPluginID(m_PluginID);
}

HINSTANCE CPlugin::LoadPluginID(INT nID)
{
	return ::LoadLibrary(m_Plugin[nID].Path);
}

BOOL	CPlugin::FindPlugin(LPCSTR lpModulePath)
{
	m_PluginNum = 0;

	string	Path = CPathlib::MakePath(lpModulePath, "*.dll");

	WIN32_FIND_DATA	find;
	HANDLE hFind = ::FindFirstFile(Path.c_str(), &find);
	if (hFind == INVALID_HANDLE_VALUE)
		return	FALSE;

	HINSTANCE	hDLL;
	VIRTUALNES_GETVLPVERSION	GetVlpVersion = NULL;
	VIRTUALNES_GETVLPLANGUAGE	GetVlpLanguage = NULL;
	VIRTUALNES_GETVLPLOCALEID	GetVlpLocaleID = NULL;

	BOOL	bFind = TRUE;
	while (bFind) {
		DWORD	dwHandle;
		DWORD	dwSize = ::GetFileVersionInfoSize(find.cFileName, &dwHandle);

		if (dwSize && (hDLL = ::LoadLibrary(find.cFileName))) {
			GetVlpVersion = (VIRTUALNES_GETVLPVERSION) ::GetProcAddress(hDLL, "VirtualNES_GetVlpVersion");
			GetVlpLanguage = (VIRTUALNES_GETVLPLANGUAGE)::GetProcAddress(hDLL, "VirtualNES_GetVlpLanguage");
			GetVlpLocaleID = (VIRTUALNES_GETVLPLOCALEID)::GetProcAddress(hDLL, "VirtualNES_GetVlpLocaleID");

			if (!(!GetVlpVersion || !GetVlpLanguage || !GetVlpLocaleID)) {
				INT	ver = GetVlpVersion();

				if (ver >= VIRTUALNES_PLUGIN_VERSION) {
					m_Plugin[m_PluginNum].LocaleID = GetVlpLocaleID();
					string	Path = CPathlib::MakePath(lpModulePath, find.cFileName);
					::strcpy(m_Plugin[m_PluginNum].Path, Path.c_str());
					GetVlpLanguage(m_Plugin[m_PluginNum].Language, sizeof(m_Plugin[m_PluginNum].Language));
					m_PluginNum++;
				}
			}

			::FreeLibrary(hDLL);
		}

		bFind = ::FindNextFile(hFind, &find);

		if (m_PluginNum >= PLUGIN_MAX)
			break;
	}

	::FindClose(hFind);

	LCID	lcid = ::GetSystemDefaultLCID();
	for (INT no = 0; no < m_PluginNum; no++) {
		if (m_Plugin[no].LocaleID == lcid) {
			m_PluginID = no;
			return	TRUE;
		}
	}
	LCID    ucid = ::GetUserDefaultLCID();
	for (INT no = 0; no < m_PluginNum; no++) {
		if (m_Plugin[no].LocaleID == ucid) {
			m_PluginID = no;
			return	TRUE;
		}
	}

	return	TRUE;
}

