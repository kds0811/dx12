#pragma once
#include "SceneComponent.h"
#include "RenderItem.h"


class BaseSceneObject
{
    SceneComponent mSceneComponent;

    RenderItem mRenderItem;

public:
    void Update();
    RenderItem* GetRenderItem() { return &mRenderItem; }


};