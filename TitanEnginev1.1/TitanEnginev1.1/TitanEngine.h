#pragma once
#include "Window.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "Renderer.h"

class TitanEngine
{
public:
	static TitanEngine* s_engine;


private:
	TitanEngine();
	~TitanEngine();
	TitanEngine(const TitanEngine&);

public:
	static TitanEngine* Get() {
		return s_engine;
	}


public:
	bool Init();
	bool Run();
	void Destroy();

	void RenderTick();

	std::shared_ptr<ResourceManager> GetResourceMgr();
	Window* GetWindowIns();
	Scene* GetSceneIns();
	Renderer* GetRenderer();
	GameTimer GetTimer();

protected:
	Window* WindowIns;
	std::shared_ptr<ResourceManager> ResourceMgr;
	Scene* SceneIns;
	Renderer* RendererIns;
	GameTimer mTimer;

	bool isRunning;
};

