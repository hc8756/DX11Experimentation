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
#include "Mesh.h"
#include "Entity.h"
#include "Material.h"
#include "BufferStruct.h"
//For path getting stuff 
#include <string>
#include <sstream>
//To use vectors
#include <vector>

class DX11App
{
public:

	DX11App(
		HINSTANCE hInstance,
		unsigned int wndWidth,	
		unsigned int wndHeight
	);
	~DX11App();
	//Main function
	HRESULT InitWindow();
	HRESULT InitDirectX();
	HRESULT Run();

	//Static stuff required for OS - level message processing
	static LRESULT CALLBACK StaticWindowProc(//message processing function
		HWND hWnd,
		UINT uMsg,
		WPARAM wParam,
		LPARAM lParam
	);
	static DX11App* DX11AppInstance;//reference to self

private:

	//Parameters passed in constructor when called in Main.cpp
	HINSTANCE hInstance;
	unsigned int wndWidth;
	unsigned int wndHeight;

	//Handle to window
	HWND hWnd;

	//Functions called by main functions
	void Update(float deltaTime, float totalTime);
	void Draw(float deltaTime, float totalTime);
	void CreateBasicGeometry();
	void LoadShaders();

	/*Direct3D objects & resources*/
	//They are all pointers because they refer to addresses in GPU memory
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapChain;
	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	//Resources for swap chain
	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> backBufferView;
	//Resource for depth stencil
	Microsoft::WRL::ComPtr<ID3D11Texture2D> depthBufferTexture;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depthStencilView;
	//Shader related data
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixelShader;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertexShader;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;
	//Resource that will hold external data for vertex shader
	Microsoft::WRL::ComPtr<ID3D11Buffer> constantBufferVS;
	//Direct3D device metadata
	D3D_FEATURE_LEVEL dxFeatureLevel;
	
	//Variables related to timer
	double secsInCount;
	float totalTime;
	float deltaTime;
	__int64 startTime;
	__int64 currentTime;
	__int64 prevTime;
	void UpdateTimer();//called once a frame

	//File path getting function
	std::wstring GetExePath(std::wstring relativeFilePath);
	
	bool vsync;

	//Create vector of mesh & entity pointers 
	//These are pointers so that they aren't destroyed when they are out of scope
	std::vector<Mesh*> myMeshes;
	std::vector<Entity*> myEntities;
	std::vector<Material*> myMaterials;
};

