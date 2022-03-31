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
