#include "DX11App.h"

DX11App* DX11App::DX11AppInstance = 0;

// For the DirectX Math library
using namespace DirectX;
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
    //Create variable to hold timer frequency
    _int64 freq;
    //This function takes pointer to large int and fills with counts/sec
    QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
    //Inverse is secs/count
    secsInCount = 1.0 / (double)freq;
}

DX11App::~DX11App()
{
    //Delete input manager instance
    delete& Input::GetInstance();
}

HRESULT DX11App::InitWindow()
{
    //Register instance of WNDCLASS
    WNDCLASS wndClass = {}; //zero out
    wndClass.style = CS_HREDRAW | CS_VREDRAW; //redraw at horizontal or vertical adjustment
    wndClass.lpfnWndProc = DX11App::StaticWindowProc;
    wndClass.cbClsExtra = 0;
    wndClass.cbWndExtra = 0;
    wndClass.hInstance = hInstance;
    wndClass.hIcon = LoadIcon(NULL, IDI_APPLICATION); //default
    wndClass.hCursor = LoadCursor(NULL, IDC_ARROW); //default
    wndClass.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wndClass.lpszMenuName = NULL;
    wndClass.lpszClassName = L"DX11AppWindowClass";

    if (!RegisterClass(&wndClass))
    {
        DWORD dwError = GetLastError();
        if (dwError != ERROR_CLASS_ALREADY_EXISTS)
            return HRESULT_FROM_WIN32(dwError);
    }

    /*Create window with CreateWindow function.
    This function will return a HWND, which is a pointer to a window.
    Reference this HWND when you need to reference window*/
    
    //Create rectangle and adjust window to fit in it
    RECT wndRect;
    SetRect(&wndRect, 0, 0, wndWidth, wndHeight);
    AdjustWindowRect(
        &wndRect,
        WS_OVERLAPPEDWINDOW,
        false //no menu
    );

    //Get (x,y) that will position window at center of screen
    RECT screenRect;
    GetClientRect(GetDesktopWindow(), &screenRect);
    int x= (screenRect.right / 2) - (wndRect.right / 2);
    int y = (screenRect.bottom / 2) - (wndRect.bottom / 2);

    //Create the window for our viewport.
    hWnd = CreateWindow(
        wndClass.lpszClassName,
        L"DirectX11 Application",
        WS_OVERLAPPEDWINDOW,
        x, y,
        (wndRect.right - wndRect.left), (wndRect.bottom - wndRect.top), //calculated width and height
        0,
        0, //no menu
        hInstance,
        0 //only one window
    );

    //Check that everything went right
    if (hWnd == NULL)
    {
        DWORD dwError = GetLastError();
        return HRESULT_FROM_WIN32(dwError);
    }

    // The window exists but is not visible yet
    // We need to tell Windows to show it, and how to show it
    ShowWindow(hWnd, SW_SHOW);

    // Initialize the input manager now that we definitely have a window
    Input::GetInstance().Initialize(hWnd);
    return S_OK;
}

HRESULT DX11App::InitDirectX()
{
    //Added for compatibility with Direct2D.
    UINT deviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

    //Swap chain description
    DXGI_SWAP_CHAIN_DESC scDesc = {};
    scDesc.Windowed = TRUE;
    scDesc.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
    scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scDesc.BufferCount = 2;
    scDesc.SampleDesc.Count = 1;
    scDesc.SampleDesc.Quality = 0;
    scDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scDesc.OutputWindow = hWnd;
    
    //Attempt Direct3D initialization & return failure if something goes wrong
    HRESULT hr = S_OK;
    hr = D3D11CreateDeviceAndSwapChain(
        0,							//default values
        D3D_DRIVER_TYPE_HARDWARE,	
        0,							
        deviceFlags,				
        0,							
        0,							
        D3D11_SDK_VERSION,			//up to here
        &scDesc,					//address of swap chain description
        swapChain.GetAddressOf(),	//pointer to swap chain variable
        device.GetAddressOf(),		//pointer to device variable
        &dxFeatureLevel,			
        deviceContext.GetAddressOf());	//pointer to device context variable
    if (FAILED(hr)) return hr;

    /*Above function created Direct3D objects & resources
    among which is the back buffer texture (something to draw pixels onto)
    this function places that texture into our texture variable*/
    swapChain->GetBuffer(
        0,
        __uuidof(ID3D11Texture2D),
        (void**)&backBufferTexture);

    //Next, we use that texture to create a render target view
    if (backBufferTexture != 0)
    {
        device->CreateRenderTargetView(
            backBufferTexture.Get(),
            0,
            backBufferRTV.GetAddressOf());
        backBufferTexture->Release();
    }

    //Description of texture to use for the depth buffer
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

    //Create the depth buffer and its view
    device->CreateTexture2D(&depthStencilDesc, 0, &depthBufferTexture);
    if (depthBufferTexture != 0)
    {
        device->CreateDepthStencilView(
            depthBufferTexture.Get(),
            0,
            depthStencilView.GetAddressOf());
        depthBufferTexture->Release();
    }

    // Bind the views to the pipeline, so rendering properly 
    // uses their underlying textures
    deviceContext->OMSetRenderTargets(
        1,
        backBufferRTV.GetAddressOf(),
        depthStencilView.Get());

    // Lastly, set up a viewport so we render into
    // to correct portion of the window
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)wndWidth;
    viewport.Height = (float)wndHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    deviceContext->RSSetViewports(1, &viewport);

    // Return the "everything is ok" HRESULT value
    return S_OK; 
}

void DX11App::CreateBasicGeometry()
{
    //Create the vertex buffer description
    D3D11_BUFFER_DESC vbd = {}; //the {} initializes struct members to zero 
    //Set relevant buffer description struct members
    vbd.Usage = D3D11_USAGE_IMMUTABLE; //can we change buffer after creation?->no     
    vbd.ByteWidth = sizeof(Vertex) * 3; //size of buffer
    vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER; //tells DirectX buffer type
    //following are just members you zero 
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

    //D3D11_SUBRESOURCE_DATA struct has 1 important member: pSystem, which holds address of data
    D3D11_SUBRESOURCE_DATA initialVertexData = {};
    initialVertexData.pSysMem = vertices;

    //Create the vertex buffer with addresses to vertex buffer description and vertex buffer itself (declared in header)
    device->CreateBuffer(&vbd, &initialVertexData, vertexBuffer.GetAddressOf());//GetAddressOf() is how you get address of pointer

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

//This function was copied and pasted straight from Chris's code 
void DX11App::LoadShaders()
{
    // Blob for reading raw data
    // - This is a simplified way of handling raw data
    ID3DBlob* shaderBlob;

    // Read our compiled vertex shader code into a blob
    // - Essentially just "open the file and plop its contents here"
    D3DReadFileToBlob(
        GetFullPathTo_Wide(L"VertexShader.cso").c_str(), // Using a custom helper for file paths
        &shaderBlob);

    // Create a vertex shader from the information we
    // have read into the blob above
    // - A blob can give a pointer to its contents, and knows its own size
    device->CreateVertexShader(
        shaderBlob->GetBufferPointer(), // Get a pointer to the blob's contents
        shaderBlob->GetBufferSize(),	// How big is that data?
        0,								// No classes in this shader
        vertexShader.GetAddressOf());	// The address of the ID3D11VertexShader*


    // Create an input layout that describes the vertex format
    // used by the vertex shader we're using
    //  - This is used by the pipeline to know how to interpret the raw data
    //     sitting inside a vertex buffer
    //  - Doing this NOW because it requires a vertex shader's byte code to verify against!
    //  - Luckily, we already have that loaded (the blob above)
    D3D11_INPUT_ELEMENT_DESC inputElements[2] = {};

    // Set up the first element - a position, which is 3 float values
    inputElements[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;				// Most formats are described as color channels; really it just means "Three 32-bit floats"
    inputElements[0].SemanticName = "POSITION";							// This is "POSITION" - needs to match the semantics in our vertex shader input!
    inputElements[0].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// How far into the vertex is this?  Assume it's after the previous element

    // Set up the second element - a color, which is 4 more float values
    inputElements[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;			// 4x 32-bit floats
    inputElements[1].SemanticName = "COLOR";							// Match our vertex shader input!
    inputElements[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;	// After the previous element

    // Create the input layout, verifying our description against actual shader code
    device->CreateInputLayout(
        inputElements,					// An array of descriptions
        2,								// How many elements in that array
        shaderBlob->GetBufferPointer(),	// Pointer to the code of a shader that uses this layout
        shaderBlob->GetBufferSize(),	// Size of the shader code that uses this layout
        inputLayout.GetAddressOf());	// Address of the resulting ID3D11InputLayout*



    // Read and create the pixel shader
    //  - Reusing the same blob here, since we're done with the vert shader code
    D3DReadFileToBlob(
        GetFullPathTo_Wide(L"PixelShader.cso").c_str(), // Using a custom helper for file paths
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
    // Background color (Cornflower Blue in this case) for clearing
    const float color[4] = { 0.4f, 0.6f, 0.75f, 0.0f };

    // Clear the render target and depth buffer (erases what's on the screen)
    //  - Do this ONCE PER FRAME
    //  - At the beginning of Draw (before drawing *anything*)
    deviceContext->ClearRenderTargetView(backBufferRTV.Get(), color);
    deviceContext->ClearDepthStencilView(
        depthStencilView.Get(),
        D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
        1.0f,
        0);


    //for (each mesh drawn){
    //Bind resources (e.g buffers) to rendering pipline 
    deviceContext->VSSetShader(vertexShader.Get(), 0, 0);
    deviceContext->PSSetShader(pixelShader.Get(), 0, 0);
    deviceContext->IASetInputLayout(inputLayout.Get());
    UINT stride = sizeof(Vertex);
    UINT offset = 0;
    deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
    deviceContext->IASetIndexBuffer(indexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);

    //Start the rendering pipeline
    deviceContext->DrawIndexed(
        3,//number of indices to use 
        0,    
        0);    
    //}end hypothetical for loop

    //Tell API frame is complete and present 
    swapChain->Present(vsync ? 1 : 0, 0);

    // Due to the usage of a more sophisticated swap chain,
    // the render target must be re-bound after every call to Present()
    deviceContext->OMSetRenderTargets(1, backBufferRTV.GetAddressOf(), depthStencilView.Get());
}

//Game & message loop function
/*From documentation: 
 "...each iteration should choose to process new Windows messages if 
 they are available, and if no messages are in the queue it should 
 render a new frame.*/
HRESULT DX11App::Run()
{
    //set timer variables
    _int64 now;
    QueryPerformanceCounter((LARGE_INTEGER*)&now);
    startTime = now;
    currentTime = now;
    prevTime = now;
    
    //stuff for messages
    bool bGotMsg;
    MSG  msg;
    msg.message = WM_NULL;
    PeekMessage(&msg, NULL, 0U, 0U, PM_NOREMOVE);

    /*Stuff that was originally in game class init function*/
    CreateBasicGeometry();
    LoadShaders();
    deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


    while (WM_QUIT != msg.message)
    {
        // Process window events.
        // Use PeekMessage() so we can use idle time to render the scene. 
        bGotMsg = (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE) != 0);

        if (bGotMsg)
        {
            // Translate and dispatch the message
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


void DX11App::OnResize()
{
    // Release the buffers before resizing the swap chain
    backBufferRTV.Reset();
    depthStencilView.Reset();

    // Resize the underlying swap chain buffers
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
            backBufferRTV.ReleaseAndGetAddressOf()); // ReleaseAndGetAddressOf() cleans up the old object before giving us the pointer
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
            depthStencilView.ReleaseAndGetAddressOf()); // ReleaseAndGetAddressOf() cleans up the old object before giving us the pointer
        depthBufferTexture->Release();
    }

    // Bind the views to the pipeline, so rendering properly 
    // uses their underlying textures
    deviceContext->OMSetRenderTargets(
        1,
        backBufferRTV.GetAddressOf(), // This requires a pointer to a pointer (an array of pointers), so we get the address of the pointer
        depthStencilView.Get());

    // Lastly, set up a viewport so we render into
    // to correct portion of the window
    D3D11_VIEWPORT viewport = {};
    viewport.TopLeftX = 0;
    viewport.TopLeftY = 0;
    viewport.Width = (float)wndWidth;
    viewport.Height = (float)wndHeight;
    viewport.MinDepth = 0.0f;
    viewport.MaxDepth = 1.0f;
    deviceContext->RSSetViewports(1, &viewport);
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

    //save current time to previous time for next frame
    prevTime = currentTime;
}

//Simple message processing function that dsetroys window when user exits out
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

/*Code related to path getting starts here*/
 std::string DX11App::GetExePath()
{
    // Assume the path is just the "current directory" for now
    std::string path = ".\\";

    // Get the real, full path to this executable
    char currentDir[1024] = {};
    GetModuleFileNameA(0, currentDir, 1024);

    // Find the location of the last slash charaacter
    char* lastSlash = strrchr(currentDir, '\\');
    if (lastSlash)
    {
        // End the string at the last slash character, essentially
        // chopping off the exe's file name.  Remember, c-strings
        // are null-terminated, so putting a "zero" character in 
        // there simply denotes the end of the string.
        *lastSlash = 0;

        // Set the remainder as the path
        path = currentDir;
    }

    // Toss back whatever we've found
    return path;
}


// ---------------------------------------------------
//  Same as GetExePath(), except it returns a wide character
//  string, which most of the Windows API requires.
// ---------------------------------------------------
std::wstring DX11App::GetExePath_Wide()
{
    // Grab the path as a standard string
    std::string path = GetExePath();

    // Convert to a wide string
    wchar_t widePath[1024] = {};
    mbstowcs_s(0, widePath, path.c_str(), 1024);

    // Create a wstring for it and return
    return std::wstring(widePath);
}


// ----------------------------------------------------
//  Gets the full path to a given file.  NOTE: This does 
//  NOT "find" the file, it simply concatenates the given
//  relative file path onto the executable's path
// ----------------------------------------------------
std::string DX11App::GetFullPathTo(std::string relativeFilePath)
{
    return GetExePath() + "\\" + relativeFilePath;
}



// ----------------------------------------------------
//  Same as GetFullPathTo, but with wide char strings.
// 
//  Gets the full path to a given file.  NOTE: This does 
//  NOT "find" the file, it simply concatenates the given
//  relative file path onto the executable's path
// ----------------------------------------------------
std::wstring DX11App::GetFullPathTo_Wide(std::wstring relativeFilePath)
{
    return GetExePath_Wide() + L"\\" + relativeFilePath;
}








