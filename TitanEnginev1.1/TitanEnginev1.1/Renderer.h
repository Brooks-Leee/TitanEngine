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
	void MainPass();
	void ExtractHightlightPass();
	void BloomDownL1Pass();
	void BloomDownL2Pass();
	void BloomDownL3Pass();
	void BloomUpL1Pass();
	void BloomUpL2Pass();

	void BloomDown(TRenderTarget* bloomInput, TRenderTarget* RT, TPipeline* pipeline);
	void BloomUp(TRenderTarget* bloomInput1, TRenderTarget* bloomInput2, TRenderTarget* RT, TPipeline* pipeline);

	void BloomMergePass();
	void ToneMapPass();
	void OutlinePass();
	void MergeOutline();
	void FinalPass();


protected:
	FRHI* RHI;
	TSceneRender* sceneRender;
	Primitive* triangle = new Primitive();
	std::unordered_map<std::string, StaticMesh*> MeshBufferMap;
	std::unordered_map<std::string, TRenderTarget*> RenderTargetMap;
};

