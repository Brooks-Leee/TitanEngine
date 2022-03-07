#include "stdafx.h"
#include "AppDraw.h"
#include "Scene.h"
#include "TitanEngine.h"

#if defined(_PLATFORM_IOS)
int main()
{

}
#endif


#if defined(_PLATFORM_WINDOWS)
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
	#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	#endif
	try
	{
		std::unique_ptr<Application> myApp = std::make_unique<AppDraw>(hInstance);

		if (!myApp->Initialize())
			return 0;
		return myApp->Run();
	}
	catch (DxException& e)
	{
		MessageBox(nullptr, e.ToString().c_str(), L"HR Failed", MB_OK);
		return 0;
	}
}

#endif
