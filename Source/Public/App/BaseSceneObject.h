#pragma once
#include "SceneComponent.h"
#include "RenderItem.h"
#include "MovementComponent.h"

class BaseSceneObject
{

public:
    BaseSceneObject();
    void Update(float dt);
    RenderItem* GetRenderItem() { return &mRenderItem; }

protected:
    SceneComponent mSceneComponent;
    MovementComponent mMovementComponent;
    RenderItem mRenderItem;


};