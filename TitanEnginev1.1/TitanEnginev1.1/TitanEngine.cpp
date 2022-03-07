#include "stdafx.h"
#include "TitanEngine.h"
#include "Application.h"
#include "AppDraw.h"




bool TitanEngine::Init(HINSTANCE hInstance)
{
	std::unique_ptr<Application> myApp = std::make_unique<AppDraw>(hInstance);
	if (!myApp->Initialize())
	{
		return 0;
	}
}
