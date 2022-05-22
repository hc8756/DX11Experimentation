//Code from Microsoft documentation: https://docs.microsoft.com/en-us/windows/win32/learnwin32/your-first-windows-program
//And Chris Cascioli's resources from IGME 540 at RIT
//Make sure that linker>system>subsystem is set to WINDOWS not CONSOLE 
#ifndef UNICODE
#define UNICODE
#endif 
#include "DX11App.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	/*code in here was all copied & pasted from Chris's starter code, only modification is that
	DX11App is created instead of game*/

	//Adding comment from macbook to see if it runs after pushs
	// Create the Game object using
	 // the app handle we got from WinMain
	DX11App dxGame(hInstance,1280,720);

	// Result variable for function calls below
	HRESULT hr = S_OK;

	// Attempt to create the window for our program, and
	// exit early if something failed
	hr = dxGame.InitWindow();
	if (FAILED(hr)) return hr;

	// Attempt to initialize DirectX, and exit
	// early if something failed
	hr = dxGame.InitDirectX();
	if (FAILED(hr)) return hr;

	// Begin the message and game loop, and then return
	// whatever we get back once the game loop is over
	return dxGame.Run();
}

