#pragma once
#include "stdafx.h"
#include "resource.h"

#define MAX_LOADSTRING 100

class D3DBase {
public:
	D3DBase(HINSTANCE);
	~D3DBase();
	// Global Variables:
	HINSTANCE hInst;								// current instance
	TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
	TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

	// Forward declarations of functions included in this code module:
	
	bool				InitInstance(HINSTANCE, int);
	
	INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
	BOOL Init();
};