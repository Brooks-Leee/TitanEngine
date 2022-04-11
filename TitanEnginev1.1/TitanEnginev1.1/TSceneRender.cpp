#include "stdafx.h"
#include "TSceneRender.h"

TSceneRender::~TSceneRender()
{
	for (auto& pipeline : PipelineMap)
	{
		delete pipeline.second;
	}


	for (auto& shader : ShaderMap)
	{
		delete shader.second;
	}

	for (auto& tex : TextureBufferMap)
	{
		if (tex.second != nullptr)
		{
			delete(tex.second);
			tex.second = nullptr;
		}
	}

	for (auto& mat : MaterialMap)
	{
		if (mat.second != nullptr)
		{
			delete mat.second;
			mat.second = nullptr;
		}
	}

	for (auto& light : LightMap)
	{
		if (light.second != nullptr)
		{
			delete light.second;
			light.second = nullptr;
		}
	}

}
