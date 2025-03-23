#pragma once
#include "SceneObject.h"
#include "SceneComponent.h"
#include "memory"

class MovableObject : public SceneObject
{
    std::unique_ptr<SceneComponent> mSceneComponent;


public:
    MovableObject(int id, std::string name);
};