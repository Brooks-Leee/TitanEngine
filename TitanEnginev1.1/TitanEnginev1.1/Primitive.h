#pragma once
#include "StaticMesh.h"
#include "TTexTure.h"
#include "TMaterial.h"


class Primitive
{
public:
	FTransform Transform;
	std::string AssetPath;
	UINT HeapIndex;

	StaticMesh* MeshBuffer;
	TTexTure* TextureBuffer;
	TMaterial* MaterialBuffer;
};

