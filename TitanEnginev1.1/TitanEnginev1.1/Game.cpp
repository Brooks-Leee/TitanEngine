#include "stdafx.h"
#include "Game.h"

void Game::Init()
{
	
	LoadAssets();

}

void Game::LoadAssets()
{
	
	TitanEngine::Get()->GetResourceMgr()->LoadAssests();

}

void Game::Run()
{

}

void Game::Destroy()
{
}
