#pragma once
#include "SceneComponent.h"
#include "RenderItem.h"
#include "MovementComponent.h"

class alignas (16) BaseSceneObject
{

public:
    BaseSceneObject() : mMovementComponent(mSceneComponent){}
    virtual ~BaseSceneObject() {}

    void Update(float dt);
    RenderItem* GetRenderItem() { return &mRenderItem; }

protected:
    SceneComponent mSceneComponent;
    MovementComponent mMovementComponent;
    RenderItem mRenderItem;

private:
    virtual void CreateRenderItem() = 0;
};