#pragma once
#include "TLight.h"

class TSceneRender
{
public:
	TSceneRender(){}
	~TSceneRender(){}





public:
	std::unordered_map<std::string, TLight*> LightMap;

};

