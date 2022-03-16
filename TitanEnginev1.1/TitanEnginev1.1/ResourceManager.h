#pragma once
#include "StaticMesh.h"
#include <map>
#include "DDSTextureLoader.h"
#include "d3dUtil.h"

using Microsoft::WRL::ComPtr;
using namespace DirectX;
using namespace DirectX::PackedVector;

//struct Texture
//{
//	std::string Name;
//	std::wstring Filename;
//
//	Microsoft::WRL::ComPtr<ID3D12Resource> Resourece = nullptr;
//	Microsoft::WRL::ComPtr<ID3D12Resource> UploadHeap = nullptr;
//};



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
	std::unordered_map<std::string, std::shared_ptr<Texture>> getTextures();


private:
	std::map<std::string, FMeshData*> mAllMeshData;
	std::unordered_map<std::string, std::shared_ptr<Texture>> mTextures;
};

