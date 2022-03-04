#include "stdafx.h"
#include "BoxApp.h"
//#include "d3dApp.h"
//#include "MathHelper.h"
//#include "UploadBuffer.h"
#include "Scene.h"


#if defined(_PLATFORM_IOS)
int main()
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
		//std::string FilePath = "Assets\\StaticMesh\\1M_Cube.dat";
		////BoxApp theApp(hInstance);
		//std::unique_ptr<StaticMesh> staticMesh = std::make_unique<StaticMesh>();
		//staticMesh->LoadBinaryFile(FilePath);
		//theApp.GetLoadedBinaryFile(staticMesh->GetStaticMesh());
		//theApp.GetLoadedAllActor(AllActor->SceneDataArr);

		std::unique_ptr<Application> myApp = std::make_unique<BoxApp>(hInstance);

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
