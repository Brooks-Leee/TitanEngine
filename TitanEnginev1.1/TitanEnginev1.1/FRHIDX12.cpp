#include "stdafx.h"
#include "FRHIDX12.h"
#include "Win32Window.h"
#include "TitanEngine.h"
#include "DDSTextureLoader.h"
#include "TLight.h"


void FRHIDX12::InitRHI(Scene* scene)
{
	mScene = scene;
	FRHIDX12::Initialize();

}

void FRHIDX12::CreateMeshBuffer(FMeshData* meshData)
{
	mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get());

	std::vector<Vertex> vertices;
	size_t verticesLen = meshData->Vertices.size();
	vertices.resize(verticesLen);

	for (int i = 0; i < verticesLen; i++)
	{
		vertices[i].Pos.x = meshData->Vertices[i].x;
		vertices[i].Pos.y = meshData->Vertices[i].y;
		vertices[i].Pos.z = meshData->Vertices[i].z;

		vertices[i].Normal.x = meshData->normals[i].x;
		vertices[i].Normal.y = meshData->normals[i].y;
		vertices[i].Normal.z = meshData->normals[i].z;
		vertices[i].Normal.w = meshData->normals[i].w;

		vertices[i].Texcoord.x = meshData->texcoords[i].u;
		vertices[i].Texcoord.y = meshData->texcoords[i].v;
	}

	std::vector<uint32_t> indices;
	indices.resize(meshData->indices.size());
	indices = meshData->indices;

	std::shared_ptr<MeshGeometry> Geo = std::make_shared<MeshGeometry>();

	const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
	const UINT ibByteSize = (UINT)indices.size() * sizeof(uint32_t);

	Geo->Name = meshData->AssetPath;

	ThrowIfFailed(D3DCreateBlob(vbByteSize, &Geo->VertexBufferCPU));
	ThrowIfFailed(D3DCreateBlob(ibByteSize, &Geo->IndexBufferCPU));

	CopyMemory(Geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
	CopyMemory(Geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

	Geo->VertexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), vertices.data(), vbByteSize, Geo->VertexBufferUploader);

	Geo->IndexBufferGPU = d3dUtil::CreateDefaultBuffer(md3dDevice.Get(),
		mCommandList.Get(), indices.data(), ibByteSize, Geo->IndexBufferUploader);

	Geo->VertexByteStride = sizeof(Vertex);
	Geo->VertexBufferByteSize = vbByteSize;
	Geo->IndexFormat = DXGI_FORMAT_R32_UINT;
	Geo->IndexBufferByteSize = ibByteSize;

	SubmeshGeometry submesh;
	submesh.IndexCount = (UINT)indices.size();
	submesh.StartIndexLocation = 0;
	submesh.BaseVertexLocation = 0;

	Geo->DrawArgs[meshData->AssetPath] = submesh;

	mGeoMap[meshData->AssetPath] = std::move(Geo);
	mGeoArr.push_back(Geo);
	

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

}


void FRHIDX12::CreateConstantBuffer()
{
	mCurrentElementCount = (UINT)mScene->SceneDataArr.size();
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), mCurrentElementCount, true);
	mMaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(md3dDevice.Get(), 3, true);
	mShadowPassCB = std::make_unique<UploadBuffer<ShadowPassConstants>>(md3dDevice.Get(), mCurrentElementCount, true);

	UINT DescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	// Constant buffers must be a multiple of the minimum hardware allocation size (usually 256 bytes)
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
	matCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(MaterialConstants));
	shadowCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ShadowPassConstants));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	auto heapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < mCurrentElementCount; i++)
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
		cbAddress += i * objCBByteSize;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		md3dDevice->CreateConstantBufferView(&cbvDesc, heapCPUHandle);
		heapCPUHandle.Offset(1, DescriptorSize);
	}
}

void FRHIDX12::CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index)
{
	ResetCommandList();

	auto renderTex = std::make_shared<TTextureDX12>();
	renderTex->Filename = Texture->Filename;
	renderTex->Name = Texture->Name;
	renderTex->TexIndex = mCurrentElementCount + index;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), renderTex->Filename.c_str(), renderTex->Resource, renderTex->UploadHeap));
	
	mTextures[renderTex->Name] = std::move(renderTex);
	//std::shared_ptr<TRHITexture> GPUTexture = renderTex;
	CloseCommandList();
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	auto Tex = mTextures[Texture->Name]->Resource;
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Tex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	auto heapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
	UINT DescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	heapCPUHandle.Offset(mTextures[Texture->Name]->TexIndex, DescriptorSize);
	
	md3dDevice->CreateShaderResourceView(Tex.Get(), &srvDesc, heapCPUHandle);

}

void FRHIDX12::CreateMaterials()
{
	auto water = std::make_shared<TMaterial>();
	water->name = "water";
	water->diffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	water->fresnelR0 = glm::vec3(0.2f, 0.2f, 0.2f);
	water->roughness = 0.0f;
	water->diffuseSrvHeapIndex = mTextures["waterTex"]->TexIndex;
	water->matCBIndex = 0;

	auto rock = std::make_shared<TMaterial>();
	rock->name = "rock";
	rock->diffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	rock->fresnelR0 = glm::vec3(0.01f, 0.01f, 0.01f);
	rock->roughness = 0.125f;
	rock->diffuseSrvHeapIndex = mTextures["rockTex"]->TexIndex;
	rock->matCBIndex = 1;


	auto brick = std::make_shared<TMaterial>();
	brick->name = "brick";
	brick->diffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	brick->fresnelR0 = glm::vec3(0.1f, 0.1f, 0.1f);
	brick->roughness = 0.25f;
	brick->diffuseSrvHeapIndex = mTextures["brickTex"]->TexIndex;
	brick->matCBIndex = 2;


	mMaterials["water"] = std::move(water);
	mMaterials["rock"] = std::move(rock);
	mMaterials["brick"] = std::move(brick);

}


void FRHIDX12::SetViewPort(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth)
{

	mScreenViewport.TopLeftX = TopLeftX;
	mScreenViewport.TopLeftY = TopLeftY;
	mScreenViewport.Width = Width;
	mScreenViewport.Height = Height;
	mScreenViewport.MinDepth = MinDepth;
	mScreenViewport.MaxDepth = MaxDepth;
	mCommandList->RSSetViewports(1, &mScreenViewport);
}

void FRHIDX12::SetScissorRects(int ClientWidth, int ClientHeight)
{
	mScissorRect = { 0, 0, ClientWidth, ClientHeight };
	mCommandList->RSSetScissorRects(1, &mScissorRect);
	
}

void FRHIDX12::SetMeshBuffer()
{
}




void FRHIDX12::UpdateObjectCB(FSceneData actor)
{
	ObjectConstants objConstants;
	mScene->camera.UpdateViewMat();

	mView = mScene->camera.GetView4x4();
	mProj = mScene->camera.GetProj4x4();
	mCameraloc = mScene->camera.GetCameraPos3f();


	auto& t = actor.Transform;
	auto& s = t.scale;
	auto& r = t.rotation;
	auto& l = t.location;

	auto scale = glm::scale(MathHelper::Identity4x4glm(), glm::vec3(s.x, s.y, s.z));
	auto rotation = glm::transpose(glm::mat4_cast(glm::quat(r.w, r.pitch, r.yaw, r.roll)));
	auto location = glm::translate(MathHelper::Identity4x4glm(), glm::vec3(l.x, l.y, l.z));


	glm::mat4 world = location * rotation * scale;
	//glm::mat4 world = location * scale * rotation;
	glm::mat4 view = mView;
	glm::mat4 proj = mProj;
	glm::mat4 worldViewProj = proj * view * world;

	mWorld = glm::transpose(world);


	objConstants.World = mWorld;
	objConstants.ViewProj = glm::transpose(proj * view);
	objConstants.WorldViewProj = glm::transpose(worldViewProj);
	objConstants.gTime = mTimer.TotalTime();
	objConstants.Location = location;
	objConstants.Rotation = rotation;
	objConstants.Scale = scale;
	// put the constant object into constant buffer which is a Upload Buffer
	mObjectCB->CopyData(actor.HeapIndex, objConstants);

}

void FRHIDX12::UpdateMaterialCB()
{
	MaterialConstants matConstants;
	for (auto material : mMaterials)
	{
		auto mat = material.second.get();
		matConstants.diffuseAlbedo = mat->diffuseAlbedo;
		matConstants.fresnelR0 = mat->fresnelR0;
		matConstants.roughness = mat->roughness;
		mMaterialCB->CopyData(mat->matCBIndex, matConstants);
	}

}

void FRHIDX12::UpdateShadowPass(FSceneData actor)
{
	//TLight* light = TitanEngine::Get()->GetSceneIns()->light;
		//glm::vec3 lightDir = light->LightDirection;

	glm::vec3 lightPos = glm::vec3(2620.0f, 2450.0f, 1860.0f);
	glm::vec3 TargetPos = glm::vec3(0.0f, 0.0f, 0.0f);
	glm::vec3 lightUp = glm::vec3(0.0f, 0.0f, 1.0f);

	glm::mat4 lightView = glm::lookAtLH(lightPos, TargetPos, lightUp);
	glm::mat4 lightProj = glm::orthoLH_ZO(-3000.f, 4000.f, -2000.f, 4000.f, -1000.0f, 10000.f);

	//glm::mat4 LightVP = glm::transpose(lightProj * lightView);


	glm::mat4 LightVP = glm::transpose(lightProj * lightView);
	//glm::mat4 LightMVP = glm::transpose(LightVP * mWorld);




	//	glm::mat4 TLightMVP = glm::transpose(LightMVP);
		//XMVECTOR lightDir = XMLoadFloat3(&light->LightDirection);
		//XMVECTOR lightPos = -2.0f * mSceneBounds.Radius * lightDir;
		//XMVECTOR targetPos = XMLoadFloat3(&mSceneBounds.Center);
		//XMVECTOR lightUp = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		//XMMATRIX lightView = XMMatrixLookAtLH(lightPos, targetPos, lightUp);

		//XMFLOAT3 sphereCenterLS;
		//XMStoreFloat3(&sphereCenterLS, XMVector3TransformCoord(targetPos, lightView));

		//// Ortho frustum in light space encloses scene.
		//float l = sphereCenterLS.x - mSceneBounds.Radius;
		//float b = sphereCenterLS.y - mSceneBounds.Radius;
		//float n = sphereCenterLS.z - mSceneBounds.Radius;
		//float r = sphereCenterLS.x + mSceneBounds.Radius;
		//float f = sphereCenterLS.z + mSceneBounds.Radius;
		//float t = sphereCenterLS.y + mSceneBounds.Radius;
		//XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(-100.f, 100.f, -100.f, 100.f, 100.f, 10000.f);

		//// Transform NDC space [-1,+1]^2 to texture space [0,1]^2
		//XMMATRIX T(
		//	0.5f, 0.0f, 0.0f, 0.0f,
		//	0.0f, -0.5f, 0.0f, 0.0f,
		//	0.0f, 0.0f, 1.0f, 0.0f,
		//	0.5f, 0.5f, 0.0f, 1.0f);

		//XMMATRIX ViewProj = lightView * lightProj * T;
		//XMFLOAT4X4 LightViewProj;

		//XMStoreFloat4x4(&LightViewProj, XMMatrixTranspose(ViewProj));

	ShadowPassConstants ShadowConstant;
	ShadowConstant.lightMVP = LightVP;

	/*glm::mat4 testmat = MathHelper::testglm();
	ShadowConstant.lightMVP = testmat;*/

	//mShadowPassCB->CopyData(actor.HeapIndex, ShadowConstant);
	mShadowPassCB->CopyData(0, ShadowConstant);
}



void FRHIDX12::SetRenderTarget()
{
	//ThrowIfFailed(mDirectCmdListAlloc->Reset());

	//ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSOs["opaque"].Get()));
	mCommandList->SetPipelineState(mPSOs["opaque"].Get());

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);

	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	float rtvColor[4] = { 1.0f, 0.9f, 0.8f, 1.0f };
	// Clear the back buffer and depth buffer.
	mCommandList->ClearRenderTargetView(CurrentBackBufferView(), rtvColor, 0, nullptr);
	mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

	// Specify the buffers we are going to render to.
	mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(), true, &DepthStencilView());

	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	auto shadowPassCB = mShadowPassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS shadowPassCBAddress = shadowPassCB->GetGPUVirtualAddress();
	mCommandList->SetGraphicsRootConstantBufferView(4, shadowPassCBAddress);
}

void FRHIDX12::SetShadowMapTarget()
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSOs["opaque"].Get()));
	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());
	mCommandList->RSSetViewports(1, &mShadowMap->Viewport());
	mCommandList->RSSetScissorRects(1, &mShadowMap->ScissorRect());

	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	mCommandList->ClearDepthStencilView(mShadowMap->Dsv(),
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
	mCommandList->OMSetRenderTargets(0, nullptr, false, &mShadowMap->Dsv());
	mCommandList->SetPipelineState(mPSOs["shadow_opaque"].Get());


}


void FRHIDX12::DrawShadowMap(FSceneData actor)
{
	auto geo = mGeoMap[actor.AssetPath];
	mCommandList->IASetVertexBuffers(0, 1, &geo->VertexBufferView());
	mCommandList->IASetIndexBuffer(&geo->IndexBufferView());
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	heapGPUHandle.Offset(actor.HeapIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mCommandList->SetGraphicsRootDescriptorTable(0, heapGPUHandle);


	auto shadowPassCB = mShadowPassCB->Resource();
	//D3D12_GPU_VIRTUAL_ADDRESS shadowPassCBAddress = shadowPassCB->GetGPUVirtualAddress() + actor.HeapIndex * shadowCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS shadowPassCBAddress = shadowPassCB->GetGPUVirtualAddress();
	mCommandList->SetGraphicsRootConstantBufferView(4, shadowPassCBAddress);

	mCommandList->DrawIndexedInstanced(geo->DrawArgs[geo->Name].IndexCount, 1, 0, 0, 0);


}

void FRHIDX12::EndSHadowMap()
{
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mShadowMap->Resource(),
		D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ));
}


void FRHIDX12::Draw(FSceneData actor)
{

	//ThrowIfFailed(mDirectCmdListAlloc->Reset());

	//ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));

	auto geo = mGeoMap[actor.AssetPath];
	mCommandList->IASetVertexBuffers(0, 1, &geo->VertexBufferView());
	mCommandList->IASetIndexBuffer(&geo->IndexBufferView());
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);



	auto heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	heapGPUHandle.Offset(actor.HeapIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mCommandList->SetGraphicsRootDescriptorTable(0, heapGPUHandle);

	auto shadowMapHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mShadowMap->Srv());
	mCommandList->SetGraphicsRootDescriptorTable(5, shadowMapHandle);



	if (geo->Name == "Plane.titan")
	{
		heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
		heapGPUHandle.Offset(mTextures["waterTex"]->TexIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(1, heapGPUHandle);
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = mMaterialCB->Resource()->GetGPUVirtualAddress() + mMaterials["water"]->matCBIndex * matCBByteSize;
		mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);

	}
	else if (geo->Name == "SM_MatPreviewMesh_02.titan")
	{
		heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
		heapGPUHandle.Offset(mTextures["rockTex"]->TexIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(1, heapGPUHandle);
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = mMaterialCB->Resource()->GetGPUVirtualAddress() + mMaterials["rock"]->matCBIndex * matCBByteSize;
		mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);
	}
	else
	{
		heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
		heapGPUHandle.Offset(mTextures["brickTex"]->TexIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(1, heapGPUHandle);
		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = mMaterialCB->Resource()->GetGPUVirtualAddress() + mMaterials["brick"]->matCBIndex * matCBByteSize;
		mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);
	}

	
	mCommandList->SetGraphicsRoot32BitConstants(2, 3, &mCameraloc, 0);

	mCommandList->DrawIndexedInstanced(geo->DrawArgs[geo->Name].IndexCount, 1, 0, 0, 0);

	

}

void FRHIDX12::EndFrame()
{
	// Indicate a state transition on the resource usage.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(),
		D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT));

	// Done recording commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// swap the back and front buffers
	ThrowIfFailed(mSwapChain->Present(0, 0));
	mCurrBackBuffer = (mCurrBackBuffer + 1) % SwapChainBufferCount;

	// Wait until frame commands are complete.  This waiting is inefficient and is
	// done for simplicity.  Later we will show how to organize our rendering code
	// so we do not have to wait per frame.

	FlushCommandQueue();
}





bool FRHIDX12::Initialize()
{
	if (!InitDirect3D())
		return false;

	OnResize();

	//RootSignature();
	//BuildShadersAndInputLayout();
	//BuildPSO();
	mShadowMap = std::make_unique<ShadowMap>(md3dDevice.Get(), 2048, 2048);
	mSceneBounds.Center = XMFLOAT3(0.0f, 0.0f, 0.0f);
	mSceneBounds.Radius = sqrtf(3000000);

	BuildDescriptorHeaps();
	BuildRootSignature();
	BuildShadersAndInputLayout();
	BuildPSO();

	return true;
}

void FRHIDX12::OnResize()
{
	assert(md3dDevice);
	assert(mSwapChain);
	assert(mDirectCmdListAlloc);

	// Flush before changing any resources.
	FlushCommandQueue();

	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));

	// Release the previous resources we will be recreating.
	for (int i = 0; i < SwapChainBufferCount; ++i)
		mSwapChainBuffer[i].Reset();
	mDepthStencilBuffer.Reset();

	// Resize the swap chain.
	ThrowIfFailed(mSwapChain->ResizeBuffers(
		SwapChainBufferCount,
		mClientWidth, mClientHeight,
		mBackBufferFormat,
		DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

	mCurrBackBuffer = 0;

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
	for (UINT i = 0; i < SwapChainBufferCount; i++)
	{
		ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])));
		md3dDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
		rtvHeapHandle.Offset(1, mRtvDescriptorSize);
	}

	// Create the depth/stencil buffer and view.
	D3D12_RESOURCE_DESC depthStencilDesc;
	depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	depthStencilDesc.Alignment = 0;
	depthStencilDesc.Width = mClientWidth;
	depthStencilDesc.Height = mClientHeight;
	depthStencilDesc.DepthOrArraySize = 1;
	depthStencilDesc.MipLevels = 1;

	// Correction 11/12/2016: SSAO chapter requires an SRV to the depth buffer to read from 
	// the depth buffer.  Therefore, because we need to create two views to the same resource:
	//   1. SRV format: DXGI_FORMAT_R24_UNORM_X8_TYPELESS
	//   2. DSV Format: DXGI_FORMAT_D24_UNORM_S8_UINT
	// we need to create the depth buffer resource with a typeless format.  
	depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;

	depthStencilDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	depthStencilDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE optClear;
	optClear.Format = mDepthStencilFormat;
	optClear.DepthStencil.Depth = 1.0f;
	optClear.DepthStencil.Stencil = 0;
	ThrowIfFailed(md3dDevice->CreateCommittedResource(
		&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
		D3D12_HEAP_FLAG_NONE,
		&depthStencilDesc,
		D3D12_RESOURCE_STATE_COMMON,
		&optClear,
		IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())));


	// Create descriptor to mip level 0 of entire resource using the format of the resource.
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Format = mDepthStencilFormat;
	dsvDesc.Texture2D.MipSlice = 0;
	md3dDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

	// Transition the resource from its initial state to be used as a depth buffer.
	mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(),
		D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));

	// Execute the resize commands.
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	// Wait until resize is complete.
	FlushCommandQueue();

	// Update the viewport transform to cover the client area.
	mScreenViewport.TopLeftX = 0;
	mScreenViewport.TopLeftY = 0;
	mScreenViewport.Width = static_cast<float>(mClientWidth);
	mScreenViewport.Height = static_cast<float>(mClientHeight);
	mScreenViewport.MinDepth = 0.0f;
	mScreenViewport.MaxDepth = 1.0f;

	mScissorRect = { 0, 0, mClientWidth, mClientHeight };


	mScene->camera.SetCameraPos(1000.0f, 2000.0f, 2000.0f);
	mScene->camera.SetLens(0.25f * MathHelper::Piglm, static_cast<float>(mClientWidth) / mClientHeight, 1.0f, 1000000.0f);
	mScene->camera.LookAt(mScene->camera.GetCameraPos3f(), glm::vec3(0.0f, 0.0f, 0.0f), mScene->camera.GetUp());

	//wwwwmProj = glm::perspectiveFovLH(0.25f * MathHelper::Piglm, (float)mClientWidth, (float)mClientHeight, 1.0f, 1000000.0f);
}


bool FRHIDX12::InitDirect3D()
{
	#if defined(DEBUG) || defined(_DEBUG) 
	// Enable the D3D12 debug layer.
	{
		ComPtr<ID3D12Debug> debugController;
		ThrowIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)));
		debugController->EnableDebugLayer();
	}
	#endif

	ThrowIfFailed(CreateDXGIFactory1(IID_PPV_ARGS(&mdxgiFactory)));

	// Try to create hardware device.
	HRESULT hardwareResult = D3D12CreateDevice(
		nullptr,             // default adapter
		D3D_FEATURE_LEVEL_11_0,
		IID_PPV_ARGS(&md3dDevice));

	// Fallback to WARP device.
	if (FAILED(hardwareResult))
	{
		ComPtr<IDXGIAdapter> pWarpAdapter;
		ThrowIfFailed(mdxgiFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)));

		ThrowIfFailed(D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&md3dDevice)));
	}

	ThrowIfFailed(md3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)));

	mRtvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	mDsvDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	mCbvSrvUavDescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Check 4X MSAA quality support for our back buffer format.
	// All Direct3D 11 capable devices support 4X MSAA for all render 
	// target formats, so we only need to check quality support.

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
	msQualityLevels.Format = mBackBufferFormat;
	msQualityLevels.SampleCount = 4;
	msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	msQualityLevels.NumQualityLevels = 0;
	ThrowIfFailed(md3dDevice->CheckFeatureSupport(
		D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&msQualityLevels,
		sizeof(msQualityLevels)));

	//m4xMsaaQuality = msQualityLevels.NumQualityLevels;
	//assert(m4xMsaaQuality > 0 && "Unexpected MSAA quality level.");

	#ifdef _DEBUG
	LogAdapters();
	#endif

	CreateCommandObjects();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	return true;
}

void FRHIDX12::CreateCommandObjects()
{
	D3D12_COMMAND_QUEUE_DESC queueDesc = {};
	queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	ThrowIfFailed(md3dDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)));

	ThrowIfFailed(md3dDevice->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		IID_PPV_ARGS(mDirectCmdListAlloc.GetAddressOf())));
	ThrowIfFailed(md3dDevice->CreateCommandList(
		0,
		D3D12_COMMAND_LIST_TYPE_DIRECT,
		mDirectCmdListAlloc.Get(), // Associated command allocator
		nullptr,                   // Initial PipelineStateObject
		IID_PPV_ARGS(mCommandList.GetAddressOf())));

	// Start off in a closed state.  This is because the first time we refer 
	// to the command list we will Reset it, and it needs to be closed before
	// calling Reset.
	mCommandList->Close();
}
void FRHIDX12::CreateSwapChain()
{
	// Release the previous swapchain we will be recreating.
	mSwapChain.Reset();

	DXGI_SWAP_CHAIN_DESC sd;
	sd.BufferDesc.Width = mClientWidth;
	sd.BufferDesc.Height = mClientHeight;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = mBackBufferFormat;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	sd.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	sd.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = SwapChainBufferCount;

	HWND hwnd = dynamic_cast<Win32Window*>(TitanEngine::Get()->GetWindowIns())->GetHWnd();
	sd.OutputWindow = hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	// Note: Swap chain uses queue to perform flush.
	ThrowIfFailed(mdxgiFactory->CreateSwapChain(
		mCommandQueue.Get(),
		&sd,
		mSwapChain.GetAddressOf()));
}
void FRHIDX12::CreateRtvAndDsvDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
	rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 2;
	dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())));
}


void FRHIDX12::FlushCommandQueue()
{
	mCurrentFence++;

	// Add an instruction to the command queue to set a new fence point.  Because we 
	// are on the GPU timeline, the new fence point won't be set until the GPU finishes
	// processing all the commands prior to this Signal().
	ThrowIfFailed(mCommandQueue->Signal(mFence.Get(), mCurrentFence));

	// Wait until the GPU has completed commands up to this fence point.
	if (mFence->GetCompletedValue() < mCurrentFence)
	{
		HANDLE eventHandle = CreateEventEx(nullptr, false, false, EVENT_ALL_ACCESS);

		// Fire event when GPU hits current fence.  
		ThrowIfFailed(mFence->SetEventOnCompletion(mCurrentFence, eventHandle));

		// Wait until the GPU hits current fence event is fired.
		WaitForSingleObject(eventHandle, INFINITE);
		CloseHandle(eventHandle);
	}
}


void FRHIDX12::BuildDescriptorHeaps()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvAndsrvDesc;
	//mCurrentElementCount = (UINT)mScene->SceneDataArr.size();
	cbvAndsrvDesc.NumDescriptors = 100;
	cbvAndsrvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvAndsrvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvAndsrvDesc.NodeMask = 0;

	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvAndsrvDesc, IID_PPV_ARGS(&mCbvSrvHeap)));

	// allocate heap memory for shadow map pass
	auto CbvSrvCPUHeapStart = mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart();
	auto CbvSrvGPUHeapStart = mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart();
	auto DsvCPUStart = mDsvHeap->GetCPUDescriptorHandleForHeapStart();

	UINT descriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	UINT dsvDesSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	mShadowMap->BuildDescriptors(
		CD3DX12_CPU_DESCRIPTOR_HANDLE(CbvSrvCPUHeapStart, 50, descriptorSize),
		CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvGPUHeapStart, 50, descriptorSize),
		CD3DX12_CPU_DESCRIPTOR_HANDLE(DsvCPUStart, 1, dsvDesSize));
}

void FRHIDX12::BuildDescriptor()
{
	mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(md3dDevice.Get(), mCurrentElementCount, true);

	UINT DescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// Constant buffers must be a multiple of the minimum hardware allocation size (usually 256 bytes)
	UINT objCBByteSize = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

	D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;

	auto heapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < mCurrentElementCount; i++)
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
		cbAddress += i * objCBByteSize;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = d3dUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
		md3dDevice->CreateConstantBufferView(&cbvDesc, heapCPUHandle);
		heapCPUHandle.Offset(1, DescriptorSize);
	}
	// Fill out SRV 
	auto Textures = TitanEngine::Get()->GetResourceMgr()->getTextures();

	// Create render texture
	ResetCommandList();
	for (auto texture : Textures)
	{
		texture->Name;
		auto renderTex = std::make_shared<TTextureDX12>();
		renderTex->Filename = texture->Filename;
		renderTex->Name = texture->Name;
		ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), texture->Filename.c_str(), renderTex->Resource, renderTex->UploadHeap));
		mTextures[renderTex->Name] = std::move(renderTex);

	}
	CloseCommandList();
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	auto waterTex = mTextures["waterTex"]->Resource;
	auto rockTex = mTextures["rockTex"]->Resource;
	auto brickTex = mTextures["brickTex"]->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = waterTex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	md3dDevice->CreateShaderResourceView(waterTex.Get(), &srvDesc, heapCPUHandle);
	mTexTableIndex["waterTex"] = mCurrentElementCount;

	srvDesc.Format = rockTex->GetDesc().Format;
	heapCPUHandle.Offset(1, DescriptorSize);
	md3dDevice->CreateShaderResourceView(rockTex.Get(), &srvDesc, heapCPUHandle);
	mTexTableIndex["rockTex"] = mCurrentElementCount + 1;

	heapCPUHandle.Offset(1, DescriptorSize);
	srvDesc.Format = brickTex->GetDesc().Format;
	md3dDevice->CreateShaderResourceView(brickTex.Get(), &srvDesc, heapCPUHandle);
	mTexTableIndex["brickTex"] = mCurrentElementCount + 2;


}

void FRHIDX12::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE shadowMapTable;
	shadowMapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	//CD3DX12_DESCRIPTOR_RANGE normalTable;
	//normalTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);



	CD3DX12_ROOT_PARAMETER slotRootParameter[6]; 
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
	slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	// camera location
	slotRootParameter[2].InitAsConstants(3, 1, 0);
	// Material constant buffer
	slotRootParameter[3].InitAsConstantBufferView(2);
	// shadow map constant buffer
	slotRootParameter[4].InitAsConstantBufferView(3);
	slotRootParameter[5].InitAsDescriptorTable(1, &shadowMapTable);


	auto staticSamplers = GetStaticSamplers();
	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(6, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(),
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	// create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
	ComPtr<ID3DBlob> serializedRootSig = nullptr;
	ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

	if (errorBlob != nullptr)
	{
		::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
	}
	ThrowIfFailed(hr);

	ThrowIfFailed(md3dDevice->CreateRootSignature(
		0,
		serializedRootSig->GetBufferPointer(),
		serializedRootSig->GetBufferSize(),
		IID_PPV_ARGS(&mRootSignature)));
}


void FRHIDX12::BuildShadersAndInputLayout()
{
	HRESULT hr = S_OK;

	mvsByteCode = d3dUtil::CompileShader(L"Assets\\Shaders\\color.hlsl", nullptr, "VS", "vs_5_0");
	mpsByteCode = d3dUtil::CompileShader(L"Assets\\Shaders\\color.hlsl", nullptr, "PS", "ps_5_0");

	msmapVSByteCode = d3dUtil::CompileShader(L"Assets\\Shaders\\shadowmap.hlsl", nullptr, "VS", "vs_5_0");
	//msmapVSByteCode = d3dUtil::CompileShader(L"Assets\\Shaders\\shadowmap.hlsl", nullptr, "PS", "vs_5_0");


	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};
}

void FRHIDX12::BuildPSO()
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();

	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()),
		mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()),
		mpsByteCode->GetBufferSize()
	};

	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.FrontCounterClockwise = true;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = mBackBufferFormat;
	psoDesc.SampleDesc.Count = m4xMsaaState ? 4 : 1;
	psoDesc.SampleDesc.Quality = m4xMsaaState ? (m4xMsaaQuality - 1) : 0;
	psoDesc.DSVFormat = mDepthStencilFormat;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSOs["opaque"])));


	D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = psoDesc;
	//smapPsoDesc.RasterizerState.DepthBias = 100000;
	//smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
	//smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
	smapPsoDesc.pRootSignature = mRootSignature.Get();

	smapPsoDesc.VS = 
	{
		reinterpret_cast<BYTE*>(msmapVSByteCode->GetBufferPointer()), msmapVSByteCode->GetBufferSize()
	};
	//smapPsoDesc.PS = 
	//{
	//	reinterpret_cast<BYTE*>(msmapPSByteCode->GetBufferPointer()), msmapPSByteCode->GetBufferSize()
	//};
	
	smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
	smapPsoDesc.NumRenderTargets = 0;
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&mPSOs["shadow_opaque"])));

}






ID3D12Resource* FRHIDX12::CurrentBackBuffer() const
{
	return mSwapChainBuffer[mCurrBackBuffer].Get();
}

D3D12_CPU_DESCRIPTOR_HANDLE FRHIDX12::CurrentBackBufferView() const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		mCurrBackBuffer,
		mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE FRHIDX12::DepthStencilView() const
{
	return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> FRHIDX12::GetStaticSamplers()
{
	// Applications usually only need a handful of samplers.  So just define them all up front
	// and keep them available as part of the root signature.  

	const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
		0, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
		1, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
		2, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
		3, // shaderRegister
		D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
		4, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
		0.0f,                             // mipLODBias
		8);                               // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
		5, // shaderRegister
		D3D12_FILTER_ANISOTROPIC, // filter
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
		0.0f,                              // mipLODBias
		8);                                // maxAnisotropy

	const CD3DX12_STATIC_SAMPLER_DESC shadow(
		6, // shaderRegister
		D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
		D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
		0.0f,                               // mipLODBias
		16,                                 // maxAnisotropy
		D3D12_COMPARISON_FUNC_LESS_EQUAL,
		D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

	return {
		pointWrap, pointClamp,
		linearWrap, linearClamp,
		anisotropicWrap, anisotropicClamp, shadow };
}

void FRHIDX12::ResetCommandList()
{
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), mPSO.Get()));
}

void FRHIDX12::CloseCommandList()
{
	ThrowIfFailed(mCommandList->Close());
}





void FRHIDX12::CalculateFrameStats()
{
	static int frameCnt = 0;
	static float timeElapsed = 0.0f;

	frameCnt++;

	// Compute averages over one second period.
	if ((mTimer.TotalTime() - timeElapsed) >= 1.0f)
	{
		float fps = (float)frameCnt; // fps = frameCnt / 1
		float mspf = 1000.0f / fps;

		wstring fpsStr = to_wstring(fps);
		wstring mspfStr = to_wstring(mspf);

		wstring windowText =
			L"TitanEngine    fps: " + fpsStr +
			L"   mspf: " + mspfStr;

		SetWindowText(dynamic_cast<Win32Window*>(TitanEngine::Get()->GetWindowIns())->GetHWnd(), windowText.c_str());

		// Reset for next average.
		frameCnt = 0;
		timeElapsed += 1.0f;
	}
}

void FRHIDX12::LogAdapters()
{
	UINT i = 0;
	IDXGIAdapter* adapter = nullptr;
	std::vector<IDXGIAdapter*> adapterList;
	while (mdxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);

		std::wstring text = L"***Adapter: ";
		text += desc.Description;
		text += L"\n";

		OutputDebugString(text.c_str());

		adapterList.push_back(adapter);

		++i;
	}

	for (size_t i = 0; i < adapterList.size(); ++i)
	{
		LogAdapterOutputs(adapterList[i]);
		ReleaseCom(adapterList[i]);
	}
}

void FRHIDX12::LogAdapterOutputs(IDXGIAdapter* adapter)
{
	UINT i = 0;
	IDXGIOutput* output = nullptr;
	while (adapter->EnumOutputs(i, &output) != DXGI_ERROR_NOT_FOUND)
	{
		DXGI_OUTPUT_DESC desc;
		output->GetDesc(&desc);

		std::wstring text = L"***Output: ";
		text += desc.DeviceName;
		text += L"\n";
		OutputDebugString(text.c_str());

		LogOutputDisplayModes(output, mBackBufferFormat);

		ReleaseCom(output);

		++i;
	}
}

void FRHIDX12::LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format)
{
	UINT count = 0;
	UINT flags = 0;

	// Call with nullptr to get list count.
	output->GetDisplayModeList(format, flags, &count, nullptr);

	std::vector<DXGI_MODE_DESC> modeList(count);
	output->GetDisplayModeList(format, flags, &count, &modeList[0]);

	for (auto& x : modeList)
	{
		UINT n = x.RefreshRate.Numerator;
		UINT d = x.RefreshRate.Denominator;
		std::wstring text =
			L"Width = " + std::to_wstring(x.Width) + L" " +
			L"Height = " + std::to_wstring(x.Height) + L" " +
			L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
			L"\n";

		::OutputDebugString(text.c_str());
	}
}



