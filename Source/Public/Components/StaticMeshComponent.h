#pragma once
#include "MeshComponent.h"
#include "StaticMesh.h"
#include "RenderData.h"

class StaticMeshComponent : public MeshComponent
{
    StaticMesh* mStaticMesh = nullptr;

public:
    StaticMeshComponent(SceneObject* owner);



    void SetRenderData(RenderData* rd);
};