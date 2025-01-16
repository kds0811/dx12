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

void Camera::RotateYaw(float direction, float dt) 
{
    Rotator RotOffset(0.0f, direction * dt * SpeedRotateCamera, 0.0f);
    Trans.AddRotation(RotOffset);
}
