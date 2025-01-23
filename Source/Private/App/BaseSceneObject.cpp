#include "BaseSceneObject.h"

BaseSceneObject::BaseSceneObject() : mMovementComponent(mSceneComponent)
{

}

void BaseSceneObject::Update(float dt)
{
    mMovementComponent.Update(dt);
}