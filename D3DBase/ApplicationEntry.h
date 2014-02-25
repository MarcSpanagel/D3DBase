#pragma once
#include "D3DBase.h"
class ApplicationEntry : public D3DBase
{
public:
	ApplicationEntry(HINSTANCE hInstance);
	~ApplicationEntry(void);
	void ApplicationEntry::InitApp();
};

