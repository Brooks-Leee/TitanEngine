#include "stdafx.h"
#include "Game.h"

void Game::Init()
{
	
	LoadAssets();

}

void Game::LoadAssets()
{
	
	TitanEngine::Get()->GetResourceMgr()->LoadAllActorInMap();
	TitanEngine::Get()->GetResourceMgr()->LoadTextures();

}

void Game::Run()
{

}

void Game::Destroy()
{
}
