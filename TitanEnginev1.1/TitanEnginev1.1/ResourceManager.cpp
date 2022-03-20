#include "stdafx.h"
#include "ResourceManager.h"
#include "TitanEngine.h"




ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::LoadAllActorInMap()
{
	const char* FilePath = "Assets/Map/Map.titan";
	Scene* scene = TitanEngine::Get()->GetSceneIns();
	std::ifstream ifs(FilePath, std::ios::binary);
	if (ifs.is_open())
	{
		int32_t ArrLen = 0;
		ifs.read((char*)&ArrLen, sizeof(int32_t));

		for (int32_t i = 0; i < ArrLen; i++)
		{
			ifs.read((char*)&scene->SceneData.Transform, sizeof(FTransform));

			int32_t StrLen = 0;
			ifs.read((char*)&StrLen, sizeof(int32_t));
			scene->SceneData.AssetPath.resize(StrLen);
			ifs.read((char*)scene->SceneData.AssetPath.data(), sizeof(char) * StrLen);
			scene->SceneData.AssetPath.erase(scene->SceneData.AssetPath.size() - 1, 1);
			scene->SceneData.HeapIndex = i;
			scene->SceneDataArr.push_back(scene->SceneData);



			if (mAllMeshData.find(scene->SceneData.AssetPath) == mAllMeshData.end() )
			{
				std::string FilePath = "Assets\\StaticMesh\\" + scene->SceneData.AssetPath;
				StaticMesh* staticMesh = LoadStaticMesh(FilePath);
				FMeshData* meshData = staticMesh->GetStaticMesh();
				mAllMeshData.insert(std::pair<std::string, FMeshData*>(scene->SceneData.AssetPath, meshData));

			}
		}
	}
}

StaticMesh* ResourceManager::LoadStaticMesh(const std::string& FilePath)
{
	StaticMesh* staticMesh = new StaticMesh;
	std::ifstream ifs(FilePath, std::ios::binary);
	if (ifs.is_open())
	{
	//	std::vector<int32_t> pos;
		ifs.read((char*)&staticMesh->StaticMeshInfo.VerticesNum, sizeof(int32_t));
	//	pos.push_back(ifs.tellg());
		ifs.read((char*)&staticMesh->StaticMeshInfo.TriangleNum, sizeof(int32_t));
	//	pos.push_back(ifs.tellg());
		ifs.read((char*)&staticMesh->StaticMeshInfo.IndicesNum, sizeof(int32_t));
	//	pos.push_back(ifs.tellg());
		ifs.read((char*)&staticMesh->StaticMeshInfo.LODNum, sizeof(int32_t));
	//	pos.push_back(ifs.tellg());

		int32_t strLen = 0;
		ifs.read((char*)&strLen, sizeof(int32_t));
		staticMesh->StaticMeshInfo.AssetPath.resize(strLen);
		ifs.read((char*)staticMesh->StaticMeshInfo.AssetPath.data(), strLen * sizeof(char));
	//	pos.push_back(ifs.tellg());

		int32_t verticesLen = 0;
		ifs.read((char*)&verticesLen, sizeof(int32_t));
		staticMesh->StaticMeshInfo.Vertices.resize(verticesLen);
		//pos.push_back(sizeof(FVector));
		ifs.read((char*)staticMesh->StaticMeshInfo.Vertices.data(), verticesLen * sizeof(FVector));
	//	pos.push_back(ifs.tellg());

		int32_t indicesLen = 0;
		ifs.read((char*)&indicesLen, sizeof(int32_t));
		staticMesh->StaticMeshInfo.indices.resize(indicesLen);
		ifs.read((char*)staticMesh->StaticMeshInfo.indices.data(), indicesLen * sizeof(uint32_t));
	//	pos.push_back(ifs.tellg());

		int32_t normalsLen = 0;
		ifs.read((char*)&normalsLen, sizeof(int32_t));
		staticMesh->StaticMeshInfo.normals.resize(normalsLen);
		ifs.read((char*)staticMesh->StaticMeshInfo.normals.data(), normalsLen * sizeof(FVector4));
	//	pos.push_back(ifs.tellg());

		int32_t uvLength = 0;
		ifs.read((char*)&uvLength, sizeof(int32_t));
		staticMesh->StaticMeshInfo.texcoords.resize(uvLength);
		ifs.read((char*)staticMesh->StaticMeshInfo.texcoords.data(), uvLength * sizeof(FVector2));

		staticMesh->StaticMeshInfo.AssetPath.erase(staticMesh->StaticMeshInfo.AssetPath.size() - 1, 1);

	}
	ifs.close();
	return staticMesh;
}

void ResourceManager::LoadTextures()
{

	//DXRenderer* renderer = TitanEngine::Get()->GetRenderer();
	////renderer->mCommandList->Reset(renderer->mDirectCmdListAlloc.Get(), nullptr);
	//renderer->ResetCommandList();

	//auto waterTex = std::make_shared<Texture>();
	//waterTex->Name = "waterTex";
	//waterTex->Filename = L"Assets/Texture/water.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(renderer->md3dDevice.Get(), renderer->mCommandList.Get(), waterTex->Filename.c_str(), waterTex->Resource, waterTex->UploadHeap));

	//auto rockTex = std::make_shared<Texture>();
	//rockTex->Name = "rockTex";
	//rockTex->Filename = L"Assets/Texture/rock.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(renderer->md3dDevice.Get(), renderer->mCommandList.Get(), rockTex->Filename.c_str(), rockTex->Resource, rockTex->UploadHeap));

	//auto brickTex = std::make_shared<Texture>();
	//brickTex->Name = "brickTex";
	//brickTex->Filename = L"Assets/Texture/brick.dds";
	//ThrowIfFailed(DirectX::CreateDDSTextureFromFile12(renderer->md3dDevice.Get(), renderer->mCommandList.Get(), brickTex->Filename.c_str(), brickTex->Resource, brickTex->UploadHeap));

	//mTextures[waterTex->Name] = std::move(waterTex);
	//mTextures[rockTex->Name] = std::move(rockTex);
	//mTextures[brickTex->Name] = std::move(brickTex);

	auto waterTex = std::make_shared<TTexTure>();
	waterTex->Name = "waterTex";
	waterTex->Filename = L"Assets/Texture/water.dds";
	
	auto rockTex = std::make_shared<TTexTure>();
	rockTex->Name = "rockTex";
	rockTex->Filename = L"Assets/Texture/rock.dds";

	auto brickTex = std::make_shared<TTexTure>();
	brickTex->Name = "brickTex";
	brickTex->Filename = L"Assets/Texture/brick.dds";

	mTextures.push_back(waterTex);
	mTextures.push_back(rockTex);
	mTextures.push_back(brickTex);
}

std::map<std::string, FMeshData*> ResourceManager::getAllMeshData()
{
	return mAllMeshData;
}

std::vector<std::shared_ptr<TTexTure>> ResourceManager::getTextures()
{
	return mTextures;
}
