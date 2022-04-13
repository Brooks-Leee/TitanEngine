#include "stdafx.h"
#include "FRHIDX12.h"
#include "Win32Window.h"
#include "TitanEngine.h"
#include "DDSTextureLoader.h"
#include "TLight.h"
#include "ShadowMapDX12.h"

FRHIDX12* FRHIDX12::mFRHIDX12 = nullptr;


FRHIDX12::FRHIDX12()
{
	if (mFRHIDX12 ==nullptr)
	{
		mFRHIDX12 = this;
	}
}

FRHIDX12::~FRHIDX12()
{
	delete mFRHIDX12;
	mFRHIDX12 = nullptr;
}

void FRHIDX12::InitRHI(Scene* scene)
{
	mScene = scene;
	FRHIDX12::Initialize();

}

void FRHIDX12::ResetCommand()
{
	ResetCommandList();
}

void FRHIDX12::ExecuteCommand()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
	FlushCommandQueue();
}


TShader* FRHIDX12::CreateShader(std::string shaderName)
{
	TShaderDX12* shaderDX = new TShaderDX12();

	//shaderDX->mvsByteCode = d3dUtil::CompileShader("Assets\\Shaders\\" + shaderName.c_str(),  nullptr, "VS", "vs_5_0");
	std::string vsShader = "Assets\\Shaders\\" + shaderName + ".hlsl";
	std::string psShader = "Assets\\Shaders\\" + shaderName + ".hlsl";
	
	shaderDX->mvsByteCode = d3dUtil::CompileShader(d3dUtil::SToWS(vsShader), nullptr, "VS", "vs_5_0");
	shaderDX->mpsByteCode = d3dUtil::CompileShader(d3dUtil::SToWS(psShader), nullptr, "PS", "ps_5_0");

	mInputLayout =
	{
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 28, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TANGENTX", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 44, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 60, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	return shaderDX;
}

TPipeline* FRHIDX12::CreatePipelineState(TShader* shader, std::string shaderName)
{
	TShaderDX12* shaderdx = static_cast<TShaderDX12*>(shader);
	TPipelineDX12* PSO = new TPipelineDX12();

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
	ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	psoDesc.InputLayout = { mInputLayout.data(), (UINT)mInputLayout.size() };
	psoDesc.pRootSignature = mRootSignature.Get();

	psoDesc.VS =
	{
		reinterpret_cast<BYTE*>(shaderdx->mvsByteCode->GetBufferPointer()),
		shaderdx->mvsByteCode->GetBufferSize()
	};
	psoDesc.PS =
	{
		reinterpret_cast<BYTE*>(shaderdx->mpsByteCode->GetBufferPointer()),
		shaderdx->mpsByteCode->GetBufferSize()
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
	PSO->PSOType = "opaque";
	ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&PSO->mPSO)));

	if (shaderName == "shadow")
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC smapPsoDesc = psoDesc;
		smapPsoDesc.RasterizerState.DepthBias = 100000;
		smapPsoDesc.RasterizerState.DepthBiasClamp = 0.0f;
		smapPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
		smapPsoDesc.pRootSignature = mRootSignature.Get();

		smapPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
		smapPsoDesc.NumRenderTargets = 0;
		PSO->PSOType = "shadow_opaque";
		ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&smapPsoDesc, IID_PPV_ARGS(&PSO->mPSO)));
	}
	
	else if (shaderName == "hdr")
	{
		D3D12_GRAPHICS_PIPELINE_STATE_DESC hdrPsoDesc = psoDesc;
		hdrPsoDesc.RTVFormats[0] = DXGI_FORMAT_R16G16B16A16_FLOAT;
		PSO->PSOType = "hdr";
		ThrowIfFailed(md3dDevice->CreateGraphicsPipelineState(&hdrPsoDesc, IID_PPV_ARGS(&PSO->mPSO)));
	}
	return PSO;
}

TMaterial* FRHIDX12::CreateMaterial(std::string name, TShader* shader, TTexTure* texture, int matIndex)
{
	TMaterial* material = new TMaterial();
	TTextureDX12* textureDX = static_cast<TTextureDX12*>(texture);

	material->shader = shader;
	material->textures.push_back(texture);
	material->name = name;
	material->diffuseSrvHeapIndex = textureDX->TexIndex;
	material->matCBIndex = matIndex;

	material->diffuseAlbedo = glm::vec4(.5f, .5f, .5f, 1.0f);
	material->fresnelR0 = glm::vec3(0.2f, 0.2f, 0.2f);
	material->roughness = 0.1f;

	return material;
}

void FRHIDX12::CreateCbvSrvHeap()
{
	D3D12_DESCRIPTOR_HEAP_DESC cbvAndsrvDesc;
	cbvAndsrvDesc.NumDescriptors = 100;
	cbvAndsrvDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	cbvAndsrvDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	cbvAndsrvDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(&cbvAndsrvDesc, IID_PPV_ARGS(&mCbvSrvHeap)));

}

 StaticMesh* FRHIDX12::CreateMeshBuffer(FMeshData* meshData)
{

	auto Geo = new TMeshBufferDX12();
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

		vertices[i].TangentX.x = meshData->tangentx[i].x;
		vertices[i].TangentX.y = meshData->tangentx[i].y;
		vertices[i].TangentX.z = meshData->tangentx[i].z;
		vertices[i].TangentX.w = meshData->tangentx[i].w;

		vertices[i].Texcoord.x = meshData->texcoords[i].u;
		vertices[i].Texcoord.y = meshData->texcoords[i].v;
	}

	std::vector<uint32_t> indices;
	indices.resize(meshData->indices.size());
	indices = meshData->indices;

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

	Geo->StaticMeshInfo.AssetPath = meshData->AssetPath;
	//mCurrentElementCount++;
	return Geo;
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

TRenderTarget* FRHIDX12::CreateRenderTarget(RENDERBUFFER_TYPE RTType, int Width, int Height)
{
	TRenderTargetDX12* renderTarget = new TRenderTargetDX12();
	if (RTType == COMMAND_RENDER_BUFFER)
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
		for (UINT i = 0; i < renderTarget->SwapChainBufferCount; i++)
		{
			ThrowIfFailed(mSwapChain->GetBuffer(i, IID_PPV_ARGS(&renderTarget->mSwapChainBuffer[i])));
			md3dDevice->CreateRenderTargetView(renderTarget->mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
			rtvHeapHandle.Offset(1, mRtvDescriptorSize);
		}

		// Create the depth/stencil buffer and view.
		D3D12_RESOURCE_DESC depthStencilDesc;
		depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		depthStencilDesc.Alignment = 0;
		depthStencilDesc.Width = Width;
		depthStencilDesc.Height = Height;
		depthStencilDesc.DepthOrArraySize = 1;
		depthStencilDesc.MipLevels = 1;
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
			IID_PPV_ARGS(renderTarget->mDepthStencilBuffer.GetAddressOf())));

		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = mDepthStencilFormat;
		dsvDesc.Texture2D.MipSlice = 0;
		md3dDevice->CreateDepthStencilView(renderTarget->mDepthStencilBuffer.Get(), &dsvDesc, DepthStencilView());

		renderTarget->mCpuDsv = DepthStencilView();

		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTarget->mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE));
	}
	else if (RTType == DEPTHSTENCIL_BUFFER)
	{
		// Create Resource

		renderTarget->isShadowMap = true;
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = Width;
		texDesc.DepthOrArraySize = 1;
		texDesc.Height = Height;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		D3D12_CLEAR_VALUE optCleard;
		optCleard.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		optCleard.DepthStencil.Depth = 1.0f;
		optCleard.DepthStencil.Stencil = 0;

		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optCleard,
			IID_PPV_ARGS(&renderTarget->mDepthStencilBuffer)));



		// allocate heap memory for shadow map pass
		auto CbvSrvCPUHeapStart = mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart();
		auto CbvSrvGPUHeapStart = mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart();
		auto DsvCPUStart = mDsvHeap->GetCPUDescriptorHandleForHeapStart();

		UINT descriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		UINT dsvDesSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


		renderTarget->mCpuSrv = CD3DX12_CPU_DESCRIPTOR_HANDLE(CbvSrvCPUHeapStart, 50, descriptorSize);
		renderTarget->mGpuSrv = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvGPUHeapStart, 50, descriptorSize);
		renderTarget->mCpuDsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(DsvCPUStart, 1, dsvDesSize);

		// Create SRV to resource so we can sample the shadow map in a shader program.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		md3dDevice->CreateShaderResourceView(renderTarget->mDepthStencilBuffer.Get(), &srvDesc, renderTarget->mCpuSrv);

		// Create DSV to resource so we can render to the shadow map.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDescd;
		dsvDescd.Flags = D3D12_DSV_FLAG_NONE;
		dsvDescd.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDescd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDescd.Texture2D.MipSlice = 0;
		md3dDevice->CreateDepthStencilView(renderTarget->mDepthStencilBuffer.Get(), &dsvDescd, renderTarget->mCpuDsv);
	}

	else if (RTType == HDR_RENDER_BUFFER)
	{
		
		renderTarget->mCurrBackBuffer = 1;
		renderTarget->mRTVindex = 2;
		// Create renderTarget Resource and view
		D3D12_RESOURCE_DESC texDesc;
		ZeroMemory(&texDesc, sizeof(D3D12_RESOURCE_DESC));
		texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
		texDesc.Alignment = 0;
		texDesc.Width = Width;
		texDesc.DepthOrArraySize = 1;
		texDesc.Height = Height;
		texDesc.MipLevels = 1;
		texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		texDesc.SampleDesc.Count = 1;
		texDesc.SampleDesc.Quality = 0;
		texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
		texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

		D3D12_CLEAR_VALUE optCleard;
		optCleard.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		optCleard.DepthStencil.Depth = 1.0f;
		optCleard.DepthStencil.Stencil = 0;

		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&texDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&optCleard,
			IID_PPV_ARGS(&renderTarget->mSwapChainBuffer[1])));

		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
		rtvHeapHandle.Offset(2, mRtvDescriptorSize);

		D3D12_RENDER_TARGET_VIEW_DESC rtvDesc;

		rtvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;

		rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvDesc.Texture2D.PlaneSlice = 0;
		rtvDesc.Texture2D.MipSlice = 0;

		md3dDevice->CreateRenderTargetView(renderTarget->mSwapChainBuffer[1].Get(), &rtvDesc, rtvHeapHandle);


		// create depthStencil resource and view
		D3D12_RESOURCE_DESC hdrDepthDesc;
		D3D12_CLEAR_VALUE hdrCleardDesc;

		hdrDepthDesc = texDesc;
		hdrCleardDesc = optCleard;

		hdrDepthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
		hdrDepthDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

		hdrCleardDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

		ThrowIfFailed(md3dDevice->CreateCommittedResource(
			&CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT),
			D3D12_HEAP_FLAG_NONE,
			&hdrDepthDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			&hdrCleardDesc,
			IID_PPV_ARGS(&renderTarget->mDepthStencilBuffer)));

		auto CbvSrvCPUHeapStart = mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart();
		auto CbvSrvGPUHeapStart = mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart();
		auto DsvCPUStart = mDsvHeap->GetCPUDescriptorHandleForHeapStart();

		UINT descriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		UINT dsvDesSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);


		renderTarget->mCpuSrv = CD3DX12_CPU_DESCRIPTOR_HANDLE(CbvSrvCPUHeapStart, 51, descriptorSize);
		renderTarget->mGpuSrv = CD3DX12_GPU_DESCRIPTOR_HANDLE(CbvSrvGPUHeapStart, 51, descriptorSize);
		renderTarget->mCpuDsv = CD3DX12_CPU_DESCRIPTOR_HANDLE(DsvCPUStart, 2, dsvDesSize);

		// Create SRV to resource so we can sample the HDR RT in shader.
		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
		srvDesc.Texture2D.PlaneSlice = 0;
		md3dDevice->CreateShaderResourceView(renderTarget->mSwapChainBuffer[1].Get(), &srvDesc, renderTarget->mCpuSrv);

		// Create DSV to resource so we can render to it.
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDescd;
		dsvDescd.Flags = D3D12_DSV_FLAG_NONE;
		dsvDescd.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDescd.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		dsvDescd.Texture2D.MipSlice = 0;
		md3dDevice->CreateDepthStencilView(renderTarget->mDepthStencilBuffer.Get(), &dsvDescd, renderTarget->mCpuDsv);

	}
		
	return renderTarget;

}

void FRHIDX12::EndDraw()
{
	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
}

TTexTure* FRHIDX12::CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index)
{
	TTextureDX12* TextureDX = new TTextureDX12();

	TextureDX->Filename = Texture->Filename;
	TextureDX->Name = Texture->Name;
	TextureDX->TexIndex = mCurrentElementCount + index;
	ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(md3dDevice.Get(), mCommandList.Get(), TextureDX->Filename.c_str(), TextureDX->Resource, TextureDX->UploadHeap));
	
	auto Tex = TextureDX->Resource;

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.Format = Tex->GetDesc().Format;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels = -1;

	auto heapCPUHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetCPUDescriptorHandleForHeapStart());
	UINT DescriptorSize = md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	heapCPUHandle.Offset(TextureDX->TexIndex, DescriptorSize);
	
	TextureDX->cpuHandle = heapCPUHandle;

	md3dDevice->CreateShaderResourceView(Tex.Get(), &srvDesc, heapCPUHandle);

	return TextureDX;
}

//void FRHIDX12::CreateMaterials()
//{
//	auto water = std::make_shared<TMaterial>();
//	water->name = "water";
//	water->diffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	water->fresnelR0 = glm::vec3(0.2f, 0.2f, 0.2f);
//	water->roughness = 0.0f;
//	water->diffuseSrvHeapIndex = mTextures["waterTex"]->TexIndex;
//	water->matCBIndex = 0;
//
//	auto rock = std::make_shared<TMaterial>();
//	rock->name = "rock";
//	rock->diffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	rock->fresnelR0 = glm::vec3(0.01f, 0.01f, 0.01f);
//	rock->roughness = 0.125f;
//	rock->diffuseSrvHeapIndex = mTextures["rockTex"]->TexIndex;
//	rock->matCBIndex = 1;
//
//	auto brick = std::make_shared<TMaterial>();
//	brick->name = "brick";
//	brick->diffuseAlbedo = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
//	brick->fresnelR0 = glm::vec3(0.1f, 0.1f, 0.1f);
//	brick->roughness = 0.25f;
//	brick->diffuseSrvHeapIndex = mTextures["brickTex"]->TexIndex;
//	brick->matCBIndex = 2;
//
//	mMaterials["water"] = std::move(water);
//	mMaterials["rock"] = std::move(rock);
//	mMaterials["brick"] = std::move(brick);
//
//}



void FRHIDX12::SetViewPortAndRects(TViewPort& viewport)
{

	mScreenViewport.TopLeftX = viewport.TopLeftX;
	mScreenViewport.TopLeftY = viewport.TopLeftY;
	mScreenViewport.Width = viewport.Width;
	mScreenViewport.Height = viewport.Height;
	mScreenViewport.MinDepth = viewport.MinDepth;
	mScreenViewport.MaxDepth = viewport.MaxDepth;

	mScissorRect = { 0, 0, viewport.ClientWidth, viewport.ClientHeight };

	mCommandList->RSSetViewports(1, &mScreenViewport);
	mCommandList->RSSetScissorRects(1, &mScissorRect);
}



void FRHIDX12::SetPrimitiveTopology(PRIMITIVE_TOPOLOGY primitiveTolology)
{
	mCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY(primitiveTolology));
}

void FRHIDX12::SetMeshBuffer(Primitive* actor)
{
	TMeshBufferDX12* DXMesh = static_cast<TMeshBufferDX12*>(actor->MeshBuffer);
	mCommandList->IASetVertexBuffers(0, 1, &DXMesh->VertexBufferView());
	mCommandList->IASetIndexBuffer(&DXMesh->IndexBufferView());
}

void FRHIDX12::DrawMesh(Primitive* actor)
{
	TMeshBufferDX12* DXMeshBuffer = static_cast<TMeshBufferDX12*>(actor->MeshBuffer);
	mCommandList->DrawIndexedInstanced(DXMeshBuffer->DrawArgs[DXMeshBuffer->Name].IndexCount, 1, 0, 0, 0);
}

void FRHIDX12::ChangeResourceState(TRenderTarget* renderTarget, RESOURCE_STATE stateBefore, RESOURCE_STATE stateAfter)
{
	D3D12_RESOURCE_STATES stateBeforeDX;
	D3D12_RESOURCE_STATES stateAfterDX;
	
	switch (stateBefore)
	{
	case RESOURCE_STATE_RENDER_TARGET:
		stateBeforeDX = D3D12_RESOURCE_STATE_RENDER_TARGET;
		break;
	case RESOURCE_STATE_PRESENT:
		stateBeforeDX = D3D12_RESOURCE_STATE_PRESENT;
		break;
	case RESOURCE_STATE_DEPTH_WRITE:
		stateBeforeDX = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		break;
	case RESOURCE_STATE_GENERIC_READ:
		stateBeforeDX = D3D12_RESOURCE_STATE_GENERIC_READ;
		break;
	default:
		break;
	}

	switch (stateAfter)
	{
	case RESOURCE_STATE_RENDER_TARGET:
		stateAfterDX = D3D12_RESOURCE_STATE_RENDER_TARGET;
		break;
	case RESOURCE_STATE_PRESENT:
		stateAfterDX = D3D12_RESOURCE_STATE_PRESENT;
		break;
	case RESOURCE_STATE_DEPTH_WRITE:
		stateAfterDX = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		break;
	case RESOURCE_STATE_GENERIC_READ:
		stateAfterDX = D3D12_RESOURCE_STATE_GENERIC_READ;
		break;
	default:
		break;
	}

	TRenderTargetDX12* renderTargetDx = static_cast<TRenderTargetDX12*>(renderTarget);
	if (renderTargetDx->mRTVindex == 2)
	{
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDx->mSwapChainBuffer[renderTargetDx->mCurrBackBuffer].Get(), stateBeforeDX, stateAfterDX));
	}
	else if (renderTargetDx->mSwapChainBuffer->Get() == nullptr)
	{
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDx->mDepthStencilBuffer.Get(), stateBeforeDX, stateAfterDX));
	}
	else
	{
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDx->mSwapChainBuffer[renderTargetDx->mCurrBackBuffer].Get(), stateBeforeDX, stateAfterDX));
	}

}




void FRHIDX12::BeginFrame()
{
	ThrowIfFailed(mDirectCmdListAlloc->Reset());
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
}

void FRHIDX12::UpdateObjectCB(Primitive* actor, GameTimer& gt, TSceneRender* sceneRender)
{
	ObjectConstants objConstants;
	mScene->camera.UpdateViewMat();

	mView = mScene->camera.GetView4x4();
	mProj = mScene->camera.GetProj4x4();
	mCameraloc = mScene->camera.GetCameraPos3f();


	auto& t = actor->Transform;
	auto& s = t.scale;
	auto& r = t.rotation;
	auto& l = t.location;

	auto scale = glm::scale(MathHelper::Identity4x4glm(), glm::vec3(s.x, s.y, s.z));
	//auto rotation = glm::transpose(glm::mat4_cast(glm::quat(r.w, r.pitch, r.yaw, r.roll)));
	auto rotation = glm::mat4_cast(glm::quat(r.w, r.pitch, r.yaw, r.roll));
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
	objConstants.gTime = gt.TotalTime();
	objConstants.Location = location;
	objConstants.Rotation = rotation;
	objConstants.Scale = scale;
	objConstants.light.LightColor = sceneRender->LightMap["dirLight"]->LightColor;
	objConstants.light.LightDirection = sceneRender->LightMap["dirLight"]->LightDirection;
	objConstants.light.Intensity = sceneRender->LightMap["dirLight"]->Intensity;
	// put the constant object into constant buffer which is a Upload Buffer
	mObjectCB->CopyData(actor->PrimitiveMVPIndex, objConstants);

}

void FRHIDX12::UpdateMaterialCB(Primitive* primitive)
{
	MaterialConstants matConstants;
	auto mat = primitive->Material;

	matConstants.diffuseAlbedo = mat->diffuseAlbedo;
	matConstants.fresnelR0 = mat->fresnelR0;
	matConstants.roughness = mat->roughness;
	mMaterialCB->CopyData(mat->matCBIndex, matConstants);

}

void FRHIDX12::UpdateShadowPass(TSceneRender* sceneRender)
{

	
	ShadowPassConstants ShadowConstant;
	ShadowConstant.lightVP = sceneRender->LightMap["dirLight"]->lightVP;
	ShadowConstant.lightTVP = sceneRender->LightMap["dirLight"]->lightTVP;

	mShadowPassCB->CopyData(0, ShadowConstant);
}



void FRHIDX12::SetRenderTarget(TRenderTarget* renderTarget)
{
	TRenderTargetDX12* renderTargetDx = static_cast<TRenderTargetDX12*>(renderTarget);
	
	if (renderTargetDx->mRTVindex == 2)
	{
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDx->mSwapChainBuffer[renderTargetDx->mCurrBackBuffer].Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET));
		float rtvColor[4] = { 1.0f, 0.9f, 0.8f, 1.0f };
		mCommandList->ClearDepthStencilView(renderTargetDx->DSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(renderTargetDx->mCurrBackBuffer + 1), rtvColor, 0, nullptr);
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(renderTargetDx->mCurrBackBuffer + 1), true, &renderTargetDx->DSV());
	}

	else if (renderTargetDx->mSwapChainBuffer->Get() == nullptr)
	{
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDx->mDepthStencilBuffer.Get(),
			D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE));
		mCommandList->ClearDepthStencilView(renderTargetDx->DSV(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		mCommandList->OMSetRenderTargets(0, nullptr, false, &renderTargetDx->DSV());
	}

	else
	{
		mCommandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(renderTargetDx->mSwapChainBuffer[renderTargetDx->mCurrBackBuffer].Get(),
			D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
		float rtvColor[4] = { 1.0f, 0.9f, 0.8f, 1.0f };
		mCommandList->ClearDepthStencilView(DepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);
		mCommandList->ClearRenderTargetView(CurrentBackBufferView(renderTargetDx->mCurrBackBuffer), rtvColor, 0, nullptr);
		mCommandList->OMSetRenderTargets(1, &CurrentBackBufferView(renderTargetDx->mCurrBackBuffer), true, &DepthStencilView());
	}

	ID3D12DescriptorHeap* descriptorHeaps[] = { mCbvSrvHeap.Get() };
	mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);
}

void FRHIDX12::SetPipelineState(TPipeline* pipeline)
{
	TPipelineDX12* PSO = static_cast<TPipelineDX12*>(pipeline);
	mCommandList->SetPipelineState(PSO->mPSO.Get());
}

void FRHIDX12::SetShaderData(Primitive* actor, TRenderTarget* renderTarget)
{
	TRenderTargetDX12* renderTargetDx = static_cast<TRenderTargetDX12*>(renderTarget);
	TMeshBufferDX12* DXMeshBuffer = static_cast<TMeshBufferDX12*>(actor->MeshBuffer);


	mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

	auto heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	heapGPUHandle.Offset(actor->PrimitiveMVPIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mCommandList->SetGraphicsRootDescriptorTable(0, heapGPUHandle);


	//if (DXMeshBuffer->Name == "Plane.titan")
	//{
	//	heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	//	heapGPUHandle.Offset(mTextures["waterTex"]->TexIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//	mCommandList->SetGraphicsRootDescriptorTable(1, heapGPUHandle);
	//	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = mMaterialCB->Resource()->GetGPUVirtualAddress() + mMaterials["water"]->matCBIndex * matCBByteSize;
	//	mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);

	//}
	//else if (DXMeshBuffer->Name == "SM_MatPreviewMesh_02.titan")
	//{
	//	heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	//	heapGPUHandle.Offset(mTextures["rockTex"]->TexIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	//	mCommandList->SetGraphicsRootDescriptorTable(1, heapGPUHandle);
	//	D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = mMaterialCB->Resource()->GetGPUVirtualAddress() + mMaterials["rock"]->matCBIndex * matCBByteSize;
	//	mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);
	//}
	//else


	{

		int texIndex = actor->Material->diffuseSrvHeapIndex;
		int normalMapIndex = actor->Material->normalMapIndex;

		heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
		heapGPUHandle.Offset(texIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(1, heapGPUHandle);

		heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
		heapGPUHandle.Offset(normalMapIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
		mCommandList->SetGraphicsRootDescriptorTable(6, heapGPUHandle);



		D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = mMaterialCB->Resource()->GetGPUVirtualAddress() + actor->Material->matCBIndex * matCBByteSize;
		mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);
	}


	mCommandList->SetGraphicsRoot32BitConstants(2, 3, &mCameraloc, 0);

	auto shadowPassCB = mShadowPassCB->Resource();
	D3D12_GPU_VIRTUAL_ADDRESS shadowPassCBAddress = shadowPassCB->GetGPUVirtualAddress();
	mCommandList->SetGraphicsRootConstantBufferView(4, shadowPassCBAddress);

	if (renderTargetDx->isShadowMap)
	{
		auto shadowMapHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(renderTargetDx->SRV());
		mCommandList->SetGraphicsRootDescriptorTable(5, shadowMapHandle);
	}

}


void FRHIDX12::DrawShadowMap(Primitive* actor)
{
	TMeshBufferDX12* DXMesh = static_cast<TMeshBufferDX12*>(actor->MeshBuffer);

	//auto geo = mGeoMap[actor->AssetPath];
	mCommandList->IASetVertexBuffers(0, 1, &DXMesh->VertexBufferView());
	mCommandList->IASetIndexBuffer(&DXMesh->IndexBufferView());
	mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	auto heapGPUHandle = CD3DX12_GPU_DESCRIPTOR_HANDLE(mCbvSrvHeap->GetGPUDescriptorHandleForHeapStart());
	heapGPUHandle.Offset(actor->PrimitiveMVPIndex, md3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV));
	mCommandList->SetGraphicsRootDescriptorTable(0, heapGPUHandle);


	auto shadowPassCB = mShadowPassCB->Resource();
	//D3D12_GPU_VIRTUAL_ADDRESS shadowPassCBAddress = shadowPassCB->GetGPUVirtualAddress() + actor.HeapIndex * shadowCBByteSize;
	D3D12_GPU_VIRTUAL_ADDRESS shadowPassCBAddress = shadowPassCB->GetGPUVirtualAddress();
	mCommandList->SetGraphicsRootConstantBufferView(4, shadowPassCBAddress);

	mCommandList->DrawIndexedInstanced(DXMesh->DrawArgs[DXMesh->Name].IndexCount, 1, 0, 0, 0);


}




void FRHIDX12::EndFrame(TRenderTarget* renderTarget)
{
	TRenderTargetDX12* renderTargetDx = static_cast<TRenderTargetDX12*>(renderTarget);

	ThrowIfFailed(mCommandList->Close());
	ID3D12CommandList* cmdsLists[] = { mCommandList.Get() };
	mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

	

	ThrowIfFailed(mSwapChain->Present(0, 0));
	renderTargetDx->mCurrBackBuffer = (renderTargetDx->mCurrBackBuffer + 1) % SwapChainBufferCount;

	FlushCommandQueue();
}





bool FRHIDX12::Initialize()
{
	if (!InitDirect3D())
		return false;

	OnResize();

	CreateCbvSrvHeap();
	BuildRootSignature();
	//BuildShadersAndInputLayout();
	//BuildPSO();

	return true;
}

FRHIDX12* FRHIDX12::Get()
{
	return mFRHIDX12;
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
	rtvHeapDesc.NumDescriptors = 10;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NodeMask = 0;
	ThrowIfFailed(md3dDevice->CreateDescriptorHeap(
		&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())));


	D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
	dsvHeapDesc.NumDescriptors = 10;
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





void FRHIDX12::BuildRootSignature()
{
	CD3DX12_DESCRIPTOR_RANGE texTable;
	texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE cbvTable;
	cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
	CD3DX12_DESCRIPTOR_RANGE shadowMapTable;
	shadowMapTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
	CD3DX12_DESCRIPTOR_RANGE normalTable;
	normalTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);



	CD3DX12_ROOT_PARAMETER slotRootParameter[7]; 
	slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);
	slotRootParameter[1].InitAsDescriptorTable(1, &texTable, D3D12_SHADER_VISIBILITY_PIXEL);
	// camera location
	slotRootParameter[2].InitAsConstants(3, 1, 0);
	// Material constant buffer
	slotRootParameter[3].InitAsConstantBufferView(2);
	// shadow map constant buffer
	slotRootParameter[4].InitAsConstantBufferView(3);
	slotRootParameter[5].InitAsDescriptorTable(1, &shadowMapTable);
	slotRootParameter[6].InitAsDescriptorTable(1, &normalTable, D3D12_SHADER_VISIBILITY_PIXEL);


	auto staticSamplers = GetStaticSamplers();
	// A root signature is an array of root parameters.
	CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(7, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(),
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

D3D12_CPU_DESCRIPTOR_HANDLE FRHIDX12::CurrentBackBufferView(int currentBackBuffer) const
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(
		mRtvHeap->GetCPUDescriptorHandleForHeapStart(),
		currentBackBuffer,
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
	ThrowIfFailed(mCommandList->Reset(mDirectCmdListAlloc.Get(), nullptr));
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



