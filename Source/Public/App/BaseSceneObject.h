#pragma once
#include "SceneComponent.h"
#include "RenderItem.h"
#include "MovementComponent.h"


class BaseSceneObject
{
    SceneComponent mSceneComponent;
    MovementComponent mMovementComponent;

    RenderItem mRenderItem;

public:
    BaseSceneObject();
    void Update(float dt);
    RenderItem* GetRenderItem() { return &mRenderItem; }


};