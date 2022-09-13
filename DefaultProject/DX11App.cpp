#include "DX11App.h"

DX11App* DX11App::DX11AppInstance = 0;
using namespace DirectX;

DX11App::DX11App(HINSTANCE hInstance, unsigned int wndWidth, unsigned int wndHeight)
{
    //Set reference to self
    DX11AppInstance = this;
    //Initialize variables w/ parameters
    this->hInstance = hInstance;
    this->wndWidth = wndWidth;
    this->wndHeight = wndHeight;
    //Initialize timer variables 
    totalTime = 0;
    deltaTime = 0;
    startTime = 0;
    currentTime = 0;
    prevTime = 0;
    _int64 freq;//create variable to hold timer frequency
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);//this function takes pointer to large int and fills with counts/sec
    secsInCount = 1.0 / (double)freq;//inverse is secs/count
    //Create a camera
    mainCam = new Camera(XMFLOAT3(0.0f, 2.0f, -15.0f),float(wndWidth/wndHeight), XM_PIDIV4, 0.01f, 100.0f);
}

DX11App::~DX11App()
{
    for(auto e:myMeshes) {
        delete e;
        e = NULL;
    }
    for (auto e : myEntities) {
        delete e;
        e = NULL;
    }
    for (auto e : myMaterials) {
        delete e;
        e = NULL;
    }
    delete pixelShader;
    pixelShader = nullptr;
    delete vertexShader;
    vertexShader = nullptr;
    delete mainCam;
    mainCam = nullptr;
}

HRESULT DX11App::InitWindow()
{
    //Fill out a window class struct
    //Documentation: https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassw
    WNDCLASS wndClass = {};//zero out
    wndClass.style = CS_HREDRAW | CS_VREDRAW;//redraw at horizontal or vertical adjustment
    wndClass.lpfnWndProc = DX11App::StaticWindowProc;//pointer to the message processing function you created
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;//handle to app that contains the message processing function
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION);//default
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);//default
    wndClass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = L"DX11AppWindowClass";

    //Make sure window class has been registered
    if (!RegisterClass(&wndClass))
    {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            return HRESULT_FROM_WIN32(dwError);
    }

    //Create rectangle w/ param values and adjust window setting to fit in it
    RECT wndRect;
    SetRect(&wndRect, 0, 0, wndWidth, wndHeight);
    AdjustWindowRect(
        &wndRect,
        WS_OVERLAPPEDWINDOW,
        false//no menu
    );

    //Get (x,y) that will position window at center of screen
    RECT screenRect;
    GetClientRect(GetDesktopWindow(), &screenRect);
    int x= (screenRect.right / 2) - (wndRect.right / 2);
    int y = (screenRect.bottom / 2) - (wndRect.bottom / 2);

    //Use everything you've done so far in this function to create the window
    //This function will return a HWND, which is a pointer to a window
    //Reference this HWND when you need to reference window
    hWnd = CreateWindow(
        wndClass.lpszClassName,
        L"DirectX11 Application",
        WS_OVERLAPPEDWINDOW,
        x, y,
        (wndRect.right - wndRect.left), (wndRect.bottom - wndRect.top),//calculated width and height
        0,
        0,//no menu
        hInstance,
        0//only one window
    );

    //Check that everything went right
    if (hWnd == NULL)
    {
        DWORD dwError = GetLastError();
        return HRESULT_FROM_WIN32(dwError);
    }

    //Show window we created
    ShowWindow(hWnd, SW_SHOW);
    return S_OK;
}

//Create DirectX resources
HRESULT DX11App::InitDirectX()
{
    //Added for compatibility with Direct2D
    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    //Swap chain description
    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.Windowed = TRUE;
    scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;//this allows buffers to be used as drawing surface 
    scDesc.BufferCount = 2;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scDesc.OutputWindow = hWnd;
    
    //Attempt Direct3D initialization & return failure if something goes wrong
    HRESULT hr = S_OK;
    hr = D3D11CreateDeviceAndSwapChain(
        0,
        D3D_DRIVER_TYPE_HARDWARE,	
        0,							
        deviceFlags,				
        0,							
        0,							
        D3D11_SDK_VERSION,
        &scDesc,//address of swap chain description
        swapChain.GetAddressOf(),//address of swap chain variable
        device.GetAddressOf(),//address of device variable
        &dxFeatureLevel,			
        deviceContext.GetAddressOf());//address of device context variable
    if (FAILED(hr)) return hr;

    //Above function created Direct3D objects & resources, including back buffers
    //This accesses one of the back buffers and places it in our backbuffer (texture) variable 
    swapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (void**)&backBufferTexture);
    
    //In Direct3D, a view is a way to access a buffer
    //Here we create a view for our back buffer
    if (backBufferTexture != 0)
    {
        device->CreateRenderTargetView(
            backBufferTexture.Get(),
            0,
            backBufferView.GetAddressOf());
        backBufferTexture->Release();
    }

    //Description of texture to use for the depth-stencil buffer
    //Must be same size as the render target
    D3D11_TEXTURE2D_DESC depthStencilDesc = {};
    depthStencilDesc.Width = wndWidth;
    depthStencilDesc.Height = wndHeight;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.ArraySize = 1;
    depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
    depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
    depthStencilDesc.CPUAccessFlags = 0;
    depthStencilDesc.MiscFlags = 0;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;

    //Create the depth-stencil buffer texture
    device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
    //Create a depth-stencil view
    if (depthBufferTexture != 0)
    {
       device->CreateDepthStencilView(
            depthBufferTexture.Get(),
            0,
            depthStencilView.GetAddressOf());
        depthBufferTexture->Release();
    }

    //Bind these views to the pipeline
    deviceContext->OMSetRenderTargets(
        1,
        backBufferView.GetAddressOf(),
        depthStencilView.Get());

    //Set up a viewport
    //The viewport is the section of the backbuffer texture shown on screen
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)wndWidth;
    viewport.Height = (float)wndHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    deviceContext->RSSetViewports(1, &viewport);

    return S_OK; 
}

void DX11App::CreateBasicGeometry()
{
    XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    Mesh* cubeMesh = new Mesh(GetPath(L"../Assets/Models/sphere.obj").c_str(), device);
    myMeshes.push_back(cubeMesh);
    Material* redMat = new Material(pixelShader,vertexShader,red);
    myMaterials.push_back(redMat);
    Entity* cubeEntity = new Entity(cubeMesh,redMat,mainCam);
    myEntities.push_back(cubeEntity);
}

void DX11App::LoadShaders()
{
    vertexShader = new SimpleVertexShader(device, deviceContext, GetPath(L"VertexShader.cso").c_str());
    pixelShader = new SimplePixelShader(device, deviceContext, GetPath(L"PixelShader.cso").c_str());
    
}

//For game logic & user input later
void DX11App::Update(float deltaTime, float totalTime)
{
    myEntities[0]->GetTransform()->Rotate(0, 0, deltaTime);
}

//Clear the screen, redraw everything, present to the user
void DX11App::Draw(float deltaTime, float totalTime)
{
    const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };//background color for clearing

    //Clear the render target and depth buffer once per frame before drawing anything
    deviceContext->ClearRenderTargetView(backBufferView.Get(), color);
    deviceContext->ClearDepthStencilView(
        depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);

    for(auto e : myEntities) {
        e->Draw(deviceContext);
    }

    //Tell API frame is complete and present 
    swapChain->Present(vsync ? 1 : 0, 0);
    //Re-bind render target after every call to Present()
    deviceContext->OMSetRenderTargets(1, backBufferView.GetAddressOf(), depthStencilView.Get());
}

//Game & message loop function
HRESULT DX11App::Run()
{
    //Set timer variables
    _int64 now;
    QueryPerformanceCounter((LARGE_INTEGER*)&now);
    startTime = now;
    currentTime = now;
    prevTime = now;
    
    //Stuff for messages
    bool bGotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);
    LoadShaders(); CreateBasicGeometry();
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    /*From documentation:
     "...each iteration should choose to process new Windows messages if
     they are available, and if no messages are in the queue it should
     render a new frame.*/
    while (WM_QUIT != msg.message)
    {
        bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);
        if (bGotMsg)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            UpdateTimer();
            Update(deltaTime, totalTime);
            Draw(deltaTime, totalTime);
        }
    }
    return (HRESULT)msg.wParam;
}

void DX11App::UpdateTimer()
{
    //Update current time variable
    __int64 now;
    QueryPerformanceCounter((LARGE_INTEGER*)&now);
    currentTime = now;

    //Calculate time since last frame and total time
    deltaTime =(currentTime - prevTime) * secsInCount;
    totalTime = (currentTime - startTime) * secsInCount;

    //Save current time to previous time for next frame
    prevTime = currentTime;
}

//Simple message processing function that destroys window when user exits out
LRESULT DX11App::StaticWindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_CLOSE:
    {
        DestroyWindow(hWnd);
        return 0;
    }

    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    }
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//Function used to get full path given file path relative to project exe 
std::wstring DX11App::GetPath(std::wstring relativeFilePath)
{
    std::string path = "";
    //Get path of project exe
    char currentDir[1024] = {};
    GetModuleFileNameA(0, currentDir, 1024);

    //Find the location of the last slash character 
    char* lastSlash = strrchr(currentDir, '\\');
    if (lastSlash)
    {
        //Set that char to zero, "cutting off" the rest of the string
        *lastSlash = 0;
        path = currentDir;
    }

    //Convert path to a wide string
    wchar_t widePath[1024] = {};
    mbstowcs_s(0, widePath, path.c_str(), 1024);

    //Return path after adding \parameter  
    return std::wstring(widePath) + L"\\" + relativeFilePath;
}



