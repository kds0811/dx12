#include "Scene.h"

void Scene::InitScene() 
{

}

void Scene::Update()
{
    for (const auto& object : mSceneObjects)
    {
        object->Update(mTimer.GetDeltaTime());
    }
}
