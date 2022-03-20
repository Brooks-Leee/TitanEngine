#pragma once
#include "StaticMesh.h"
#include <map>
#include "TTexTure.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;



class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

public:
	void LoadAllActorInMap();
	void LoadTextures();

	StaticMesh* LoadStaticMesh(const std::string& FilePath);


	std::map<std::string, FMeshData*> getAllMeshData();
	std::vector<std::shared_ptr<TTexTure>> getTextures();


private:
	std::map<std::string, FMeshData*> mAllMeshData;
	//std::unordered_map<std::string, std::shared_ptr<Texture>> mTextures;
	std::vector<std::shared_ptr<TTexTure>> mTextures;
};

