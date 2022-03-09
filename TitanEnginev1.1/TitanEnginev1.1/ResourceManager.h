#pragma once
#include "StaticMesh.h"
#include "Scene.h"

class ResourceManager
{
public:
	void LoadAllActorInMap(const std::string& FilePath);
	StaticMesh* LoadBinaryFile(const std::string& FilePath);

public:
	Scene* scene = new Scene;
	StaticMesh* staticMesh = new StaticMesh;
	std::map<std::string, FMeshData*> AllMeshData ;
};

