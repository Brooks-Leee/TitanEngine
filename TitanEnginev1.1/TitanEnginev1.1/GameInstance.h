#pragma once
#include "TitanEngine.h"
#include "Game.h"



class GameInstance
{
public:
	void Init();
	void Run();
	void Destrou();

public:
	TitanEngine TitanEngineins;
	Game GameLogic;
};

