#pragma once
#include "MyMath.h"
#include "TResource.h"

struct Vertex
{
	glm::vec3 Pos;
	glm::vec4 Color;
	glm::vec4 Normal;
	glm::vec4 TangentX;
	glm::vec2 Texcoord;
};

struct VertexPos
{
	glm::vec3 Pos;
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
	std::vector<FVector4> tangentx;
	std::vector<FVector2> texcoords;
};


class StaticMesh : public TResource
{
public:
	StaticMesh(){}
	virtual ~StaticMesh(){}

public:
	FMeshData* GetStaticMesh();

public:
	FMeshData StaticMeshInfo;
};