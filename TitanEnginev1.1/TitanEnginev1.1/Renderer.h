#pragma once
#include "FRHI.h"
#include "TRenderTarget.h"
#include "ShadowMap.h"

class Renderer
{
public:
	Renderer();
	~Renderer();


public:
	void Init();
	void BeginRender();
	void Run();
	void UpdateScene();

protected:
	FRHI* RHI;
	std::unordered_map<std::string, StaticMesh*> MeshBufferMap;
	std::unordered_map<std::string, TTexTure*> TextureBufferMap;
	std::shared_ptr<TRenderTarget> mRenderTarget;
	ShadowMap* mShadowMap;
};

