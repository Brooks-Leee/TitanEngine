#pragma once
#include "TPipeline.h"

class TPipelineDX12 : public TPipeline
{
public:
	TPipelineDX12();
	 ~TPipelineDX12();

public:
	std::string PSOType;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> mPSO;

};

