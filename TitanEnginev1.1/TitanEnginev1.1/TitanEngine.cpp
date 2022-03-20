#include "stdafx.h"
#include "TitanEngine.h"
#include "Win32Window.h"



TitanEngine* TitanEngine::s_engine = new TitanEngine;


TitanEngine::TitanEngine()
{
	isRunning = false;
	ResourceMgr = std::make_shared<ResourceManager>();
	SceneIns = new Scene;
}

TitanEngine::~TitanEngine()
{
	delete SceneIns;
	SceneIns = nullptr;
}

bool TitanEngine::Init()
{

	WindowIns = Window::Create();
	WindowIns->initWindow();

	RendererIns = new Renderer;
	RendererIns->Init();
	return true;
}

bool TitanEngine::Run()
{
	isRunning = true;
	mTimer.Reset();

	RendererIns->UpdateScene();

	while (isRunning && WindowIns->Run())
	{
		mTimer.Tick();
		SceneIns->camera.Update();
		RenderTick();
	}
	return false;
}

void TitanEngine::Destroy()
{
	delete WindowIns;
	WindowIns = nullptr;
	delete RendererIns;
	RendererIns = nullptr;
}

void TitanEngine::RenderTick()
{
	RendererIns->Run();

}


std::shared_ptr<ResourceManager> TitanEngine::GetResourceMgr()
{
	return ResourceMgr;
}

Window* TitanEngine::GetWindowIns()
{
	return WindowIns;
}

Scene* TitanEngine::GetSceneIns()
{
	return SceneIns;
}

Renderer* TitanEngine::GetRenderer()
{
	return RendererIns;
}

GameTimer TitanEngine::GetTimer()
{
	return mTimer;
}
