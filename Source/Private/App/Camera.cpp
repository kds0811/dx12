#include "Camera.h"
#include "App.h"

using namespace DirectX;

Camera::Camera(MainInputController* inputController, GameTimerW* timer,  UINT width, UINT height)
    :
    Trans(Vector{0.f, 50.f, -50.f}, Rotator(45.0f, 0.0f, 0.0f), Vector(1.f, 1.f, 1.f)),
    mWidth(width),
    mHeight(height)

{
    pMainInputController = inputController;
    pTimer = timer;
    mCameraController = std::make_unique<CameraController>(this, pMainInputController);
    SetLens(XMConvertToRadians(mFovYDegrees), GetAspectRatio(), mNearZ, mFarZ);


    
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

void Camera::SetLens(float fovY, float aspect, float zn, float zf)
{
    // cache properties
    mFovY = fovY;
    mAspect = aspect;
    mNearZ = zn;
    mFarZ = zf;

    mNearWindowHeight = 2.0f * mNearZ * tanf(0.5f * mFovY);
    mFarWindowHeight = 2.0f * mFarZ * tanf(0.5f * mFovY);

    DirectX::XMMATRIX P = DirectX::XMMatrixPerspectiveFovLH(mFovY, mAspect, mNearZ, mFarZ);
    DirectX::XMStoreFloat4x4(&mProj, P);
}



void Camera::UpdateViewMatrix()
{
    Vector Position = Trans.GetLocation();
    Vector ForwardDir = Trans.GetForwardVector();
    Vector UpDir = Trans.GetUpVector();

    DirectX::XMVECTOR Target = DirectX::XMVectorAdd(Position.ToSIMD(), ForwardDir.ToSIMD());

    DirectX::XMStoreFloat4x4(&mView, DirectX::XMMatrixLookAtLH(Position.ToSIMD(), Target, UpDir.ToSIMD()));
}


