/*Code from Microsoft documentation:
Chapter 4: https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-dxgi
Chapter 5: https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/understand-the-directx-11-2-graphics-pipeline
Chapter 6: https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/work-with-shaders-and-shader-resources
https://docs.microsoft.com/en-us/windows/win32/direct3dgetstarted/complete-code-sample-for-using-a-corewindow-with-directx
And Chris Cascioli's resources from IGME 540 at RIT*/
#include "DX11App.h"
DX11App* DX11App::DX11AppInstance = 0;
using namespace DirectX;//for the DirectX Math library

DX11App::DX11App(HINSTANCE hInstance, unsigned int wndWidth, unsigned int wndHeight)
{
    //Set reference to self
    DX11AppInstance = this;
    //Initialize variables
    this->hInstance = hInstance;
    this->wndWidth = wndWidth;
    this->wndHeight = wndHeight;
    vsync = false;
    //Initialize timer variables 
    totalTime = 0;
    deltaTime = 0;
    startTime = 0;
    currentTime = 0;
    prevTime = 0;
    _int64 freq;//create variable to hold timer frequency
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);//this function takes pointer to large int and fills with counts/sec
    secsInCount = 1.0 / (double)freq;//inverse is secs/count
}

DX11App::~DX11App()
{
    //Delete input manager instance
    delete& Input::GetInstance();
}

HRESULT DX11App::InitWindow()
{
    //Fill out a window class struct
    //Microsoft Documentation: https://docs.microsoft.com/en-us/windows/win32/api/winuser/ns-winuser-wndclassw
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

    //Initialize the input manager
    Input::GetInstance().Initialize(hWnd);
    return S_OK;
}

//Set up swap chain and swap chain buffers (back buffer and depth buffer) & define viewport
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
        0,//default values...
        D3D_DRIVER_TYPE_HARDWARE,	
        0,							
        deviceFlags,				
        0,							
        0,							
        D3D11_SDK_VERSION,//...up to here
        &scDesc,//address of swap chain description
        swapChain.GetAddressOf(),//address of swap chain variable
        device.GetAddressOf(),//address of device variable
        &dxFeatureLevel,			
        deviceContext.GetAddressOf());//address of device context variable
    if (FAILED(hr)) return hr;

    //Above function created Direct3D objects & resources 
    //For example, it creates buffers that can be used as drawing surfaces
    //This function places one of those buffers into our backBufferTexture variable
    swapChain->GetBuffer(
        0,
        //uuid of backBufferTexture data type 
        __uuidof(ID3D11Texture2D),
        (void**)&backBufferTexture);
    
    //We will now need to read our backBufferTexture into the swap chain to be drawn onto
    //To do this, we will need to create a render-target view and place it in our backBufferRTV variable
    //In Direct3D, a view is a way to access a specific resource
    if (backBufferTexture != 0)
    {
        device->CreateRenderTargetView(
            backBufferTexture.Get(),
            0,
            backBufferRTV.GetAddressOf());
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

    //Bind thse views to the pipeline
    deviceContext->OMSetRenderTargets(
        1,
        backBufferRTV.GetAddressOf(),
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

void DX11App::OnResize()
{
    //Get rid of the views we created in previous function
    backBufferRTV.Reset();
    depthStencilView.Reset();

    //Resize swap chain buffers & repeat the process of creating views + binding them to pipeline
    swapChain->ResizeBuffers(
        2,
        wndWidth,
        wndHeight,
        DXGI_FORMAT_R8G8B8A8_UNORM,
        0);

    swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBufferTexture.GetAddressOf()));
    if (backBufferTexture != 0)
    {
        device->CreateRenderTargetView(
            backBufferTexture.Get(),
            0,
            backBufferRTV.ReleaseAndGetAddressOf());//ReleaseAndGetAddressOf() cleans up the old object before giving us the pointer
        backBufferTexture->Release();
    }

    D3D11_TEXTURE2D_DESC depthStencilDesc;
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

    device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
    if (depthBufferTexture != 0)
    {
        device->CreateDepthStencilView(
            depthBufferTexture.Get(),
            0,
            depthStencilView.ReleaseAndGetAddressOf()); 
        depthBufferTexture->Release();
    }

    deviceContext->OMSetRenderTargets(
        1,
        backBufferRTV.GetAddressOf(), 
        depthStencilView.Get());

    //Re-set viewport w/ new dimensions
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)wndWidth;
    viewport.Height = (float)wndHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    deviceContext->RSSetViewports(1, &viewport);
}

void DX11App::CreateBasicGeometry()
{
    //Create the vertex buffer description
    D3D11_BUFFER_DESC vbd = {};  
    //Set relevant buffer description struct members
    vbd.Usage = D3D11_USAGE_IMMUTABLE;//can we change buffer after creation?->no     
    vbd.ByteWidth = sizeof(Vertex) * 3;//size of buffer
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;//tells DirectX buffer type
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    vbd.StructureByteStride = 0;

    //Create data for vertex buffer
    XMFLOAT4 red = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
    XMFLOAT4 green = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
    XMFLOAT4 blue = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
    Vertex vertices[] =
    {
        { XMFLOAT3(+0.0f, +0.5f, +0.0f), red },
        { XMFLOAT3(+0.5f, -0.5f, +0.0f), blue },
        { XMFLOAT3(-0.5f, -0.5f, +0.0f), green },
    };

    //D3D11_SUBRESOURCE_DATA struct has 1 important member: pSystem, which holds data
    D3D11_SUBRESOURCE_DATA initialVertexData = {};
    initialVertexData.pSysMem = vertices;

    //Create the vertex buffer
    device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());

    //Then do the same thing for index buffer 
    D3D11_BUFFER_DESC ibd = {};
    ibd.Usage = D3D11_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(unsigned int) * 3;	
    ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;	
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    ibd.StructureByteStride = 0;
    unsigned int indices[] = { 0, 1, 2 };
    D3D11_SUBRESOURCE_DATA initialIndexData = {};
    initialIndexData.pSysMem = indices;
    device->CreateBuffer(&ibd, &initialIndexData, indexBuffer.GetAddressOf());
}

void DX11App::LoadShaders()
{
    //Blob that will hold codes of shaders in order (vertex->pixel)
    //Blob is buffer for raw data
    ID3DBlob* shaderBlob;

    //Read vertex shader code into blob
    D3DReadFileToBlob(
        GetExePath(L"VertexShader.cso").c_str(),
        &shaderBlob);

    //Create a vertex shader from information in blob
    device->CreateVertexShader(
        shaderBlob->GetBufferPointer(),//pointer to the blob's contents
        shaderBlob->GetBufferSize(),//size of blob's contents
        0,	
        vertexShader.GetAddressOf());//address of our vertex shader pointer

    /*Create an input layout that informs vertex shader how to read vertex buffer*/
    //You need one input layout per vertex type (not per vertex buffer)
    //In order to create an input layout you must have a loaded vertex shader + array of D3D11_INPUT_ELEMENT_DESC
    //An D3D11_INPUT_ELEMENT_DESC describes the syntax of a single component of vertex in buffer (should match Vertex.h)
    D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

    //Component 1: 3 x 32 bit float position
    inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;//data type documentation: https://docs.microsoft.com/en-us/windows/win32/api/dxgiformat/ne-dxgiformat-dxgi_format			
    inputElements[0].SemanticName = "POSITION";//must match vertex shader input
    inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;

    //Component 2: 4 x 32 bit float color (R,G,B,A)
    inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			
    inputElements[1].SemanticName = "COLOR";//must match vertex shader input
    inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;//this just makes sure each component is read in order w/o offset

    //Create the input layout which will later be bound to the input assembler
    device->CreateInputLayout(
        inputElements,//your array of input elements
        2,//size of that array
        shaderBlob->GetBufferPointer(),//pointer to the code of a shader that uses this layout
        shaderBlob->GetBufferSize(),//size of the shader code 
        inputLayout.GetAddressOf());//address of our input layout pointer

    //Reuse same blob to hold pixel shader code
    D3DReadFileToBlob(
        GetExePath(L"PixelShader.cso").c_str(),
        &shaderBlob);

    device->CreatePixelShader(
        shaderBlob->GetBufferPointer(),
        shaderBlob->GetBufferSize(),
        0,
        pixelShader.GetAddressOf());
}

//For game logic & user input later
void DX11App::Update(float deltaTime, float totalTime)
{
}

//Clear the screen, redraw everything, present to the user
void DX11App::Draw(float deltaTime, float totalTime)
{
    const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };//background color for clearing

    //Clear the render target and depth buffer once per frame before drawing anything
    deviceContext->ClearRenderTargetView(backBufferRTV.Get(), color);
    deviceContext->ClearDepthStencilView(
        depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);

    //for (each mesh drawn){
        /*Bind resources to various parts of the rendering pipline*/
        //Create variables for stride and offset as their addresses need to be accessed 
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
        deviceContext->IASetInputLayout(inputLayout.Get());
        deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
        deviceContext->VSSetShader(vertexShader.Get(), 0, 0);
        deviceContext->PSSetShader(pixelShader.Get(), 0, 0);

        //Start the rendering pipeline
        //There are several other "Draw" functions that can do this
        //But DrawIndexed is the most common- it requires you to have bound a buffer of vertices & their indices to the pipeline
        deviceContext->DrawIndexed(
            3,//number of indices to use 
            0,//this and next param are offset values, both being 0 means you start at first index and then go through each of them in order     
            0);    
    //}end hypothetical for loop

    //Tell API frame is complete and present 
    swapChain->Present(vsync ? 1 : 0, 0);
    //Re-bind render target after every call to Present()
    deviceContext->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

//Game & message loop function
/*From documentation: 
 "...each iteration should choose to process new Windows messages if 
 they are available, and if no messages are in the queue it should 
 render a new frame.*/
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

    //Create resources + set settings for the pipeline
    CreateBasicGeometry();
    LoadShaders();
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

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
            Input::GetInstance().Update();
            Update(deltaTime, totalTime);
            Draw(deltaTime, totalTime);
            Input::GetInstance().EndOfFrame();
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

//Function used to get file paths of shaders so that blob can access them in LoadShaders()
//Returns a wide character string as most of the Windows API requires it
std::wstring DX11App::GetExePath(std::wstring relativeFilePath)
{
    std::string path = "";
    //Get path of this file
    //It should look something like this: C:\Users\username\Desktop\MyDX11Starter\MyDX11Starter
    char currentDir[1024] = {};
    GetModuleFileNameA(0, currentDir, 1024);

    //Find the location of the last slash character as we are assuming that shaders are in same directory as this file
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

    //Return path after adding name of specific file  
    return std::wstring(widePath) + L"\\" + relativeFilePath;
}









