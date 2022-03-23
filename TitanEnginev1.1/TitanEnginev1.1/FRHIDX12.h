#pragma once
#include "FRHI.h"
#include "Scene.h"
#include "stdafx.h"
#include "TTextureDX12.h"
#include "TMaterial.h"

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

struct ObjectConstants
{
	glm::mat4x4 WorldViewProj = MathHelper::Identity4x4glm();
	glm::mat4x4 Location;
	glm::mat4x4 Rotation;
	glm::mat4x4 Scale;
	float gTime;
};


class FRHIDX12 : public FRHI
{
public:

	virtual void InitRHI(Scene* scene) override;
	virtual void CreateMeshBuffer(FMeshData* meshData) override;
	virtual void CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index) override;
	virtual void CreateMaterials() override;
	virtual void CreateConstantBuffer() override;
	

	virtual void SetViewPort(float TopLeftX, float TopLeftY, float Width, float Height, float MinDepth, float MaxDepth) override;
	virtual void SetScissorRects(int ClientWidth, int ClientHeight) override;
	virtual void SetMeshBuffer() override;
	virtual void SetRenderTarget() override;

	virtual void UpdateObjectCB(FSceneData actor) override;
	virtual void UpdateMaterialCB() override;
 	virtual void Draw(FSceneData actor) override;
	virtual void EndFrame() override;




public:
	bool Initialize();

public:
	void UpdateObjectCB(const GameTimer& gt);
	void Draw(const GameTimer& gt);

	void OnResize();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	void FlushCommandQueue();

	void UpdateScene();

	void BuildDescriptorHeaps();
	void BuildDescriptor();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildGeometry();
	void BuildPSO();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	void CalculateFrameStats();
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 6> GetStaticSamplers();

	void ResetCommandList();
	void CloseCommandList();


	bool      m4xMsaaState = false;    // 4X MSAA enabled
	UINT      m4xMsaaQuality = 0;      // quality level of 4X MSAA
	GameTimer mTimer;
	Microsoft::WRL::ComPtr<IDXGIFactory4> mdxgiFactory;
	Microsoft::WRL::ComPtr<IDXGISwapChain> mSwapChain;
	Microsoft::WRL::ComPtr<ID3D12Device> md3dDevice;

	Microsoft::WRL::ComPtr<ID3D12Fence> mFence;
	UINT64 mCurrentFence = 0;

	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDirectCmdListAlloc;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mRtvHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDsvHeap;

	D3D12_VIEWPORT mScreenViewport;
	D3D12_RECT mScissorRect;

	UINT mRtvDescriptorSize = 0;
	UINT mDsvDescriptorSize = 0;
	UINT mCbvSrvUavDescriptorSize = 0;

	// Derived class should set these in derived constructor to customize starting values.
	D3D_DRIVER_TYPE md3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
	DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	int mClientWidth = 800;
	int mClientHeight = 600;
	glm::vec3 mCameraloc;


private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvSrvHeap = nullptr;
	ComPtr<ID3D12DescriptorHeap> mSrvHeap = nullptr;

	std::vector<ComPtr<ID3D12DescriptorHeap>> mCbvHeapArr;

	Scene* mScene = nullptr;

	std::map<std::string, int> mTexTableIndex;
	std::map<std::string, std::shared_ptr<TTextureDX12>> mTextures;

	std::unique_ptr<MeshGeometry> mGeo = nullptr;
	std::vector<std::shared_ptr<MeshGeometry>> mGeoArr;
	std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> mGeoMap;

	std::unordered_map<std::string, std::shared_ptr<TMaterial>> mMaterials;


	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;
	ComPtr<ID3DBlob> mvsByteCodeWPO = nullptr;
	ComPtr<ID3DBlob> mpsByteCodeWPO = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;


	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> mMaterialCB = nullptr;
	UINT matCBByteSize = 0;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadBufferArr;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	glm::mat4x4 mWorld = MathHelper::Identity4x4glm();
	glm::mat4x4 mView = MathHelper::Identity4x4glm();
	glm::mat4x4 mProj = MathHelper::Identity4x4glm();

	POINT mLastMousePos;

	int mLastElementCount = 0;
	int mCurrentElementCount = 100;



	FMeshData* mLoadedStruct;




};

