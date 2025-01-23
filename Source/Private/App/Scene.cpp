#include "Scene.h"

void Scene::AddObject(std::unique_ptr<BaseSceneObject>&& object) 
{
    mSceneObjects.push_back(object);
}



void Scene::Update() {}
