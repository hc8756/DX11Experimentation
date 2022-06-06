//Code from Microsoft documentation: 
//Chapter 4: https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-dxgi
//Chapter 5: https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/understand-the-directx-11-2-graphics-pipeline
//Chapter 6: https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-shaders-and-shader-resources
//https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/complete-code-sample-for-using-a-corewindow-with-directx
//And Chris Cascioli's resources from IGME 540 at RIT

#pragma once
//Include to use DirectX11 API
#include <windows.h>
#include <d3d11.h>
#include <d3dcompiler.h>
//Include to use smart pointers
#include <wrl/client.h>
//Include to query performance counter
#include <windowsX.h>
//Include to use DirectX Math library
#include <DirectXMath.h>
//Make sure correct libraries are included in VS settings
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

//Include other class headers
#include "Vertex.h"
#include "Input.h"
//For path getting stuff 
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
	/*Main functions*/
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

	/*Functions called by main functions*/
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void CreateBasicGeometry();
	void LoadShaders();

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
	//Buffers to hold mesh data
	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11Buffer> indexBuffer;
	//Shader related data
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	/*Direct3D device metadata*/
	D3D_FEATURE_LEVEL dxFeatureLevel;
	
	/*Related to timer*/
	double secsInCount;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 prevTime;
	void UpdateTimer();//called once a frame

	/*Path getting stuff*/
	std::string GetExePath();
	std::wstring GetExePath_Wide();
	std::string GetFullPathTo(std::string relativeFilePath);
	std::wstring GetFullPathTo_Wide(std::wstring relativeFilePath);
	bool vsync;
};

