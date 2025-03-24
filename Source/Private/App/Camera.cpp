#include "Camera.h"
#include "App.h"

Camera::Camera(MainInputController* inputController, GameTimerW* timer) : Trans(Vector{0.f, 50.f, -50.f}, Rotator(45.0f, 0.0f, 0.0f), Vector(1.f, 1.f, 1.f))
{
    pMainInputController = inputController;
    pTimer = timer;
    mCameraController = std::make_unique<CameraController>(this, pMainInputController);
}

void Camera::UpdateInput()
{
    mCameraController->UpdateInput();
    if (mViewDirty)
    {
        UpdateViewMatrix();
        mViewDirty = false;
    }
}

void Camera::MoveRight(float direction)
{
    Vector VecOffset = Trans.GetRightVector() * direction * pTimer->GetDeltaTime() * mSpeedCamera;
    Trans.AddLocation(VecOffset);
    mViewDirty = true;
}

void Camera::MoveForward(float direction)
{
    Vector VecOffset = Trans.GetForwardVector() * direction * pTimer->GetDeltaTime() * mSpeedCamera;
    Trans.AddLocation(VecOffset);
    mViewDirty = true;
}

void Camera::MoveAbsoluteUp(float direction)
{
    Vector VecOffset = Vector(0.f, 1.0f, 0.0f) * direction * pTimer->GetDeltaTime() * mSpeedCamera;
    Trans.AddLocation(VecOffset);
    mViewDirty = true;
}

void Camera::RotateCamera(float xOffset, float yOffset)
{
    float NewPitch = yOffset * pTimer->GetDeltaTime() * mSpeedRotateCamera;
    float NewYaw = xOffset * pTimer->GetDeltaTime() * mSpeedRotateCamera;
    Rotator RotOffset(NewPitch, NewYaw, 0.0f);
    Trans.AddRotation(RotOffset);
    mViewDirty = true;
}

void Camera::UpdateViewMatrix()
{
    Vector Position = Trans.GetLocation();
    Vector ForwardDir = Trans.GetForwardVector();
    Vector UpDir = Trans.GetUpVector();

    DirectX::XMVECTOR Target = DirectX::XMVectorAdd(Position.ToSIMD(), ForwardDir.ToSIMD());

    DirectX::XMStoreFloat4x4(&mView, DirectX::XMMatrixLookAtLH(Position.ToSIMD(), Target, UpDir.ToSIMD()));
}


