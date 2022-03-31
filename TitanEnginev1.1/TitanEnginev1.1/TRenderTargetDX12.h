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

public:
	CD3DX12_CPU_DESCRIPTOR_HANDLE currentBackBufferView;
};

