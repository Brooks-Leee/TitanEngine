#pragma once
#include "TRHITexture.h"



class GPUResourceManager
{
public:
	std::unordered_map<std::string, std::shared_ptr<TRHITexture>> Textures;

};

