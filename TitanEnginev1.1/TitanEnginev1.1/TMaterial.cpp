#include "stdafx.h"
#include "TMaterial.h"


TMaterial::TMaterial()
{
}

TMaterial::~TMaterial()
{
}

void TMaterial::addTexture(TTexTure* texture)
{
	textures.push_back(texture);
	this->normalMapIndex = this->diffuseSrvHeapIndex + 1;
}
