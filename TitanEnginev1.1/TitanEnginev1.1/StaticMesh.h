#pragma once
#include "MyMath.h"
#include <DirectXMath.h>

using namespace DirectX;

struct Vertex
{
	XMFLOAT3 Pos;
	XMFLOAT4 Color;
	XMFLOAT4 Normal;
};

struct FMeshData
{
	int32_t VerticesNum;
	int32_t TriangleNum;
	int32_t IndicesNum;
	int32_t	LODNum;
	std::string AssetPath;
	std::vector<FVector> Vertices;
	std::vector<uint32_t> indices;
	std::vector<FVector4> normals;
};


class StaticMesh
{
public:
	void LoadBinaryFile(const std::string& FilePath);
	FMeshData* GetStaticMesh();


private:
	FMeshData StaticMeshInfo;
};