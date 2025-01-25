#include "BaseSceneObject.h"


void BaseSceneObject::Update(float dt)
{
    mMovementComponent.Update(dt);
}