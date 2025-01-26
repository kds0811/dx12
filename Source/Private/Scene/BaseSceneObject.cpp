#include "BaseSceneObject.h"


void BaseSceneObject::Update(float dt)
{
    mMovementComponent.Update(dt);
}

void BaseSceneObject::SetContiniusRotation(Rotator rot) 
{
    mMovementComponent.SetContinuesRotation(rot);
}
