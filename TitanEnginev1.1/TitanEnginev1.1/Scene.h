#pragma once
#include "MyMath.h"
#include "StaticMesh.h"
#include <map>
#include "Camera.h"

struct FSceneData
{
	FTransform Transform;
	std::string AssetPath;
};

class Scene
{
public:
	void SpawnSphere();
	void DestorySphere();


public:
	Camera camera;
	FSceneData SceneData;
	std::vector<FSceneData> SceneDataArr;
};

