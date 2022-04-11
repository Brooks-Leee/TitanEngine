#pragma once
#include "TLight.h"
#include "TShader.h"
#include "TPipeline.h"
#include "TTexTure.h"
#include "TMaterial.h"

class TSceneRender
{
public:
	TSceneRender(){}
	~TSceneRender();

public:
	std::unordered_map<std::string, TLight*> LightMap;
	std::unordered_map<std::string, TShader*> ShaderMap;
	std::unordered_map<std::string, TPipeline*> PipelineMap;
	std::unordered_map<std::string, TTexTure*> TextureBufferMap;
	std::unordered_map<std::string, TMaterial*> MaterialMap;
};

