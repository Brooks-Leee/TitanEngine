#pragma once
#include "Scene.h"
#include "TTexTure.h"
#include "TRHITexture.h"
#include "StaticMesh.h"
#include "Primitive.h"
#include "TStruct.h"
#include "ShadowMap.h"
#include "TRenderTarget.h"
#include "TSceneRender.h"

class FRHI
{
public:
	static FRHI* Get();
	static void CreateRHI();
	static void DestoryRHI();
	 


	virtual void InitRHI(Scene* scene) = 0;
	virtual ShadowMap* CreateShadowMap() = 0;
	virtual void CreateCbvSrvHeap() = 0;
	virtual StaticMesh* CreateMeshBuffer(FMeshData* meshData) = 0;
	virtual void CreateConstantBuffer() = 0;
	virtual void CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index) = 0;
	virtual void CreateMaterials() = 0;
	virtual TRenderTarget* CreateRenderTarget(RENDERBUFFER_TYPE RTType, int Width, int Height) = 0;
	virtual void EndDraw() = 0;

	virtual void BeginFrame() = 0;
	virtual void UpdateObjectCB(Primitive* primitive, GameTimer& gt) = 0;
	virtual void UpdateMaterialCB() = 0;
	virtual void UpdateShadowPass(TSceneRender* sceneRender) = 0;

	virtual void SetViewPortAndRects(TViewPort& viewport) = 0;
	virtual void SetRenderTarget() = 0;
	virtual void SetRenderTarget(TRenderTarget* renderTarget) = 0;
	virtual void SetPipelineState(std::string pso) = 0;
	virtual void SetShaderData(Primitive* primitive, TRenderTarget* renderTarget) = 0;
	virtual void ChangeResourceState(TRenderTarget* renderTarget, RESOURCE_STATE stateBefore, RESOURCE_STATE stateAfter) = 0;

	virtual void SetPrimitiveTopology(PRIMITIVE_TOPOLOGY primitiveTolology) = 0;
	virtual void SetMeshBuffer(Primitive* primitive) = 0;
	virtual void DrawActor(Primitive* primitive) = 0;


	virtual void Draw(Primitive* primitive) = 0;
	virtual void EndFrame(TRenderTarget* renderTarget) = 0;

	virtual void SetShadowMapTarget() = 0;
	virtual void DrawShadowMap(Primitive* primitive) = 0;


protected:
	static FRHI* RHI;

};

