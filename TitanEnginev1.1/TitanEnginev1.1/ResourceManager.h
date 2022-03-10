#pragma once
#include "StaticMesh.h"
#include "Scene.h"

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

public:
	void LoadAllActorInMap(const std::string& FilePath);
	StaticMesh* LoadBinaryFile(const std::string& FilePath);


	std::map<std::string, FMeshData*> getAllMeshData();
	Scene* getScene();

private:
	Scene* scene;
	StaticMesh* staticMesh;
	std::map<std::string, FMeshData*> AllMeshData ;
};

