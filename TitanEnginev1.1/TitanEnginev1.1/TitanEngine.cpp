#include "stdafx.h"
#include "TitanEngine.h"
#include "Application.h"
#include "AppDraw.h"
#include "Win32Window.h"



TitanEngine* TitanEngine::s_engine = new TitanEngine;


TitanEngine::TitanEngine()
{
	isRunning = false;
}

TitanEngine::~TitanEngine()
{
	Destroy();
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

	if (isRunning && WindowIns->Run())
	{
		mTimer.Tick();
		RenderTick();
	}
	return false;
}

void TitanEngine::Destroy()
{
	delete WindowIns;
	delete Renderer;
}

void TitanEngine::RenderTick()
{
	Renderer->CalculateFrameStats();
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
