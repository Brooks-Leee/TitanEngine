#pragma once
#include "TShader.h"
class TShaderDX12 :
    public TShader
{
public:
    TShaderDX12();
    ~TShaderDX12();

public:
	Microsoft::WRL::ComPtr<ID3DBlob> mvsByteCode;
	Microsoft::WRL::ComPtr<ID3DBlob> mpsByteCode;
  
};

