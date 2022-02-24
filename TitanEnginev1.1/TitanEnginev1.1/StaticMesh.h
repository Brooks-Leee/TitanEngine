#pragma once
struct FVector
{
	float x;
	float y;
	float z;
};

struct FStaticMeshInfo
{
	int32_t VerticesNum;
	int32_t TriangleNum;
	int32_t IndicesNum;
	int32_t	LODNum;
	std::string AssetPath;
	std::vector<FVector> Vertices;
	std::vector<uint32_t> indices;
};


class StaticMesh
{
public:
	void LoadBinaryFile(const std::string& FilePath);
	FStaticMeshInfo* GetStaticMesh();


private:
	FStaticMeshInfo StaticMeshInfo;
};