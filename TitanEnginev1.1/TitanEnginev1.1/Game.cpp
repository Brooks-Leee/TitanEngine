#include "stdafx.h"
#include "Game.h"

void Game::Init()
{
	const char* FilePath = "Assets\\Map\\Map.titan";
	LoadAllActorInMap(FilePath);

}

void Game::LoadAllActorInMap(const std::string& FilePath)
{
	TitanEngine::Get()->GetResourceMgr()->LoadAllActorInMap(FilePath);
	TitanEngine::Get()->GetRenderer()->UpdateScene();
}

void Game::Run()
{

}

void Game::Destroy()
{
}
