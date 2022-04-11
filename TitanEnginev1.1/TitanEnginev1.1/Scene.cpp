#include "stdafx.h"
#include "Scene.h"
#include "TitanEngine.h"



Scene::Scene()
{
	light = new TLight();
	actor = new Primitive();
}

Scene::~Scene()
{

	for (auto& actor : SceneDataArr)
	{
		if (actor != nullptr)
		{
			delete actor;
		}
	}
}
