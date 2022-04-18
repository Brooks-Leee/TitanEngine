#pragma once
#include "FRHI.h"
#include "Scene.h"
#include "stdafx.h"
#include "TTextureDX12.h"
#include "TMaterial.h"
#include "ShadowMapDX12.h"
#include "TMeshBufferDX12.h"
#include "StaticMesh.h"
#include "TRenderTargetDX12.h"
#include "TShaderDX12.h"
#include "TPipelineDX12.h"

using Microsoft::WRL::ComPtr;
using namespace std;
using namespace DirectX;

struct ObjectConstants
{
	glm::mat4x4 WorldViewProj = MathHelper::Identity4x4glm();
	glm::mat4x4 World = MathHelper::Identity4x4glm();
	glm::mat4x4 ViewProj = MathHelper::Identity4x4glm();
	glm::mat4x4 Location;
	glm::mat4x4 Rotation;
	glm::mat4x4 Scale;
	Light light;
	float gTime;
};

struct ShadowPassConstants
{	
	glm::mat4 lightVP;
	glm::mat4 lightTVP;
};

class FRHIDX12 : public FRHI
{
public:

	FRHIDX12();
	~FRHIDX12();

	virtual void InitRHI(Scene* scene) override;
	virtual void ResetCommand() override;
	virtual void ExecuteCommand() override;

	virtual TRenderTarget* CreateRenderTarget(TEX_FORMAT format, int rtvIndex, int srvIndex, int dsvIndex, int Width, int Height) override;
	virtual TRenderTarget* CreateRenderTarget(RENDERBUFFER_TYPE RTType, int Width, int Height) override;

	virtual void SetRenderTargetbloom(TRenderTarget* rendertarget, std::string event) override;
	virtual void SetShaderDatabloom(TRenderTarget* rt1, TRenderTarget* rt2);



	virtual TShader* CreateShader(std::string shaderName) override;
	virtual TPipeline* CreatePipelineState(TShader* shader, std::string shaderName, TEX_FORMAT format) override;
	virtual TMaterial* CreateMaterial(std::string name, TShader* shader, TTexTure* texture, int matIndex) override;
	
	virtual void CreateCbvSrvHeap() override;
	virtual StaticMesh* CreateMeshBuffer(FMeshData* meshData) override;
	virtual TTexTure* CreateTexture(std::shared_ptr<TTexTure> Texture, UINT index) override;
	virtual void CreateConstantBuffer() override;
	virtual void EndDraw() override;

	virtual void BeginFrame() override;
	virtual void UpdateObjectCB(Primitive* primitive, GameTimer& gt, TSceneRender* sceneRender) override;
	virtual void UpdateMaterialCB(Primitive* primitive) override;
	virtual void UpdateShadowPass(TSceneRender* sceneRender) override;
	
	virtual void SetRenderTarget(TRenderTarget* renderTarget, std::string event) override;

	virtual void SetViewPortAndRects(TViewPort& viewport) override;
	virtual void SetPipelineState(TPipeline* pipeline) override;

	virtual void SetPrimitiveTopology(PRIMITIVE_TOPOLOGY primitiveTolology) override;
	virtual void SetMeshBuffer(Primitive* primitive) override;
	virtual void SetShaderData(Primitive* primitive, TRenderTarget* renderTarget) override;
	virtual void DrawMesh(Primitive* primitive) override;
	virtual void ChangeResourceState(TRenderTarget* renderTarget, RESOURCE_STATE stateBefore, RESOURCE_STATE stateAfter) override;


	virtual void EndFrame(TRenderTarget* renderTarget) override;

	virtual void DrawShadowMap(Primitive* primitive) override;


	virtual void EndEvent() override;

public:
	bool Initialize();

public:
	static FRHIDX12* Get();
	void UpdateObjectCB(const GameTimer& gt);
	void Draw(const GameTimer& gt);

	void OnResize();
	bool InitDirect3D();
	void CreateCommandObjects();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();

	void FlushCommandQueue();

	void BuildDescriptor();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildPSO();

	ID3D12Resource* CurrentBackBuffer()const;
	D3D12_CPU_DESCRIPTOR_HANDLE CurrentBackBufferView()const;
	D3D12_CPU_DESCRIPTOR_HANDLE FRHIDX12::CurrentBackBufferView(int CurrentBackBuffer) const;
	D3D12_CPU_DESCRIPTOR_HANDLE DepthStencilView()const;

	void CalculateFrameStats();
	void LogAdapters();
	void LogAdapterOutputs(IDXGIAdapter* adapter);
	void LogOutputDisplayModes(IDXGIOutput* output, DXGI_FORMAT format);
	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

	void ResetCommandList();
	void CloseCommandList();


	static FRHIDX12* mFRHIDX12;
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

	Scene* mScene = nullptr;


	std::unique_ptr<MeshGeometry> mGeo = nullptr;
	std::vector<std::shared_ptr<MeshGeometry>> mGeoArr;
	std::unordered_map<std::string, std::shared_ptr<MeshGeometry>> mGeoMap;

	std::unordered_map<std::string, std::shared_ptr<TMaterial>> mMaterials;

	std::unique_ptr<ShadowMapDX12> mShadowMap = nullptr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;
	ComPtr<ID3DBlob> msmapVSByteCode = nullptr;
	ComPtr<ID3DBlob> msmapPSByteCode = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;


	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	std::unique_ptr<UploadBuffer<MaterialConstants>> mMaterialCB = nullptr;
	std::unique_ptr<UploadBuffer<ShadowPassConstants>> mShadowPassCB = nullptr;

	UINT matCBByteSize = 0;
	UINT shadowCBByteSize = 0;

	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadBufferArr;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;


	glm::mat4x4 mWorld = MathHelper::Identity4x4glm();
	glm::mat4x4 mView = MathHelper::Identity4x4glm();
	glm::mat4x4 mProj = MathHelper::Identity4x4glm();

	POINT mLastMousePos;

	int mLastElementCount = 0;
	int mCurrentElementCount = 20;

	FMeshData* mLoadedStruct;

	glm::vec4 mBloomsetupSize= { 800.0f, 600.0f,800.0f, 600.0f };


};

