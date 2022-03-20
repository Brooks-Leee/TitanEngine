#pragma once
#include "TTexTure.h"
#include "TRHITexture.h"




class TTextureDX12 :
    public TRHITexture
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> Resource = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;

	UINT TexIndex = 0;
};

