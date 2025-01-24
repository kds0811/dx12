#include "BaseSceneObject.h"

BaseSceneObject::BaseSceneObject() : mMovementComponent(mSceneComponent)
{
    CreateRenderItem();
}

void BaseSceneObject::Update(float dt)
{
    mMovementComponent.Update(dt);
}