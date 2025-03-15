#include "SceneComponent.h"

SceneComponent::SceneComponent() : mTrans(Vector::Zero(), Rotator::Zero(), Vector::One()) 
{
    mMovementComponent.Init(this);
}

SceneComponent::SceneComponent(Transform trans) : mTrans(trans)
{
    mMovementComponent.Init(this);
}

SceneComponent::SceneComponent(Vector loc) : mTrans(loc)
{
    mMovementComponent.Init(this);
}

SceneComponent::SceneComponent(Vector loc, Rotator rot) : mTrans(loc, rot)
{
    mMovementComponent.Init(this);
}


void SceneComponent::Update(float dt) {}

void SceneComponent::SetTargetLocation(Vector targetLocation) {}

void SceneComponent::SetTargetRotation(Rotator targetRotation) {}

void SceneComponent::SetContinuesRotation(Rotator rotation) {}
