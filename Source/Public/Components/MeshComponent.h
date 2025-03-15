#pragma once
#include "Material.h"
#include "BoundComponent.h"
#include "BaseComponent.h"

class MeshComponent : public BaseComponent
{
protected:
    Material* pMaterial = nullptr;
    BoundComponent mBoundComponent;

public:
    MeshComponent(SceneObject* owner) { pOwner = owner; }
    virtual ~MeshComponent() = default;

    inline void SetMaterial(Material* mat) { pMaterial = mat; }
    inline Material* GetMaterial() const { return pMaterial; }
};