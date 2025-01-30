#pragma once
#include "SceneComponent.h"


class MovementComponent
{
    SceneComponent& mUpdatedSceneComponent;
    float mSpeedMoving = 20.0f;
    float mSpeedRotating = 20.0f;

    Vector mTargetLocation = Vector::Zero();
    Rotator mTargetRotation = Rotator::Zero();
    Rotator mContinuesRotation = Rotator::Zero();

    bool bTargetLocationIsSet = false;
    bool bTargetRotationIsSet = false;
    bool bContinuesRotationIsSet = false;

    Vector Velocity = Vector::Zero();

public:
    MovementComponent(SceneComponent& updatedSceneComponent) : mUpdatedSceneComponent(updatedSceneComponent) {}

    void Update(float dt);

    void SetTargetLocation(Vector targetLocation);
    void SetTargetRotation(Rotator targetRotation);
    void SetContinuesRotation(Rotator rotation);

private:
    void MoveToLocation(float dt);
    void Rotation(float dt);
    void ContinuesRotation(float dt);
};