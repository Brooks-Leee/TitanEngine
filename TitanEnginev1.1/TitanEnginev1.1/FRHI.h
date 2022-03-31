#pragma once
#include "Scene.h"
#include "TTexTure.h"
#include "TRHITexture.h"
#include "StaticMesh.h"
#include "Actor.h"
#include "TStruct.h"
#include "ShadowMap.h"

class FRHI
{
public:
	static FRHI* Get();
	static void CreateRHI();
	static void DestoryRHI();
	 


	virtual void InitRHI(Scene* scene) = 0;
	virtual ShadowMap* CreateShadowMap() = 0;
	virtual void CreateDescriptorHeaps(ShadowMap* shadowmap) = 0;
	virtual StaticMesh* CreateMeshBuffer(FMeshData* meshData) = 0;
	virtual void CreateConstantBuffer() = 0;
	virtual void CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index) = 0;
	virtual void CreateMaterials() = 0;

	virtual void BeginFrame() = 0;
	virtual void UpdateObjectCB(Actor* actor, GameTimer& gt) = 0;
	virtual void UpdateMaterialCB() = 0;
	virtual void UpdateShadowPass(GameTimer& gt) = 0;

	virtual void SetViewPortAndRects(TViewPort& viewport) = 0;
	virtual void SetRenderTarget() = 0;
	virtual void SetRenderTarget(int NumRTDescriptors, unsigned __int64 RThandle, bool RTsSingleHandleToDescriptorRange, unsigned __int64 DShandle) = 0;
	virtual void SetPipelineState(std::string pso) = 0;
	virtual void SetShaderData(Actor* actor, ShadowMap* shadowmap) = 0;

	virtual void SetPrimitiveTopology(PRIMITIVE_TOPOLOGY primitiveTolology) = 0;
	virtual void SetMeshBuffer(Actor* actor) = 0;
	virtual void DrawActor(Actor* actor) = 0;


	virtual void Draw(Actor* actor) = 0;
	virtual void EndFrame() = 0;

	virtual void SetShadowMapTarget() = 0;
	virtual void DrawShadowMap(Actor* actor) = 0;
	virtual void EndSHadowMap(ShadowMap* shadowmap) = 0;


protected:
	static FRHI* RHI;

};

