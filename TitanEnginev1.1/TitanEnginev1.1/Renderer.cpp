#include "stdafx.h"
#include "Renderer.h"
#include "FRHI.h"
#include "TitanEngine.h"
#include "GPUResourceManager.h"
#include "GameTimer.h"

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
}

void Renderer::Run()
{
	auto Actors = TitanEngine::Get()->GetSceneIns()->SceneDataArr;
	

	// shadow pass
	RHI->SetShadowMapTarget();
	for (auto actor : Actors)
	{
		RHI->UpdateObjectCB(actor, TitanEngine::Get()->GetTimer());
		RHI->UpdateMaterialCB();
		RHI->UpdateShadowPass(TitanEngine::Get()->GetTimer());
		RHI->DrawShadowMap(actor);
	}
	RHI->EndSHadowMap();


	// main pass
	RHI->SetRenderTarget();
	for (auto actor : Actors)
	{
		RHI->UpdateObjectCB(actor, TitanEngine::Get()->GetTimer());
		RHI->UpdateMaterialCB();
		RHI->Draw(actor);
	}
	RHI->EndFrame();
}

void Renderer::UpdateScene()
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
		RHI->CreateMeshBuffer(mesh.second);
	}

	RHI->CreateMaterials();


}


