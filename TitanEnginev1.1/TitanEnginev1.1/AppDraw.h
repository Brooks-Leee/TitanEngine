#pragma once
#include "stdafx.h"
#include "Scene.h"

using Microsoft::WRL::ComPtr;
//using namespace DirectX;
//using namespace DirectX::PackedVector;


struct ObjectConstants
{
	glm::mat4x4 WorldViewProj = MathHelper::Identity4x4glm();
	float gTime;
	glm::mat4x4 Location;
	glm::mat4x4 Rotation;
	glm::mat4x4 Scale;
};

class AppDraw : public AppInit
{
public:
	AppDraw(HINSTANCE hInstance);
	AppDraw(const AppDraw& rhs) = delete;
	AppDraw& operator=(const AppDraw& rhs) = delete;
	~AppDraw();

	virtual bool Initialize()override;

	void GetLoadedBinaryFile(FMeshData* LoadedStruct);
	void GetLoadedAllActor(std::vector<FSceneData> AllActorArr);

private:
	virtual void OnResize()override;
	virtual void Update(const GameTimer& gt)override;
	virtual void Draw(const GameTimer& gt)override;

	virtual void OnMouseDown(WPARAM btnState, int x, int y)override;
	virtual void OnMouseUp(WPARAM btnState, int x, int y)override;
	virtual void OnMouseMove(WPARAM btnState, int x, int y)override;

	void BuildDescriptorHeaps();
	void BuildConstantBuffers();
	void BuildRootSignature();
	void BuildShadersAndInputLayout();
	void BuildGeometry();
	void BuildPSO();

private:

	ComPtr<ID3D12RootSignature> mRootSignature = nullptr;
	ComPtr<ID3D12DescriptorHeap> mCbvHeap = nullptr;

	std::vector<ComPtr<ID3D12DescriptorHeap>> mCbvHeapArr;

	std::unique_ptr<MeshGeometry> mGeo = nullptr;

	std::unique_ptr<Scene> mAllActor;
	std::vector<std::shared_ptr<MeshGeometry>> mGeoArr;

	ComPtr<ID3DBlob> mvsByteCode = nullptr;
	ComPtr<ID3DBlob> mpsByteCode = nullptr;
	ComPtr<ID3DBlob> mvsByteCodeWPO = nullptr;
	ComPtr<ID3DBlob> mpsByteCodeWPO = nullptr;

	std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;


	std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> uploadBufferArr;

	ComPtr<ID3D12PipelineState> mPSO = nullptr;

	glm::mat4x4 mWorld = MathHelper::Identity4x4glm();
	glm::mat4x4 mView = MathHelper::Identity4x4glm();
	glm::mat4x4 mProj = MathHelper::Identity4x4glm();

	float mTheta = 1.5f * MathHelper::Piglm;
	float mPhi = MathHelper::XM_PIDIV4glm;
	float mRadius = 3000.0f;

	POINT mLastMousePos;

	FMeshData* mLoadedStruct;
	//Scene* mAllActor;
};