#pragma once
#include "TResource.h"
class TRenderTarget :
    public TResource
{
public:
    TRenderTarget(){};
    virtual ~TRenderTarget(){};


public:
    int mWidth;
    int mHeight;
};

