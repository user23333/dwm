#include "draw.h"
Dwm_Dram* pDwm = NULL;
extern HMODULE g_hModule;


DWORD WINAPI dwmWork(LPVOID lpThreadParameter)
{
	pDwm = new Dwm_Dram();

	BOOL done = FALSE;

	if (!pDwm->InitDwm()) 
	{
		return 0;
	}


	while (!done)
	{

		if (GetAsyncKeyState(VK_END) & 1) {
			done = TRUE;
		}
		if (done) {
			break;
		}

		render();

	}

	delete pDwm;
	pDwm = NULL;
	FreeLibraryAndExitThread(g_hModule, 0);

	return 0;
}


