#include "stdafx.h"
#include "Scene.h"



void Scene::LoadAllActorInMap(const std::string& FilePath)
{
	std::ifstream ifs(FilePath, std::ios::binary);
	if (ifs.is_open())
	{
		int32_t ArrLen = 0;
		ifs.read((char*)&ArrLen, sizeof(int32_t));

		for (int32_t i = 0; i < ArrLen; i++)
		{
			ifs.read((char*)&SceneData.Transform, sizeof(FTransform));

			int32_t StrLen = 0;
			ifs.read((char*)&StrLen, sizeof(int32_t));
			SceneData.AssetPath.resize(StrLen);
			ifs.read((char*)SceneData.AssetPath.data(), sizeof(char) * StrLen);
			SceneDataArr.push_back(SceneData);

			// put the mesh data into a map
		//	std::unique_ptr<StaticMesh> staticMesh = std::make_unique<StaticMesh>();
			if (AllMeshData.find(SceneData.AssetPath) == AllMeshData.end())
			{
				StaticMesh* staticMesh = new StaticMesh;
				std::string FilePath = "Assets\\StaticMesh\\" + SceneData.AssetPath;
				staticMesh->LoadBinaryFile(FilePath);
				FMeshData* meshData = staticMesh->GetStaticMesh();
				AllMeshData.insert(std::pair<std::string, FMeshData*>(SceneData.AssetPath, meshData));
				//auto test = AllMeshData.find(SceneData.AssetPath)->second->AssetPath;
				//auto test1 = AllMeshData.find(SceneData.AssetPath)->second->Vertices;
			}
		}
	}
}
