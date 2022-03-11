#include "stdafx.h"
#include "Scene.h"
#include "TitanEngine.h"

void Scene::SpawnSphere()
{
	auto it = TitanEngine::Get()->GetResourceMgr()->getAllMeshData().find("1M_Cube.titan");
//	SceneDataArr.push_back()


	TitanEngine::Get()->GetRenderer()->UpdateScene();
}
