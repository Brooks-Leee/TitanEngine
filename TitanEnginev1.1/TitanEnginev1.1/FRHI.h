#pragma once
#include "Scene.h"
#include "TTexTure.h"
#include "TRHITexture.h"


class FRHI
{
public:
	static FRHI* Get();
	static void CreateRHI();
	static void DestoryRHI();
	 


	virtual void InitRHI(Scene* scene) = 0;
	virtual void CreateMeshBuffer(FMeshData* meshData) = 0;
	virtual void CreateConstantBuffer() = 0;
	virtual void CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index) = 0;
	virtual void CreateMaterials() = 0;

	virtual void SetViewPort(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) = 0;
	virtual void SetScissorRects (int ClientWidth, int ClientHeight) = 0;

	virtual void SetMeshBuffer() = 0;
	virtual void UpdateObjectCB(FSceneData actor, GameTimer& gt) = 0;
	virtual void UpdateMaterialCB() = 0;
	virtual void UpdateShadowPass(GameTimer& gt) = 0;

	virtual void SetRenderTarget() = 0;
	virtual void Draw(FSceneData actor) = 0;
	virtual void EndFrame() = 0;

	virtual void SetShadowMapTarget() = 0;
	virtual void DrawShadowMap(FSceneData actor) = 0;
	virtual void EndSHadowMap() = 0;


protected:
	static FRHI* RHI;

};

