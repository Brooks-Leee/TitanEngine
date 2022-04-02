#include "stdafx.h"
#include "TRenderTargetDX12.h"

unsigned __int64 TRenderTargetDX12::CurrentBackBufferView()
{
	unsigned __int64 ptr = FRHIDX12::Get()->CurrentBackBufferView().ptr;
	return ptr;
}

unsigned __int64 TRenderTargetDX12::DepthStencilBuffer()
{
	return FRHIDX12::Get()->DepthStencilView().ptr;
}

CD3DX12_GPU_DESCRIPTOR_HANDLE TRenderTargetDX12::SRV()
{
	return mGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE TRenderTargetDX12::DSV()
{
	return mCpuDsv;
}
