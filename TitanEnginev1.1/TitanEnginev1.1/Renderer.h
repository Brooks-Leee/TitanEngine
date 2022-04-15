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

	void ShadowPass();
	void HDRPass();
	void ExtractHightlightPass();
	void BloomDownL1Pass();
	void BloomDownL2Pass();
	void BloomDownL3Pass();
	void BloomUpL1Pass();
	void BloomUpL2Pass();
	void BloomMergePass();
	void ToneMapPass();




protected:
	FRHI* RHI;
	TSceneRender* sceneRender;
	Primitive* triangle = new Primitive();
	std::unordered_map<std::string, StaticMesh*> MeshBufferMap;
	std::unordered_map<std::string, TRenderTarget*> RenderTargetMap;
};

