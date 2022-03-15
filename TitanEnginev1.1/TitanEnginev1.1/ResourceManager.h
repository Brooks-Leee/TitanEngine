#pragma once
#include "StaticMesh.h"
#include <map>

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

public:
	void LoadAllActorInMap(const std::string& FilePath);
	StaticMesh* LoadStaticMesh(const std::string& FilePath);


	std::map<std::string, FMeshData*> getAllMeshData();

private:
	std::map<std::string, FMeshData*> AllMeshData ;
};

