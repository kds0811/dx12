#pragma once
#include <DirectXMath.h>
#include "Transform.h"

class App;

class alignas(16) Camera
{
    Transform Trans;
    App* pOwner = nullptr;
    float mSpeedCamera = 100.0f;
    float mSpeedRotateCamera = 100.0f;

public:
    inline Camera() noexcept : Trans(Vector{0.f, 50.f, -50.f}, Rotator(45.0f, 0.0f, 0.0f), Vector(1.f, 1.f, 1.f))
    {}

    [[nodiscard]] inline DirectX::XMFLOAT3 GetCameraPos() const noexcept
    {
        const Vector pos = Trans.GetLocation();
        return DirectX::XMFLOAT3(pos.GetX(), pos.GetY(), pos.GetZ());
    }
    void MoveRight(float direction, float dt);
    void MoveForward(float direction, float dt);
    void MoveAbsoluteUp(float direction, float dt);
    void RotateCamera(float xOffset, float yOffset, float dt);
    void SetWireframeMode(bool wireframeIsEnabled);

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