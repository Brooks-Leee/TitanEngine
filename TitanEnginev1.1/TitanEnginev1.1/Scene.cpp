#include "stdafx.h"
#include "Scene.h"
#include "TitanEngine.h"



Scene::Scene()
{
	light = new TLight();
	actor = new Actor();
}

Scene::~Scene()
{
	delete light;
	light = nullptr;
}
