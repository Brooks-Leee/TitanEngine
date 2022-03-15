#include "stdafx.h"
#include "ResourceManager.h"
#include "TitanEngine.h"




ResourceManager::ResourceManager()
{

}

ResourceManager::~ResourceManager()
{

}

void ResourceManager::LoadAllActorInMap(const std::string& FilePath)
{
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
			scene->SceneDataArr.push_back(scene->SceneData);

			if (AllMeshData.find(scene->SceneData.AssetPath) == AllMeshData.end() )
			{
				std::string FilePath = "Assets\\StaticMesh\\" + scene->SceneData.AssetPath;
				StaticMesh* staticMesh = LoadStaticMesh(FilePath);
				FMeshData* meshData = staticMesh->GetStaticMesh();
				AllMeshData.insert(std::pair<std::string, FMeshData*>(scene->SceneData.AssetPath, meshData));

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

		
	}
	ifs.close();
	return staticMesh;
}

std::map<std::string, FMeshData*> ResourceManager::getAllMeshData()
{
	return AllMeshData;
}
