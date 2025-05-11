#pragma once


class SceneComponent;

class MovementComponent
{
    SceneComponent* pOwnerSceneComp = nullptr;
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
    MovementComponent() = default;
    void Init(SceneComponent* ownerSceneComp);
    
    void Update(float dt);

    void SetTargetLocation(Vector targetLocation);
    void SetTargetRotation(Rotator targetRotation);
    void SetContinuesRotation(Rotator rotation);

private:
    void MoveToLocation(float dt);
    void Rotation(float dt);
    void ContinuesRotation(float dt);
};