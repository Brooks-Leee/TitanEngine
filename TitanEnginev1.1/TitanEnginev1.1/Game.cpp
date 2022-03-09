#include "stdafx.h"
#include "Game.h"

void Game::Init()
{
	const char* FilePath = "Assets\\Map\\Map.titan";
	LoadAllActorInMap(FilePath);

}

void Game::LoadAllActorInMap(std::string FilePath)
{
	TitanEngine::Get()->GetResourceMgr()->LoadAllActorInMap(FilePath);
}

void Game::Run()
{

}
