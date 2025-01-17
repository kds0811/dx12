#include "Camera.h"

void Camera::MoveRight(float direction, float dt) 
{
    Vector VecOffset = Trans.GetRightVector() * direction * dt * SpeedCamera;
    Trans.AddLocation(VecOffset);
}

void Camera::MoveForward(float direction, float dt) 
{
    Vector VecOffset = Trans.GetForwardVector() * direction * dt * SpeedCamera;
    Trans.AddLocation(VecOffset);
}

void Camera::MoveAbsoluteUp(float direction, float dt) 
{
    Vector VecOffset = Vector(0.f, 1.0f, 0.0f) * direction * dt * SpeedCamera;
    Trans.AddLocation(VecOffset);
}

void Camera::RotateCamera(float xOffset, float yOffset, float dt)
{
    float NewPitch = yOffset * dt * SpeedRotateCamera;
    float NewYaw = xOffset * dt * SpeedRotateCamera;
    Rotator RotOffset(NewPitch, NewYaw, 0.0f);
    Trans.AddRotation(RotOffset);
}
