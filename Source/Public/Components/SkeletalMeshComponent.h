#pragma once
#include "MeshComponent.h"
#include "SkeletalMesh.h"

class SkeletalMeshComponent : public MeshComponent
{
    SkeletalMesh* pSkeletalMesh = nullptr;

public:
    SkeletalMeshComponent() = default;

    inline void SetSkeletalMesh(SkeletalMesh* skMesh) noexcept { pSkeletalMesh = skMesh; }

    virtual void SetMesh(Mesh* mesh) override;
};
