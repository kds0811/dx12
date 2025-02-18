#include "Camera.h"
#include "App.h"

void Camera::MoveRight(float direction, float dt) 
{
    Vector VecOffset = Trans.GetRightVector() * direction * dt * mSpeedCamera;
    Trans.AddLocation(VecOffset);
}

void Camera::MoveForward(float direction, float dt) 
{
    Vector VecOffset = Trans.GetForwardVector() * direction * dt * mSpeedCamera;
    Trans.AddLocation(VecOffset);
}

void Camera::MoveAbsoluteUp(float direction, float dt) 
{
    Vector VecOffset = Vector(0.f, 1.0f, 0.0f) * direction * dt * mSpeedCamera;
    Trans.AddLocation(VecOffset);
}

void Camera::RotateCamera(float xOffset, float yOffset, float dt)
{
    float NewPitch = yOffset * dt * mSpeedRotateCamera;
    float NewYaw = xOffset * dt * mSpeedRotateCamera;
    Rotator RotOffset(NewPitch, NewYaw, 0.0f);
    Trans.AddRotation(RotOffset);
}

void Camera::SetWireframeMode(bool wireframeIsEnabled) 
{
    pOwner->SetWireframe(wireframeIsEnabled);
}
