#include "StdAfx.h"
#include "ApplicationEntry.h"
#include "D3DBase.h"


int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);
	ApplicationEntry app(hInstance);
	app.InitApp();

}
ApplicationEntry::ApplicationEntry(HINSTANCE hInstance) : D3DBase(hInstance)
{

}
ApplicationEntry::~ApplicationEntry(void)
{
}

void ApplicationEntry::InitApp()
{
	D3DBase:Init();
}



