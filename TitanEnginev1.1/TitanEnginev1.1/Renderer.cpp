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
	mRenderTarget = std::make_shared<TRenderTarget>();
}

Renderer::~Renderer()
{
	delete mShadowMap;
	mShadowMap = nullptr;
}

void Renderer::Init()
{
	RHI->InitRHI(TitanEngine::Get()->GetSceneIns());
	mShadowMap = RHI->CreateShadowMap();
	RHI->CreateDescriptorHeaps(mShadowMap);
}

void Renderer::BeginFrame()
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

	RHI->CreateMaterials();


	
}

void Renderer::Run()
{
	auto Actors = TitanEngine::Get()->GetSceneIns()->SceneDataArr;
	TViewPort ShadowViewPort;
	//ShadowViewPort.ClientHeight = 2048;
	//ShadowViewPort.ClientWidth = 2048;
	ShadowViewPort.Height = 2048;
	ShadowViewPort.Width = 2048;
	auto RenderTarget = std::dynamic_pointer_cast<TRenderTargetDX12>(mRenderTarget);
	auto ShadowMap = static_cast<ShadowMapDX12*>(mShadowMap);

	// Shadow Pass
	RHI->SetViewPortAndRects(ShadowViewPort);
	RHI->SetRenderTarget(0, 0, false, ShadowMap->Dsv().ptr);
	RHI->SetPipelineState("opaque_shadow");
	RHI->SetShaderData();

	for (auto actor : Actors)
	{
		RHI->UpdateObjectCB(actor, TitanEngine::Get()->GetTimer());
		RHI->UpdateMaterialCB();
		RHI->UpdateShadowPass(TitanEngine::Get()->GetTimer());
		RHI->SetMeshBuffer(actor);
		RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
	// 
	// 
	// 
	//// shadow pass
	//RHI->SetShadowMapTarget();
	//for (auto actor : Actors)
	//{
	//	RHI->UpdateObjectCB(actor, TitanEngine::Get()->GetTimer());
	//	RHI->UpdateMaterialCB();
	//	RHI->UpdateShadowPass(TitanEngine::Get()->GetTimer());
	//	RHI->DrawShadowMap(actor);
	//}
	//RHI->EndSHadowMap();

	//// main pass
	//RHI->SetRenderTarget();
	//for (auto actor : Actors)
	//{
	//	RHI->UpdateObjectCB(actor, TitanEngine::Get()->GetTimer());
	//	RHI->UpdateMaterialCB();
	//	RHI->Draw(actor);
	//}
	//RHI->EndFrame();
}

void Renderer::UpdateScene()
{



}


