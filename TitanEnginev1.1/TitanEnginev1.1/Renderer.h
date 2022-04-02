#pragma once
#include "FRHI.h"
#include "TRenderTarget.h"
#include "ShadowMap.h"
#include "TSceneRender.h"

class Renderer
{
public:
	Renderer();
	~Renderer();


public:
	void Init();
	void BeginRender();
	void BuildLight();
	void Run();
	void UpdateScene();

protected:
	FRHI* RHI;
	TSceneRender* sceneRender;
	std::unordered_map<std::string, StaticMesh*> MeshBufferMap;
	std::unordered_map<std::string, TTexTure*> TextureBufferMap;
	std::unordered_map<std::string, TRenderTarget*> RenderTargetMap;
};

