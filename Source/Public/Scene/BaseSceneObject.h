#pragma once
#include "SceneComponent.h"
#include "RenderItem.h"
#include "MovementComponent.h"
#include <memory>

class alignas (16) BaseSceneObject
{

public:
    BaseSceneObject() : mMovementComponent(mSceneComponent)
    {  }
    virtual ~BaseSceneObject() {}

    void Update(float dt);
    RenderItem* GetRenderItem() { return mRenderItem.get(); }

protected:
    SceneComponent mSceneComponent;
    MovementComponent mMovementComponent;
    std::unique_ptr<RenderItem> mRenderItem;
};