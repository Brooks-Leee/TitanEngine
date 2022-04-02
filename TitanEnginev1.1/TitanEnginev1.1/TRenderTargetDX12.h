#pragma once
#include "TRenderTarget.h"
#include "FRHIDX12.h"

class TRenderTargetDX12 :
    public TRenderTarget
{
public:
	TRenderTargetDX12(){};
	~TRenderTargetDX12(){};

	unsigned __int64 CurrentBackBufferView();
	unsigned __int64 DepthStencilBuffer();

	CD3DX12_GPU_DESCRIPTOR_HANDLE SRV();
	CD3DX12_CPU_DESCRIPTOR_HANDLE DSV();



public:
	CD3DX12_CPU_DESCRIPTOR_HANDLE mCpuSrv;
	CD3DX12_GPU_DESCRIPTOR_HANDLE mGpuSrv;
	CD3DX12_CPU_DESCRIPTOR_HANDLE mCpuDsv;

	static const int SwapChainBufferCount = 2;
	int mCurrBackBuffer = 0;

	Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[SwapChainBufferCount];
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthStencilBuffer;
};

