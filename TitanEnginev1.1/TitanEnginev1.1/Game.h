#pragma once
#include "TitanEngine.h"

class Game
{
public:
	void Init();

	void LoadAllActorInMap(const std::string& FilePath);

	void Run();

	void Destroy();
};

