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
#include "TShader.h"
#include "TPipeline.h"
#include <pix.h>

class FRHI
{
public:
	static FRHI* Get();
	static void CreateRHI();
	static void DestoryRHI();
	 
	

	virtual void InitRHI(Scene* scene) = 0;
	virtual void ResetCommand() = 0;
	virtual void ExecuteCommand() = 0;

	virtual TRenderTarget* CreateRenderTarget(TEX_FORMAT format, int rtvIndex, int srvIndex, int dsvIndex, int Width, int Height) = 0;
	virtual TRenderTarget* CreateRenderTarget(RENDERBUFFER_TYPE RTType, int Width, int Height) = 0;


	virtual void SetRenderTargetbloom(TRenderTarget* rendertarget, std::string event) = 0;
	virtual void SetShaderDatabloom(TRenderTarget* rt1, TRenderTarget* rt2) = 0;



	virtual TShader* CreateShader(std::string shaderName) = 0;
	virtual TPipeline* CreatePipelineState(TShader* shader, std::string shaderName, TEX_FORMAT format) = 0;
	virtual TMaterial* CreateMaterial(std::string name, TShader* shader, TTexTure* texture, int matIndex) = 0;
	

	virtual void CreateCbvSrvHeap() = 0;
	virtual StaticMesh* CreateMeshBuffer(FMeshData* meshData) = 0;
	virtual void CreateConstantBuffer() = 0;
	virtual TTexTure* CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index) = 0;
	virtual void EndDraw() = 0;

	virtual void BeginFrame() = 0;
	virtual void UpdateObjectCB(Primitive* primitive, GameTimer& gt, TSceneRender* sceneRender) = 0;
	virtual void UpdateMaterialCB(Primitive* primitive) = 0;
	virtual void UpdateShadowPass(TSceneRender* sceneRender) = 0;

	virtual void SetViewPortAndRects(TViewPort& viewport) = 0;
	virtual void SetRenderTarget(TRenderTarget* renderTarget, std::string event) = 0;
	virtual void SetPipelineState(TPipeline* pipeline) = 0;
	virtual void SetShaderData(Primitive* primitive, TRenderTarget* renderTarget) = 0;
	virtual void ChangeResourceState(TRenderTarget* renderTarget, RESOURCE_STATE stateBefore, RESOURCE_STATE stateAfter) = 0;

	virtual void SetPrimitiveTopology(PRIMITIVE_TOPOLOGY primitiveTolology) = 0;
	virtual void SetMeshBuffer(Primitive* primitive) = 0;
	virtual void DrawMesh(Primitive* primitive) = 0;


	virtual void EndFrame(TRenderTarget* renderTarget) = 0;
	virtual void DrawShadowMap(Primitive* primitive) = 0;


	virtual void EndEvent() = 0;

protected:
	static FRHI* RHI;

};

