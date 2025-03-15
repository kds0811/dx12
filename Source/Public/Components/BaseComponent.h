#pragma once
#include "SceneObject.h"

class BaseComponent
{
public:
    BaseComponent() = default;
    virtual ~BaseComponent() = default;

    inline SceneObject* GetOwner() { return pOwner; }
    inline void SetOwner (SceneObject* owner) { pOwner = owner; }

protected:
    SceneObject* pOwner = nullptr;
};