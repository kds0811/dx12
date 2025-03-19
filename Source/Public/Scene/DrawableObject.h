#pragma once
#include "SceneObject.h"
#include "SceneComponent.h"
#include "memory"
#include "StaticMeshComponent.h"

class DrawableObject : public SceneObject
{
    std::unique_ptr<SceneComponent> mSceneComponent = nullptr;
    std::unique_ptr<MeshComponent> mMeshComponent = nullptr;

public:
    DrawableObject();
};