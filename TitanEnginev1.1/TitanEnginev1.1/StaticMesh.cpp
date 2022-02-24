#pragma once
#include "stdafx.h"
#include "StaticMesh.h"


void StaticMesh::LoadBinaryFile(const std::string& FilePath)
{
	std::ifstream ifs(FilePath, std::ios::binary);
	if (ifs.is_open())
	{
		ifs.read((char*)&StaticMeshInfo.VerticesNum, sizeof(int32_t));
		ifs.read((char*)&StaticMeshInfo.TriangleNum, sizeof(int32_t));
		ifs.read((char*)&StaticMeshInfo.IndicesNum, sizeof(int32_t));
		ifs.read((char*)&StaticMeshInfo.LODNum, sizeof(int32_t));

		int32_t strLen = 0;
		ifs.read((char*)&strLen, sizeof(int32_t));
		StaticMeshInfo.AssetPath.resize(strLen);
		ifs.read((char*)StaticMeshInfo.AssetPath.data(), strLen * sizeof(char));

		int32_t verticesLen = 0;
		ifs.read((char*)&verticesLen, sizeof(int32_t));
		StaticMeshInfo.Vertices.resize(verticesLen);
		ifs.read((char*)StaticMeshInfo.Vertices.data(), verticesLen * sizeof(FVector));

		int32_t indicesLen = 0;
		ifs.read((char*)&indicesLen, sizeof(int32_t));
		StaticMeshInfo.indices.resize(indicesLen);
		ifs.read((char*)StaticMeshInfo.indices.data(), indicesLen * sizeof(int32_t));
	}
	ifs.close();
}


FStaticMeshInfo* StaticMesh::GetStaticMesh()
{
	return &StaticMeshInfo;
}