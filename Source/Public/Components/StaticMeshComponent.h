#pragma once
#include "MeshComponent.h"
#include "StaticMesh.h"

class StaticMeshComponent : public MeshComponent
{
    StaticMesh* mStaticMesh = nullptr;

public:
    StaticMeshComponent(SceneObject* owner);
};