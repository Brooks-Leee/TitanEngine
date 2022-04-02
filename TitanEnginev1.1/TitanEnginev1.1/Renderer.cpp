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
	
	sceneRender = new TSceneRender();
}

Renderer::~Renderer()
{
	delete sceneRender;
	sceneRender = nullptr;
}

void Renderer::Init()
{
	RHI->InitRHI(TitanEngine::Get()->GetSceneIns());
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

void Renderer::BuildLight()
{
	TLight* light = TitanEngine::Get()->GetSceneIns()->light;
	glm::vec3 lightDir = light->LightDirection;
	
	auto gt = TitanEngine::Get()->GetTimer();

	lightDir.y += sin(gt.TotalTime() / 10);

	float Radius = 3000;
	glm::vec3 lightPos = -2.0f * Radius * lightDir;
	glm::mat4x4 lightView = glm::lookAtLH(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	glm::vec3 sphereCenterLS = MathHelper::Vector3TransformCoord(glm::vec3(0.0f, 0.0f, 0.0f), lightView);

	float l = sphereCenterLS.x - Radius;
	float b = sphereCenterLS.y - Radius;
	float n = sphereCenterLS.z - Radius;
	float r = sphereCenterLS.x + Radius;
	float t = sphereCenterLS.y + Radius;
	float f = sphereCenterLS.z + Radius;

	glm::mat4x4 lightProj = glm::orthoLH_ZO(l, r, b, t, n, f);

	glm::mat4 T(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);
	glm::mat4 S = lightProj * lightView;

	light->lightVP = glm::transpose(S);
	light->lightTVP = glm::transpose(T*S);
	
	sceneRender->LightMap["dirLight"] = light;

}

void Renderer::Run()
{
	auto Primivitives = TitanEngine::Get()->GetSceneIns()->SceneDataArr;
	TViewPort ShadowViewPort;
	ShadowViewPort.ClientHeight = 2048;
	ShadowViewPort.ClientWidth = 2048;
	ShadowViewPort.Height = 2048;
	ShadowViewPort.Width = 2048;
	BuildLight();

	RHI->BeginFrame();
	// Shadow Pass
	RHI->SetViewPortAndRects(ShadowViewPort);
	RHI->SetRenderTarget(RenderTargetMap["ShadowMap"]);
	RHI->SetPipelineState("opaque_shadow");

	for (auto primitive : Primivitives)
	{
		RHI->UpdateObjectCB(primitive, TitanEngine::Get()->GetTimer());
		RHI->UpdateMaterialCB();
		RHI->UpdateShadowPass(sceneRender);
		RHI->SetMeshBuffer(primitive);
		RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RHI->SetShaderData(primitive, RenderTargetMap["ShadowMap"]);
		RHI->DrawActor(primitive);
	}
	RHI->ChangeResourceState(RenderTargetMap["ShadowMap"], RESOURCE_STATE_DEPTH_WRITE, RESOURCE_STATE_GENERIC_READ);

	// Main Pass
	TViewPort MainViewPort;
	RHI->SetViewPortAndRects(MainViewPort);
	RHI->SetRenderTarget(RenderTargetMap["Base"]);
	RHI->SetPipelineState("opaque");

	for (auto primitive : Primivitives)
	{
		RHI->UpdateObjectCB(primitive, TitanEngine::Get()->GetTimer());
		RHI->UpdateMaterialCB();
		RHI->UpdateShadowPass(sceneRender);
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


