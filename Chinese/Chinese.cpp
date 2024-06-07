#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "VirtualNESres.h"

BOOL WINAPI DllMain(HINSTANCE hInstanceDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}

INT	VirtualNES_GetVlpVersion(void)
{
	return VIRTUALNES_PLUGIN_VERSION;
}

void VirtualNES_GetVlpLanguage(LPSTR lpLanguege, SIZE_T length)
{
	strncpy(lpLanguege, "Chinese(ZH)", length);
}

LCID VirtualNES_GetVlpLocaleID(void)
{
	return 2052;
}

