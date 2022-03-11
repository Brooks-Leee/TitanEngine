#include "stdafx.h"
#include "TitanEngine.h"
#include "Application.h"
#include "AppDraw.h"
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

	Renderer = new DXRenderer;
	Renderer->Initialize();
	return true;
}

bool TitanEngine::Run()
{
	isRunning = true;
	mTimer.Reset();

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
	delete Renderer;
	Renderer = nullptr;
}

void TitanEngine::RenderTick()
{
	Renderer->Update(mTimer);
	Renderer->Draw(mTimer);
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

DXRenderer* TitanEngine::GetRenderer()
{
	return Renderer;
}

GameTimer TitanEngine::GetTimer()
{
	return mTimer;
}
