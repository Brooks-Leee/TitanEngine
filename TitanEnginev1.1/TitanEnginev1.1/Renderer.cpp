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

	for (auto& mesh: MeshBufferMap)
	{	
		delete mesh.second;
	}

	for (auto& rendertarget : RenderTargetMap)
	{
		delete rendertarget.second;
	}
}

void Renderer::Init()
{
	RHI->InitRHI(TitanEngine::Get()->GetSceneIns());
	


}

void Renderer::BeginRender()
{
	auto textures = TitanEngine::Get()->GetResourceMgr()->getTextures();
	auto meshs = TitanEngine::Get()->GetResourceMgr()->getAllMeshData();

	sceneRender->ShaderMap["color"] = RHI->CreateShader("color");
	sceneRender->ShaderMap["shadow"] = RHI->CreateShader("shadow");
	sceneRender->PipelineMap["opaque"] = RHI->CreatePipelineState(sceneRender->ShaderMap["color"], "color");
	sceneRender->PipelineMap["shadow_opaque"] = RHI->CreatePipelineState(sceneRender->ShaderMap["shadow"], "shadow");
	sceneRender->PipelineMap["hdr"] = RHI->CreatePipelineState(sceneRender->ShaderMap["color"], "hdr");

	RHI->BeginFrame();
	// need to create mesh first, cuz we will count the mesh buff index and followed by texture
	for (auto mesh : meshs)
	{
		auto meshBuffer = RHI->CreateMeshBuffer(mesh.second);
		MeshBufferMap[meshBuffer->GetStaticMesh()->AssetPath] = meshBuffer;
	}

	for (UINT i = 0; i < textures.size(); i++)
	{
		TTexTure* tex = RHI->CreateTexture(textures[i], i);
		sceneRender->TextureBufferMap[tex->Name] = tex;    
	}

	auto Primitives = TitanEngine::Get()->GetSceneIns()->SceneDataArr;

	sceneRender->MaterialMap["brick"] = RHI->CreateMaterial("brick", sceneRender->ShaderMap["shadow"], sceneRender->TextureBufferMap["brickTex"], 0);
	sceneRender->MaterialMap["brick"]->addTexture(sceneRender->TextureBufferMap["brickNormalTex"]);

	for (auto actor : Primitives)
	{
		actor->MeshBuffer = MeshBufferMap[actor->AssetPath];
		actor->Material = sceneRender->MaterialMap["brick"];
	}

	RenderTargetMap["ShadowMap"] = RHI->CreateRenderTarget(DEPTHSTENCIL_BUFFER, 2048, 2048);
	RenderTargetMap["Base"] = RHI->CreateRenderTarget(COMMAND_RENDER_BUFFER, 800, 600);
	RenderTargetMap["hdr"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 800, 600);
	RHI->CreateConstantBuffer();
	RHI->ExecuteCommand();


}

void Renderer::BuildLight()
{
	TLight* light = TitanEngine::Get()->GetSceneIns()->light;
	//glm::vec3 lightDir = light->LightDirection;
	
	auto gt = TitanEngine::Get()->GetTimer();

	//light->LightDirection.y += sin(gt.TotalTime() / 10);

	//OutputDebugStringA()

	float Radius = 3000;
	glm::vec3 lightPos = -2.0f * Radius * light->LightDirection;
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
	//light->LightDirection = lightDir;

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
	RHI->SetPipelineState(sceneRender->PipelineMap["shadow_opaque"]);

	for (auto primitive : Primivitives)
	{
		RHI->UpdateObjectCB(primitive, TitanEngine::Get()->GetTimer(), sceneRender);
		RHI->UpdateMaterialCB(primitive);
		RHI->UpdateShadowPass(sceneRender);
		RHI->SetMeshBuffer(primitive);
		RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RHI->SetShaderData(primitive, RenderTargetMap["ShadowMap"]);
		RHI->DrawMesh(primitive);
	}
	RHI->ChangeResourceState(RenderTargetMap["ShadowMap"], RESOURCE_STATE_DEPTH_WRITE, RESOURCE_STATE_GENERIC_READ);



	// HDR Pass

	TViewPort HDRViewPort;
	RHI->SetViewPortAndRects(HDRViewPort);
	RHI->SetRenderTarget(RenderTargetMap["hdr"]);
	RHI->SetPipelineState(sceneRender->PipelineMap["hdr"]);

	for (auto primitive : Primivitives)
	{
		RHI->UpdateObjectCB(primitive, TitanEngine::Get()->GetTimer(), sceneRender);
		RHI->UpdateMaterialCB(primitive);
		RHI->UpdateShadowPass(sceneRender);
		RHI->SetMeshBuffer(primitive);
		RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RHI->SetShaderData(primitive, RenderTargetMap["hdr"]);
		RHI->DrawMesh(primitive);
	}

	RHI->ChangeResourceState(RenderTargetMap["hdr"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
	


	// Main Pass
	TViewPort MainViewPort;
	RHI->SetViewPortAndRects(MainViewPort);
	RHI->SetRenderTarget(RenderTargetMap["Base"]);
	RHI->SetPipelineState(sceneRender->PipelineMap["opaque"]);

	for (auto primitive : Primivitives)
	{
		RHI->UpdateObjectCB(primitive, TitanEngine::Get()->GetTimer(),sceneRender);
		RHI->UpdateMaterialCB(primitive);
		RHI->UpdateShadowPass(sceneRender);
		RHI->SetMeshBuffer(primitive);
		RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		RHI->SetShaderData(primitive, RenderTargetMap["Base"]);
		RHI->DrawMesh(primitive);
	}
	RHI->ChangeResourceState(RenderTargetMap["Base"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT);
	RHI->EndFrame(RenderTargetMap["Base"]);

}

void Renderer::UpdateScene()
{



}


