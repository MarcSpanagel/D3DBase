#pragma once
#include "stdafx.h"
#include "resource.h"
#include <d3d10.h>
#include <d3dx10.h>
#include <dinput.h>


#define MAX_LOADSTRING 100

class D3DBase {
public:
	D3DBase(HINSTANCE);
	~D3DBase();
	// Global Variables:
	HINSTANCE hInst;								// current instance
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

	
	bool				InitInstance(HINSTANCE, int);
	INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
	BOOL Init();
	BOOL InitializeDirect3D(HINSTANCE);
	int MessagePump();
	bool CleanUpDevice();
	void UpdateCameraView();
	void GetKeyboardAndMouseInput();

	//virtual
	virtual bool InitScene() = 0;
	virtual void Render();


	HWND hWnd;
	int Width;
	int Height;
	ID3D10Device* d3dDevice;
	IDXGISwapChain* SwapChain;
	ID3D10RenderTargetView* RenderTargetView;
	ID3D10DepthStencilView* DepthStencilView;
	ID3D10Texture2D* DepthStencilBuffer;
	ID3D10Effect* FX;
	ID3D10InputLayout* VertexLayout;
	ID3D10Buffer* VertexBuffer;
	ID3D10Buffer* IndexBuffer;
	//FX:
	ID3D10EffectTechnique* Technique;
	ID3D10EffectShaderResourceVariable* fxDiffuseMapVar;
	ID3D10EffectMatrixVariable* fxWVPVar;
	ID3D10EffectVariable* fxLightVar;

	// Views
	D3DXMATRIX WVP;
	D3DXMATRIX World;
	D3DXMATRIX View;
	D3DXMATRIX Projection;
	D3DXMATRIX RotationMatrix;

	D3DXVECTOR3 Position;
	D3DXVECTOR3 Target;
	D3DXVECTOR3 Up;
	D3DXVECTOR3 DefaultForward;
	D3DXVECTOR3 DefaultRight;
	D3DXVECTOR3 Forward;
	D3DXVECTOR3 Right;

	// Mouse and Keyboard
	IDirectInputDevice8* DIKeyboard;
	IDirectInputDevice8* DIMouse;
	DIMOUSESTATE mouseLastState;
	LPDIRECTINPUT8 DirectInput;
	// Rotation an moving of the sphere
	float rotx;
	float rotz;
	float moveUD;
	float moveLR;
	float moveZ;
	// Camera
	float lookBF;
	float lookLR;
	float yaw;
	float pitch;

private:
	void LimitWithinTwoPi(float &r);
};