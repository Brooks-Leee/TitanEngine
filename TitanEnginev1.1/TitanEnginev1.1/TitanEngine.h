#pragma once
#include "Window.h"
#include "Scene.h"
#include "ResourceManager.h"
#include "DXRenderer.h"


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
	DXRenderer* GetRenderer();

protected:
	Window* WindowIns;
	std::shared_ptr<ResourceManager> ResourceMgr =  std::make_shared<ResourceManager>();
	Scene* SceneIns;
	DXRenderer* Renderer;
	GameTimer mTimer;
	bool isRunning;

};

