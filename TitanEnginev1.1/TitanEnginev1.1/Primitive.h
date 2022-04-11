#pragma once
#include "StaticMesh.h"
#include "TTexTure.h"
#include "TMaterial.h"


class Primitive
{
public:
	Primitive();
	~Primitive();



public:
	FTransform Transform;
	std::string AssetPath;
	UINT PrimitiveMVPIndex;


	StaticMesh* MeshBuffer;
	TMaterial* Material;

};

