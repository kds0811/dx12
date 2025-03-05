#pragma once
#include <DirectXMath.h>
#include "Transform.h"
#include "CameraController.h"
#include <memory>

class App;
class GameTimerW;
class MainInputController;

class alignas(16) Camera
{
    Transform Trans;
    GameTimerW* pTimer;
    MainInputController* pMainInputController;
    std::unique_ptr<CameraController> mCameraController = nullptr;

    float mSpeedCamera = 100.0f;
    float mSpeedRotateCamera = 100.0f;

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

    [[nodiscard]] inline DirectX::XMMATRIX GetViewMatrix() const noexcept
    {
        Vector Position = Trans.GetLocation();
        Vector ForwardDir = Trans.GetForwardVector();
        Vector UpDir = Trans.GetUpVector();

        DirectX::XMVECTOR Target = DirectX::XMVectorAdd(Position.ToSIMD(), ForwardDir.ToSIMD());
        
        return DirectX::XMMatrixLookAtLH(Position.ToSIMD(), Target, UpDir.ToSIMD());
    }

    float& GetSpeedCameraRef() { return mSpeedCamera; }
    float& GetSpeedRotateCameraRef() { return mSpeedRotateCamera; }
};