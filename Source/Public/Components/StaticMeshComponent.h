#pragma once
#include "MeshComponent.h"
#include "StaticMesh.h"
#include "RenderData.h"

class StaticMeshComponent : public MeshComponent
{
    StaticMesh* pStaticMesh = nullptr;

public:
    StaticMeshComponent(SceneObject* owner);
    inline void SetStaticMesh(StaticMesh* staticMesh) noexcept { pStaticMesh = staticMesh; }
    inline const StaticMesh* GetStaticMesh() const noexcept { return pStaticMesh; }
    inline  StaticMesh* GetStaticMesh() noexcept { return pStaticMesh; }
};