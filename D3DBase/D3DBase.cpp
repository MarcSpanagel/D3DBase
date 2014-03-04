// D3DBase.cpp : Defines the entry point for the application.
//
#include "StdAfx.h"
#include "D3DBase.h"


D3DBase::D3DBase(HINSTANCE hInstance)
{
	hInst = hInstance;
	Width = 800;
	Height = 600;
	rotx = 0.001f;
	rotz = 0.0000001f;
	moveUD = 0.0f;
	moveLR = 0.0f;
	moveZ = 0.0f;
	lookBF = 0.0001f;
	lookLR = 0.0f;
	yaw = 0.1f;
	pitch = 0.1f;

	Position = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	Target = D3DXVECTOR3( 0.0f, 0.0f, 1.0f);
	Up = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
	DefaultForward = D3DXVECTOR3(0.0f,0.0f,1.0f);
	DefaultRight = D3DXVECTOR3(1.0f,0.0f,0.0f);
	Forward = D3DXVECTOR3(0.0f,0.0f,1.0f);
	Right = D3DXVECTOR3(1.0f,0.0f,0.0f);
	//Input
	DIKeyboard = NULL;
	DIMouse = NULL;
	DirectInput = NULL;
}

D3DBase::~D3DBase()
{
	CleanUpDevice();
}

LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL D3DBase::Init()
{
	
	// Initialize global strings
	LoadString(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInst, IDC_D3DBASE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInst);

	// Perform application initialization:
	if (!InitInstance (hInst, SW_SHOW))
	{
		MessageBox(0, L"CreateWindow FAILED", 0, 0);
		PostQuitMessage(0);
		return false;
	}

	if(!InitializeDirect3D(hInst))
	{
		MessageBox(0, L"Direct3D Initialization - Failed",
			L"Error", MB_OK);
		PostQuitMessage(0);
		return 0;
	}

	if(!InitScene())
	{
		MessageBox(0, L"Scene Initialization - Failed",
			L"Error", MB_OK);
		return 0;
	}

	return 0;
}



BOOL D3DBase::InitializeDirect3D(HINSTANCE hInstance)
{
	D3D10_DRIVER_TYPE driverTypes[] =
	{
		D3D10_DRIVER_TYPE_HARDWARE,
		D3D10_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof( driverTypes ) / sizeof( driverTypes[0] );

	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferDesc.Width = Width;
	scd.BufferDesc.Height = Height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	//no multisampling
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;

	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hWnd;
	scd.Windowed = true;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = 0;

	D3D10CreateDeviceAndSwapChain(0, D3D10_DRIVER_TYPE_HARDWARE, 0, 0, D3D10_SDK_VERSION, &scd, &SwapChain, &d3dDevice);

	ID3D10Texture2D* backBuffer;
	SwapChain->GetBuffer(0, _uuidof(ID3D10Texture2D), reinterpret_cast<void**>(&backBuffer));
	HRESULT hr = d3dDevice->CreateRenderTargetView(backBuffer, 0, &RenderTargetView);
	backBuffer->Release();

	D3D10_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width     = Width;
	depthStencilDesc.Height    = Height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format    = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count   = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage          = D3D10_USAGE_DEFAULT;
	depthStencilDesc.BindFlags      = D3D10_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0; 
	depthStencilDesc.MiscFlags      = 0;

	d3dDevice->CreateTexture2D(&depthStencilDesc, NULL, &DepthStencilBuffer);
	d3dDevice->CreateDepthStencilView(DepthStencilBuffer, NULL, &DepthStencilView);

	d3dDevice->OMSetRenderTargets(1, &RenderTargetView, DepthStencilView);

	// Setup the viewport
	D3D10_VIEWPORT vp;
	vp.Width = Width;
	vp.Height = Height;
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;
	d3dDevice->RSSetViewports( 1, &vp );

	D3DXMatrixIdentity( &World );

	Position = D3DXVECTOR3( 0.0f, 4.0f, -10.0f );
	Target = D3DXVECTOR3( 0.0f, 0.0f, 0.0f );
	Up = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );	
	D3DXMatrixLookAtLH( &View, &Position, &Target, &Up );

	return true;
}


int D3DBase::MessagePump() {
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while(true)
	{
		BOOL PeekMessageL( 
			LPMSG lpMsg,
			HWND hWnd,
			UINT wMsgFilterMin,
			UINT wMsgFilterMax,
			UINT wRemoveMsg
			);

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;
			TranslateMessage(&msg);	
			DispatchMessage(&msg);
		}
		else{
			// run game code
			GetKeyboardAndMouseInput();
			// UpdateCameraView();
            Render();
		}
	}
	
	if(!CleanUpDevice())
	{
		MessageBox(0, L"Object Releasing - Failed",
			L"Error", MB_OK);
		return 0;
	}
    return ( int )msg.wParam;
}

void D3DBase::GetKeyboardAndMouseInput()
{
	float l_delta_mouse = 0.01f;
	float l_deltarot = 0.005f;

	if(GetAsyncKeyState('A') & 0x8000)	lookLR -= l_deltarot;
	if(GetAsyncKeyState('D') & 0x8000)	lookLR += l_deltarot;
	if(GetAsyncKeyState('W') & 0x8000)	lookBF += l_deltarot;
	if(GetAsyncKeyState('S') & 0x8000)	lookBF -= l_deltarot;
	if(GetAsyncKeyState('Y') & 0x8000)	rotx -= l_deltarot;
	if(GetAsyncKeyState('X') & 0x8000)	rotx += l_deltarot;


	LimitWithinTwoPi(lookBF);
	//LimitWithinTwoPi(rotz);

	Position.x =  rotx*sinf(lookBF)*sinf(lookLR);
	Position.z = -rotx*sinf(lookBF)*cosf(lookLR);
	Position.y =  rotx*cosf(lookBF);

	// Build the view matrix.
	//D3DXVECTOR3 Target(0.0f, 0.0f, 0.0f);
	//D3DXVECTOR3 Up(0.0f, 1.0f, 0.0f);
	D3DXMatrixLookAtLH( &View, &Position, &Target, &Up );

	return;
}
void D3DBase::UpdateCameraView()
{
	D3DXMatrixRotationYawPitchRoll( &RotationMatrix, yaw, pitch, 0 );
	D3DXVec3TransformCoord( &Target, &DefaultForward, &RotationMatrix );
	D3DXVec3Normalize( &Target, &Target );

	D3DXMATRIX RotateYTempMatrix;
	D3DXMatrixRotationY(&RotateYTempMatrix, yaw);

	D3DXVec3TransformNormal(&Right, &DefaultRight, &RotateYTempMatrix);
	D3DXVec3TransformNormal(&Up, &Up, &RotateYTempMatrix);
	D3DXVec3TransformNormal(&Forward, &DefaultForward, &RotateYTempMatrix);
	
	Position += lookLR*Right;
	Position += lookBF*Forward;

	lookLR = 0.0f;
	lookBF = 0.0f;
	Target = Position + Target;	

	D3DXMatrixLookAtLH( &View, &Position, &Target, &Up );
}

void D3DBase::LimitWithinTwoPi(float &r) 
{
	float c_2PI = 6.283185f;
	if ( r > c_2PI )
		r -=  c_2PI;
	else if ( r < 0 )
		r =  c_2PI + r;
}

void D3DBase::Render()
{
}

bool D3DBase::CleanUpDevice()
{
	if( d3dDevice ) d3dDevice->ClearState();
	if( RenderTargetView ) RenderTargetView->Release();
	if( SwapChain ) SwapChain->Release();
	if( d3dDevice ) d3dDevice->Release();

	if( VertexBuffer ) VertexBuffer->Release();
	if( IndexBuffer ) IndexBuffer->Release();
	if( VertexLayout ) VertexLayout->Release();
	if( FX ) FX->Release();
	

	/*for(int i = 0; i < meshCount; i++)
		if( meshes[i] ) meshes[i]->Release();*/
	return true;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_D3DBASE));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_D3DBASE);
	wcex.lpszClassName	= L"D3DWindowClass";
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

bool D3DBase::InitInstance(HINSTANCE hInstance, int nCmdShow)
{

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(L"D3DWindowClass", szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}




LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		/*case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;*/
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}
