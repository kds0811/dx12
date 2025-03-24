#pragma once
#include <DirectXMath.h>
#include "Transform.h"
#include "CameraController.h"
#include <memory>
#include <DirectXCollision.h>

class App;
class GameTimerW;
class MainInputController;

class Camera
{
    Transform Trans;
    GameTimerW* pTimer;
    MainInputController* pMainInputController;
    std::unique_ptr<CameraController> mCameraController = nullptr;
    DirectX::BoundingFrustum mCamFrustum;

    float mSpeedCamera = 100.0f;
    float mSpeedRotateCamera = 100.0f;

    // Cache frustum properties.
    float mNearZ = 0.0f;
    float mFarZ = 0.0f;
    float mAspect = 0.0f;
    float mFovY = 0.0f;
    float mNearWindowHeight = 0.0f;
    float mFarWindowHeight = 0.0f;

    // Cache View/Proj matrices.
    DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    bool mViewDirty = true;

public:
    Camera(MainInputController* inputController, GameTimerW* timer);
    void UpdateInput();

    [[nodiscard]] inline DirectX::XMFLOAT3 GetCameraPos() const noexcept
    {
        const Vector pos = Trans.GetLocation();
        return DirectX::XMFLOAT3(pos.GetX(), pos.GetY(), pos.GetZ());
    }
    void MoveRight(float direction);
    void MoveForward(float direction);
    void MoveAbsoluteUp(float direction);
    void RotateCamera(float xOffset, float yOffset);

    [[nodiscard]] inline DirectX::XMMATRIX GetViewMatrix() const noexcept { return DirectX::XMLoadFloat4x4(&mView); }

    float GetSpeedCameraRef() { return mSpeedCamera; }
    float GetSpeedRotateCameraRef() { return mSpeedRotateCamera; }

private:
    void UpdateViewMatrix();
};


