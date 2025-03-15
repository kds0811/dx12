#include "SceneComponent.h"

SceneComponent::SceneComponent(SceneObject* owner) : mTrans(Vector::Zero(), Rotator::Zero(), Vector::One())
{
    mMovementComponent.Init(this);
    pOwner = owner;
}

SceneComponent::SceneComponent(SceneObject* owner, Transform trans) : mTrans(trans)
{
    mMovementComponent.Init(this);
    pOwner = owner;
}

SceneComponent::SceneComponent(SceneObject* owner, Vector loc) : mTrans(loc)
{
    mMovementComponent.Init(this);
    pOwner = owner;
}

SceneComponent::SceneComponent(SceneObject* owner, Vector loc, Rotator rot) : mTrans(loc, rot)
{
    mMovementComponent.Init(this);
    pOwner = owner;
}


void SceneComponent::Update(float dt) {}

void SceneComponent::SetTargetLocation(Vector targetLocation) {}

void SceneComponent::SetTargetRotation(Rotator targetRotation) {}

void SceneComponent::SetContinuesRotation(Rotator rotation) {}
