#include "MovementComponent.h"
#include "SceneComponent.h"

void MovementComponent::Init(SceneComponent* ownerSceneComp) 
{
    pOwnerSceneComp = ownerSceneComp;
}

void MovementComponent::Update(float dt)
{
    if (bContinuesRotationIsSet)
    {
        ContinuesRotation(dt);
    }

    if (bTargetRotationIsSet)
    {
        Rotation(dt);
    }

    if (bTargetLocationIsSet)
    {
        MoveToLocation(dt);
    }
}

void MovementComponent::SetTargetLocation(Vector targetLocation)
{
    mTargetLocation = targetLocation;
    bTargetLocationIsSet = true;
}

void MovementComponent::SetTargetRotation(Rotator targetRotation)
{
    mTargetRotation = targetRotation.Normalize360();
    bTargetRotationIsSet = true;
    bContinuesRotationIsSet = false;
}

void MovementComponent::SetContinuesRotation(Rotator rotation)
{
    mContinuesRotation = rotation;
    bContinuesRotationIsSet = true;
    bTargetRotationIsSet = false;
}

void MovementComponent::MoveToLocation(float dt)
{

    Vector vecToTarget = mTargetLocation - pOwnerSceneComp->GetLocation();
    if (vecToTarget.Length() < 1.0f)
    {
        bTargetLocationIsSet = false;
        mTargetLocation = Vector::Zero();
        return;
    }
    Vector direction = vecToTarget.Normalize();
    Vector VecOfsset = direction * dt * mSpeedMoving;
    pOwnerSceneComp->AddLocation(VecOfsset);
}

void MovementComponent::Rotation(float dt) 
{
    Rotator UpdatedRotatorNorm = (pOwnerSceneComp->GetRotation()).Normalize360();

    if (UpdatedRotatorNorm.IsNearEqual(mTargetRotation, 1.0f))
    {
        bTargetRotationIsSet = false;
        mTargetRotation = Rotator::Zero();
    }
    Rotator offsetRot = Rotator::Zero();

    float PitchRem = std::abs(mTargetRotation.GetPitch() - UpdatedRotatorNorm.GetPitch());
    if (PitchRem > 1.0f)
    {
        if ((mTargetRotation.GetPitch() - UpdatedRotatorNorm.GetPitch()) > 0.0f)
        {
            offsetRot += Rotator(mSpeedRotating * dt, 0.0f, 0.0f);
        }
        else
        {
            offsetRot -= Rotator(mSpeedRotating * dt, 0.0f, 0.0f);
        }
        
    }
    float YawRem = std::abs(mTargetRotation.GetYaw() - UpdatedRotatorNorm.GetYaw());
    if (YawRem > 1.0f)
    {
        if ((mTargetRotation.GetYaw() - UpdatedRotatorNorm.GetYaw()) > 0.0f)
        {
            offsetRot += Rotator(0.0f, mSpeedRotating * dt, 0.0f);
        }
        else
        {
            offsetRot -= Rotator(0.0f, mSpeedRotating * dt, 0.0f);
        }
        
    }
    float RollRem = std::abs(mTargetRotation.GetRoll() - UpdatedRotatorNorm.GetRoll());
    if (RollRem > 1.0f)
    {
        if ((mTargetRotation.GetRoll() - UpdatedRotatorNorm.GetRoll()) > 0.0f)
        {
            offsetRot += Rotator(0.0f, 0.0f, mSpeedRotating * dt);
        }
        else
        {
            offsetRot -= Rotator(0.0f, 0.0f, mSpeedRotating * dt);
        }
    }
    pOwnerSceneComp->AddRotation(offsetRot);
}

void MovementComponent::ContinuesRotation(float dt)
{
    Rotator OffsetRotation = mContinuesRotation * dt * mSpeedRotating;
    pOwnerSceneComp->AddRotation(OffsetRotation);
}
