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
    float mNearZ = 1.0f;
    float mFarZ = 100000.0f;
    float mAspect = 0.0f;
    float mFovY = 0.0f;
    float mNearWindowHeight = 0.0f;
    float mFarWindowHeight = 0.0f;

    // Cache View/Proj matrices.
    DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

    bool mViewDirty = true;

    // Cahce scree sizes
    UINT mWidth = 0u;
    UINT mHeight = 0u;

    // FOV deegres
    float mFovYDegrees = 80.0f;

public:
    Camera(MainInputController* inputController, GameTimerW* timer, UINT width, UINT height);

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

    float GetSpeedCameraRef() { return mSpeedCamera; }
    float GetSpeedRotateCameraRef() { return mSpeedRotateCamera; }

    // Get frustum properties.
    [[nodiscard]] inline float GetNearZ() const noexcept { return mNearZ; }
    [[nodiscard]] inline float GetFarZ() const noexcept { return mFarZ; }
    [[nodiscard]] inline float GetAspect() const noexcept { return mAspect; }
    [[nodiscard]] inline float GetFovY() const noexcept { return mFovY; }
    [[nodiscard]] inline float GetFovX() const
    {
        float halfWidth = 0.5f * GetNearWindowWidth();
        return 2.0f * atan(halfWidth / mNearZ);
    }

    // Get near and far plane dimensions in view space coordinates.
    [[nodiscard]] inline float GetNearWindowWidth() const noexcept { return mAspect* mNearWindowHeight; }
    [[nodiscard]] inline float GetNearWindowHeight() const noexcept { return mNearWindowHeight; }
    [[nodiscard]] inline float GetFarWindowWidth() const noexcept { return mAspect * mFarWindowHeight; }
    [[nodiscard]] inline float GetFarWindowHeight() const noexcept { return mFarWindowHeight; }

    // Set frustum.
    void SetLens(float fovY, float aspect, float zn, float zf);

    // Get View/Proj matrices.
    [[nodiscard]] inline DirectX::XMMATRIX GetView() const noexcept { return XMLoadFloat4x4(&mView); }
    [[nodiscard]] inline DirectX::XMMATRIX GetProj() const noexcept { return XMLoadFloat4x4(&mProj); }

    [[nodiscard]] inline DirectX::XMFLOAT4X4 GetView4x4f() const noexcept { return mView; }
    [[nodiscard]] inline DirectX::XMFLOAT4X4 GetProj4x4f() const noexcept { return mProj; }

    [[nodiscard]] inline float GetAspectRatio() const { return static_cast<float>(mWidth) / static_cast<float>(mHeight); }

private:
    void UpdateViewMatrix();
};
