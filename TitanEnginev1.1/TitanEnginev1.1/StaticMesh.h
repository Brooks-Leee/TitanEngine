#pragma once
#include "MyMath.h"


struct Vertex
{
	glm::vec3 Pos;
	glm::vec4 Color;
	glm::vec4 Normal;
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
	FMeshData* GetStaticMesh();

public:
	FMeshData StaticMeshInfo;
};