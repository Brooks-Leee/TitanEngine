#pragma once
#include "MyMath.h"
#include "StaticMesh.h"
#include <map>

struct FSceneData
{
	FTransform Transform;
	std::string AssetPath;
};


class Scene
{
public:
	void LoadAllActorInMap(const std::string& FilePath);
	std::vector<FSceneData> GetAllSMInMapInfo();

	
public:
	FSceneData SceneData;
	std::vector<FSceneData> SceneDataArr;
	std::map<std::string, FMeshData*> AllMeshData;
};

