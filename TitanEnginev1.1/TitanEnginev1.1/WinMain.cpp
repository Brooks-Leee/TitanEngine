#include "stdafx.h"
#include "AppDraw.h"
#include "Scene.h"
#include "TitanEngine.h"
#include "Game.h"

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{

	Game game = Game();
	TitanEngine* engine = TitanEngine::Get();

	engine->Init();
	game.Init();

	if (engine->Run())
	{
		game.Run();
	}

	engine->Destroy();
	game.Destroy();
}


