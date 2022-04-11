#pragma once
#include "TResource.h"
class TTexTure :
    public TResource
{
public:
	TTexTure(){}
	virtual ~TTexTure(){}

	std::string Name;
	std::wstring Filename;
};

