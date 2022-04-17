#include "stdafx.h"
#include "Renderer.h"
#include "FRHI.h"
#include "TitanEngine.h"
#include "GPUResourceManager.h"
#include "GameTimer.h"
#include "TRenderTargetDX12.h"
#include <pix.h>


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
	sceneRender->ShaderMap["bloomsetup"] = RHI->CreateShader("bloomsetup");
	sceneRender->ShaderMap["bloomdown"] = RHI->CreateShader("bloomdown");
	sceneRender->ShaderMap["bloomup"] = RHI->CreateShader("bloomup");
	sceneRender->ShaderMap["bloommerge"] = RHI->CreateShader("bloommerge");
	sceneRender->ShaderMap["tonemapping"] = RHI->CreateShader("tonemapping");
	sceneRender->ShaderMap["outline"] = RHI->CreateShader("outline");

	sceneRender->ShaderMap["ppout"] = RHI->CreateShader("ppout");


	sceneRender->PipelineMap["opaque"] = RHI->CreatePipelineState(sceneRender->ShaderMap["color"], "color", FORMAT_R8G8B8A8_UNORM);
	sceneRender->PipelineMap["shadow_opaque"] = RHI->CreatePipelineState(sceneRender->ShaderMap["shadow"], "shadow_opaque", FORMAT_UNKNOWN);
	sceneRender->PipelineMap["hdr"] = RHI->CreatePipelineState(sceneRender->ShaderMap["color"], "hdr", FORMAT_R16G16B16A16_FLOAT);
	sceneRender->PipelineMap["bloomsetup"] = RHI->CreatePipelineState(sceneRender->ShaderMap["bloomsetup"], "bloomsetup", FORMAT_R16G16B16A16_FLOAT);
	sceneRender->PipelineMap["bloomdown"] = RHI->CreatePipelineState(sceneRender->ShaderMap["bloomdown"], "bloomdown", FORMAT_R16G16B16A16_FLOAT);
	sceneRender->PipelineMap["bloomup"] = RHI->CreatePipelineState(sceneRender->ShaderMap["bloomup"], "bloomup", FORMAT_R16G16B16A16_FLOAT);
	sceneRender->PipelineMap["bloommerge"] = RHI->CreatePipelineState(sceneRender->ShaderMap["bloommerge"], "bloommerge", FORMAT_R16G16B16A16_FLOAT);
	sceneRender->PipelineMap["tonemapping"] = RHI->CreatePipelineState(sceneRender->ShaderMap["tonemapping"], "tonemapping", FORMAT_R16G16B16A16_FLOAT);
	sceneRender->PipelineMap["outline"] = RHI->CreatePipelineState(sceneRender->ShaderMap["outline"], "outline", FORMAT_R16G16B16A16_FLOAT);
	sceneRender->PipelineMap["ppout"] = RHI->CreatePipelineState(sceneRender->ShaderMap["ppout"], "ppout", FORMAT_R16G16B16A16_FLOAT);



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
	RenderTargetMap["hdr"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 800, 600);


	RenderTargetMap["bloomsetup"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 60, 60, 60, 200, 150);
	RenderTargetMap["bloomdownL1"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 61, 61, 61, 100, 75);
	RenderTargetMap["bloomdownL2"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 62, 62, 62, 50, 37);
	RenderTargetMap["bloomdownL3"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 63, 63, 63, 25, 18);
	
	RenderTargetMap["bloomupL1"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 65, 65, 65, 50, 37);
	RenderTargetMap["bloomupL2"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 66, 66, 66, 100, 75);
	RenderTargetMap["bloommerge"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 67, 67, 67, 200, 150);
	RenderTargetMap["tonemapping"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 68, 68, 68, 800, 600);

	RenderTargetMap["outline"] = RHI->CreateRenderTarget(HDR_RENDER_BUFFER, 69, 69, 69, 800, 600);


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
	
	BuildLight();

	RHI->BeginFrame();

	ShadowPass();
	HDRPass();

	ExtractHightlightPass();
	BloomDownL1Pass();
	BloomDownL2Pass();
	BloomDownL3Pass();
	BloomUpL1Pass();
	BloomUpL2Pass();

	BloomMergePass();
	ToneMapPass();
	OutlinePass();
	MainPass();

	RHI->EndFrame(RenderTargetMap["Base"]);

}

void Renderer::UpdateScene()
{



}

void Renderer::ShadowPass()
{
	auto Primivitives = TitanEngine::Get()->GetSceneIns()->SceneDataArr;
	TViewPort ShadowViewPort;
	ShadowViewPort.ClientHeight = 2048;
	ShadowViewPort.ClientWidth = 2048;
	ShadowViewPort.Height = 2048;
	ShadowViewPort.Width = 2048;
	RHI->SetViewPortAndRects(ShadowViewPort);
	RHI->SetRenderTarget(RenderTargetMap["ShadowMap"], "shadow pass");
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
	RHI->EndEvent();
}

void Renderer::HDRPass()
{
	auto Primivitives = TitanEngine::Get()->GetSceneIns()->SceneDataArr;

	TViewPort HDRViewPort;
	RHI->SetViewPortAndRects(HDRViewPort);
	RHI->SetRenderTarget(RenderTargetMap["hdr"], "hdr pass");
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
	RHI->EndEvent();
}

void Renderer::ExtractHightlightPass()
{
	// extract hightlight
	TViewPort HDRViewPort;
	RHI->SetViewPortAndRects(HDRViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["bloomsetup"], "extract hightlight");
	RHI->SetPipelineState(sceneRender->PipelineMap["bloomsetup"]);


	triangle->MeshBuffer = MeshBufferMap["triangle.titan"];
	triangle->AssetPath = "triangle.titan";
	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["hdr"], nullptr);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["bloomsetup"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::BloomDownL1Pass()
{
	TViewPort bd1ViewPort;
	RHI->SetViewPortAndRects(bd1ViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["bloomdownL1"], "bloomdown");
	RHI->SetPipelineState(sceneRender->PipelineMap["bloomdown"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["bloomsetup"], nullptr);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["bloomdownL1"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::BloomDownL2Pass()
{
	TViewPort bd1ViewPort;
	RHI->SetViewPortAndRects(bd1ViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["bloomdownL2"], "bloomdown");
	RHI->SetPipelineState(sceneRender->PipelineMap["bloomdown"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["bloomdownL1"], nullptr);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["bloomdownL2"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::BloomDownL3Pass()
{
	TViewPort bd1ViewPort;
	RHI->SetViewPortAndRects(bd1ViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["bloomdownL3"], "bloomdown");
	RHI->SetPipelineState(sceneRender->PipelineMap["bloomdown"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["bloomdownL2"], nullptr);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["bloomdownL3"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);

}

void Renderer::BloomUpL1Pass()
{
	TViewPort bd1ViewPort;

	RHI->SetViewPortAndRects(bd1ViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["bloomupL1"], "bloomup");
	RHI->SetPipelineState(sceneRender->PipelineMap["bloomup"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["bloomdownL3"], RenderTargetMap["bloomdownL2"]);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["bloomupL1"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::BloomUpL2Pass()
{
	TViewPort bd1ViewPort;

	RHI->SetViewPortAndRects(bd1ViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["bloomupL2"], "bloomup");
	RHI->SetPipelineState(sceneRender->PipelineMap["bloomup"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["bloomupL1"], RenderTargetMap["bloomdownL1"]);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["bloomupL2"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::BloomMergePass()
{
	TViewPort bd1ViewPort;

	RHI->SetViewPortAndRects(bd1ViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["bloommerge"], "bloom merge");
	RHI->SetPipelineState(sceneRender->PipelineMap["bloommerge"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["bloomupL2"], RenderTargetMap["bloomsetup"]);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["bloommerge"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::ToneMapPass()
{
	TViewPort bd1ViewPort;

	RHI->SetViewPortAndRects(bd1ViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["tonemapping"], "tone mapping");
	RHI->SetPipelineState(sceneRender->PipelineMap["tonemapping"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["hdr"], RenderTargetMap["bloommerge"]);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["tonemapping"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::OutlinePass()
{
	// out line

	TViewPort OLViewPort;
	RHI->SetViewPortAndRects(OLViewPort);
	RHI->SetRenderTargetbloom(RenderTargetMap["outline"], "outline");

	RHI->SetPipelineState(sceneRender->PipelineMap["outline"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["tonemapping"], nullptr);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["outline"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_GENERIC_READ);
}

void Renderer::MainPass()
{

	// Main Pass

	TViewPort MainViewPort;
	RHI->SetViewPortAndRects(MainViewPort);
	RHI->SetRenderTarget(RenderTargetMap["Base"], "final pass");
	RHI->SetPipelineState(sceneRender->PipelineMap["ppout"]);

	RHI->SetMeshBuffer(triangle);
	RHI->SetPrimitiveTopology(PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	RHI->SetShaderDatabloom(RenderTargetMap["outline"], nullptr);

	RHI->DrawMesh(triangle);
	RHI->ChangeResourceState(RenderTargetMap["Base"], RESOURCE_STATE_RENDER_TARGET, RESOURCE_STATE_PRESENT);
	RHI->EndEvent();
}


