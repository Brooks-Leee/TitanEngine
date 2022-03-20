#include "stdafx.h"
#include "Renderer.h"
#include "FRHI.h"
#include "TitanEngine.h"
#include "GPUResourceManager.h"


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
	
	RHI->SetRenderTarget();
	for (auto actor : Actors)
	{
		RHI->UpdateConstantBuffer(actor);
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



}


