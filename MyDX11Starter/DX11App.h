//Code from Microsoft documentation: 
//https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-dxgi
//https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/complete-code-sample-for-using-a-corewindow-with-directx
//And Chris Cascioli's DX11Starter code from IGME 540 at RIT

#pragma once
//Include to use DirectX11 API
#include <windows.h>
#include <d3d11.h>
//Include to use smart pointers
#include <wrl/client.h>
//Include to query performance counter
#include <windowsX.h>
//Include to use DirectX Math library
#include <DirectXMath.h>
//Make sure correct library is included in VS settings
#pragma comment(lib,"d3d11.lib")
// Needed for a helper function to read compiled shader files from the hard drive
#pragma comment(lib, "d3dcompiler.lib")
#include <d3dcompiler.h>
//Include other classes
#include "Vertex.h"
#include "Input.h"
//for path getting stuff
#include <string>
#include <sstream>
class DX11App
{
public:

	DX11App(
		HINSTANCE hInstance,
		unsigned int wndWidth,	
		unsigned int wndHeight
	);
	~DX11App();
	HRESULT InitWindow();
	HRESULT InitDirectX();
	HRESULT Run();
	void OnResize();

	/*Static stuff required for OS - level message processing*/
	//Message processing function
	static LRESULT CALLBACK StaticWindowProc(
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
	);
	//Reference to self
	static DX11App* DX11AppInstance;

private:
	/*Parameters passed in constructor*/
	//Handle to application
	HINSTANCE hInstance; 
	unsigned int wndWidth;
	unsigned int wndHeight;
	//Handle to window
	HWND hWnd;

	/*Direct3D objects & resources*/
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	//Resources for swap chain
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferRTV;
	//Resource for depth stencil
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	
	/*Direct3D device metadata and device resource metadata*/
	D3D_FEATURE_LEVEL dxFeatureLevel;
	
	/*Variables related to timer*/
	double secsInCount;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 prevTime;

	//path getting stuff
	std::string GetExePath();
	std::wstring GetExePath_Wide();
	std::string GetFullPathTo(std::string relativeFilePath);
	std::wstring GetFullPathTo_Wide(std::wstring relativeFilePath);
	/*Stuff originally in game class*/
	void Init();
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void UpdateTimer();
	// Initialization helper methods - feel free to customize, combine, etc.
	void LoadShaders();
	void CreateBasicGeometry();
	// Buffers to hold actual geometry data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	// Shaders and shader-related constructs
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	// Should we use vsync to limit the frame rate?
	bool vsync;
};

