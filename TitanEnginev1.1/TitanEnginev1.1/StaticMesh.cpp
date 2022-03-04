#pragma once
#include "stdafx.h"
#include "StaticMesh.h"


void StaticMesh::LoadBinaryFile(const std::string& FilePath)
{
	//std::string testPath = "Assets/StaticMesh/1M_Cube.dat";
	std::ifstream ifs(FilePath, std::ios::binary);
	if (ifs.is_open())
	{
		std::vector<int32_t> pos;
		ifs.read((char*)&StaticMeshInfo.VerticesNum, sizeof(int32_t));
		pos.push_back(ifs.tellg());
		ifs.read((char*)&StaticMeshInfo.TriangleNum, sizeof(int32_t));
		pos.push_back(ifs.tellg());
		ifs.read((char*)&StaticMeshInfo.IndicesNum, sizeof(int32_t));
		pos.push_back(ifs.tellg());
		ifs.read((char*)&StaticMeshInfo.LODNum, sizeof(int32_t));
		pos.push_back(ifs.tellg());

		int32_t strLen = 0;
		ifs.read((char*)&strLen, sizeof(int32_t));
		StaticMeshInfo.AssetPath.resize(strLen);
		ifs.read((char*)StaticMeshInfo.AssetPath.data(), strLen * sizeof(char));
		pos.push_back(ifs.tellg());

		int32_t verticesLen = 0;
		ifs.read((char*)&verticesLen, sizeof(int32_t));
		StaticMeshInfo.Vertices.resize(verticesLen);
		//pos.push_back(sizeof(FVector));
		ifs.read((char*)StaticMeshInfo.Vertices.data(), verticesLen * sizeof(FVector));
		pos.push_back(ifs.tellg());

		int32_t indicesLen = 0;
		ifs.read((char*)&indicesLen, sizeof(int32_t));
		StaticMeshInfo.indices.resize(indicesLen);
		ifs.read((char*)StaticMeshInfo.indices.data(), indicesLen * sizeof(uint32_t));
		pos.push_back(ifs.tellg());

		int32_t normalsLen = 0;
		ifs.read((char*)&normalsLen, sizeof(int32_t));
		StaticMeshInfo.normals.resize(normalsLen);
		ifs.read((char*)StaticMeshInfo.normals.data(), normalsLen * sizeof(FVector4));
		pos.push_back(ifs.tellg());


	}
	ifs.close();
}


FMeshData* StaticMesh::GetStaticMesh()
{
	return &StaticMeshInfo;
}