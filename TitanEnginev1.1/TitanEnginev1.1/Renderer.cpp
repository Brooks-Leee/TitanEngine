#include "stdafx.h"
#include "Renderer.h"
#include "FRHI.h"
#include "TitanEngine.h"
#include "GPUResourceManager.h"
#include "GameTimer.h"
#include "TRenderTargetDX12.h"

Renderer::Renderer()
{
	FRHI::CreateRHI();
	RHI = FRHI::Get();
	
}

Renderer::~Renderer()
{

}

void Renderer::Init()
{
	RHI->InitRHI(TitanEngine::Get()->GetSceneIns());
	RHI->CreateDescriptorHeaps();
}

void Renderer::BeginRender()
{
	auto textures = TitanEngine::Get()->GetResourceMgr()->getTextures();
	auto meshs = TitanEngine::Get()->GetResourceMgr()->getAllMeshData();

	RHI->CreateConstantBuffer();

	for (UINT i = 0; i < textures.size(); i++)
	{
		RHI->CreateTexture(textures[i], i);
	}


	for (auto mesh : meshs)
	{
		auto meshBuffer = RHI->CreateMeshBuffer(mesh.second);
		MeshBufferMap[meshBuffer->GetStaticMesh()->AssetPath] = meshBuffer;
	}


	auto Actors = TitanEngine::Get()->GetSceneIns()->SceneDataArr;
	for (auto actor : Actors)
	{
		actor->MeshBuffer = MeshBufferMap[actor->AssetPath];
	}

	RenderTargetMap["ShadowMap"] = RHI->CreateRenderTarget(DEPTHSTENCIL_BUFFER, 2048, 2048);
	RenderTargetMap["Base"] = RHI->CreateRenderTarget(COMMAND_RENDER_BUFFER, 800, 600);
	RHI->CreateMaterials();


}

void Renderer::Run()
{
	auto Primivitives = TitanEngine::Get()->GetSceneIns()->SceneDataArr;
	TViewPort ShadowViewPort;
	ShadowViewPort.ClientHeight = 2048;
	ShadowViewPort.ClientWidth = 2048;
	ShadowViewPort.Height = 2048;
	ShadowViewPort.Width = 2048;

	RHI->BeginFrame();
	// Shadow Pass
	RHI->SetViewPortAndRects(ShadowViewPort);
	RHI->SetRenderTarget(RenderTargetMap["ShadowMap"]);
	RHI->SetPipelineState("opaque_shadow");

	for (auto primitive : Primivitives)
	{
		RHI->UpdateObjectCB(primitive, TitanEngine::Get()->GetTimer());
		RHI->UpdateMaterialCB();
		RHI->UpdateShadowPass(TitanEngine::Get()->GetTimer());
		RHI->SetMeshBuffer(primitive);
		RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RHI->SetShaderData(primitive, RenderTargetMap["ShadowMap"]);
		RHI->DrawActor(primitive);
	}
	RHI->ChangeResourceState(RenderTargetMap["ShadowMap"], RESOURCE_STATE_DEPTH_WRITE, RESOURCE_STATE_GENERIC_READ);
	//RHI->EndDraw();

	// Main Pass
	//RHI->BeginFrame();
	TViewPort MainViewPort;
	RHI->SetViewPortAndRects(ShadowViewPort);
	RHI->SetRenderTarget(RenderTargetMap["Base"]);
	RHI->SetPipelineState("opaque");

	for (auto primitive : Primivitives)
	{
		RHI->UpdateObjectCB(primitive, TitanEngine::Get()->GetTimer());
		RHI->UpdateMaterialCB();
		RHI->UpdateShadowPass(TitanEngine::Get()->GetTimer());
		RHI->SetMeshBuffer(primitive);
		RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RHI->SetShaderData(primitive, RenderTargetMap["Base"]);
		RHI->DrawActor(primitive);
	}
	RHI->ChangeResourceState(RenderTargetMap["Base"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT);
	RHI->EndFrame(RenderTargetMap["Base"]);

}

void Renderer::UpdateScene()
{



}


