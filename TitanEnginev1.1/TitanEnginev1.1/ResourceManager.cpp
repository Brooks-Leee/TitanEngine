#include "stdafx.h"
#include "ResourceManager.h"
#include "TitanEngine.h"
#include "TLight.h"
#include "MathHelper.h"
#include "Actor.h"

ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::LoadAssests()
{
	LoadAllActorInMap();
	LoadTextures();
	LoadLights();
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
			Actor* actor = new Actor();
			ifs.read((char*)&actor->Transform, sizeof(FTransform));
			int32_t StrLen = 0;
			ifs.read((char*)&StrLen, sizeof(int32_t));
			actor->AssetPath.resize(StrLen);
			ifs.read((char*)actor->AssetPath.data(), sizeof(char) * StrLen);
			actor->AssetPath.erase(actor->AssetPath.size() - 1, 1);
			actor->HeapIndex = i;
			scene->SceneDataArr.push_back(actor);



			if (mAllMeshData.find(actor->AssetPath) == mAllMeshData.end() )
			{
				std::string FilePath = "Assets\\StaticMesh\\" + actor->AssetPath;
				StaticMesh* staticMesh = LoadStaticMesh(FilePath);
				FMeshData* meshData = staticMesh->GetStaticMesh();
				mAllMeshData.insert(std::pair<std::string, FMeshData*>(actor->AssetPath, meshData));

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

void ResourceManager::LoadLights()
{	

	Scene* scene = TitanEngine::Get()->GetSceneIns();
	std::ifstream ifs("Assets/Lights/directionLight.titan", std::ios::binary);
	if (ifs.is_open())
	{
		ifs.read((char*)&scene->light->LightColor, sizeof(glm::vec4));
		ifs.read((char*)&scene->light->LightDirection, sizeof(glm::vec3));
		ifs.read((char*)&scene->light->Intensity, sizeof(float));
	}
}

std::map<std::string, FMeshData*> ResourceManager::getAllMeshData()
{
	return mAllMeshData;
}

std::vector<std::shared_ptr<TTexTure>> ResourceManager::getTextures()
{
	return mTextures;
}
