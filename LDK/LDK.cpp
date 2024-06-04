#define	WIN32_LEAN_AND_MEAN
#include <windows.h>
#include "VirtualNESres.h"

BOOL WINAPI DllMain(HINSTANCE hInstanceDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	return TRUE;
}

INT	GetVlpVersion(void)
{
	return VIRTUALNES_PLUGIN_VERSION;
}

void GetVlpLanguage(LPSTR lpLanguege)
{
	strcpy_s(lpLanguege, 256, "English(US)");
}

LCID GetVlpLocaleID(void)
{
	return 0x409;
}

