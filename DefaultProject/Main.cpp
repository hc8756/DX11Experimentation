//Microsoft documentation: https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
//Make sure that linker>system>subsystem is set to WINDOWS not CONSOLE 
#ifndef UNICODE
#define UNICODE
#endif 
#include "DX11App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	//Detect memory leaks
	//Documentation: https://docs.microsoft.com/en-us/visualstudio/debugger/finding-memory-leaks-using-the-crt-library?view=vs-2022
	#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif
	//Create the DX11APP object
	DX11App dxGame(hInstance,1280,720);

	HRESULT hr = S_OK;
	hr = dxGame.InitWindow();
	if (FAILED(hr)) return hr;

	hr = dxGame.InitDirectX();
	if (FAILED(hr)) return hr;

	return dxGame.Run();
}

