#include "stdafx.h"
#include "TRenderTargetDX12.h"



CD3DX12_GPU_DESCRIPTOR_HANDLE TRenderTargetDX12::SRV()
{
	return mGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE TRenderTargetDX12::DSV()
{
	return mCpuDsv;
}
