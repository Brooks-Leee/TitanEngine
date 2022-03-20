#pragma once
#include "FRHI.h"



class Renderer
{
public:
	Renderer();
	~Renderer();


public:
	void Init();
	void Run();
	void UpdateScene();

	

protected:
	FRHI* RHI;

};

