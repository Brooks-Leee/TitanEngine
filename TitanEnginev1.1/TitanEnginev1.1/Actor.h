#pragma once
#include "StaticMesh.h"
#include "TTexTure.h"
#include "TMaterial.h"


class Actor
{
public:
	FTransform Transform;
	std::string AssetPath;
	UINT HeapIndex;

	StaticMesh* MeshBuffer;
	TTexTure* TextureBuffer;
	TMaterial* MaterialBuffer;
};

