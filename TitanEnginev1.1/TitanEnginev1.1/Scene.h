#pragma once
#include "MyMath.h"
#include "StaticMesh.h"
#include <map>
#include "Camera.h"
#include "TLight.h"


struct FSceneData
{
	FTransform Transform;
	std::string AssetPath;
	UINT HeapIndex;
};

class Scene
{

public:
	Scene();
	~Scene();


public:
	Camera camera;
	FSceneData SceneData;
	std::vector<FSceneData> SceneDataArr;
	TLight* light;
};

