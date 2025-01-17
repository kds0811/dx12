#pragma once
#include <DirectXMath.h>
#include "Transform.h"

class alignas(16) Camera
{
    Transform Trans;

public:
    inline Camera() noexcept : Trans(Vector{0.f, 0.f, -10.f}, Rotator::Zero(), Vector(1.f, 1.f, 1.f)) {}

    void MoveRight(float direction, float dt);
    void MoveForward(float direction, float dt);
    void MoveAbsoluteUp(float direction, float dt);
    void RotateCamera(float xOffset, float yOffset, float dt);

    [[nodiscard]] inline DirectX::XMMATRIX GetViewMatrix() const noexcept
    {
        Vector Position = Trans.GetLocation();
        Vector ForwardDir = Trans.GetForwardVector();
        Vector UpDir = Trans.GetUpVector();

        DirectX::XMVECTOR Target = DirectX::XMVectorAdd(Position.ToSIMD(), ForwardDir.ToSIMD());
        
        return DirectX::XMMatrixLookAtLH(Position.ToSIMD(), Target, UpDir.ToSIMD());
    }

private:
    static constexpr float SpeedCamera = 50.0f;
    static constexpr float SpeedRotateCamera = 250.0f;
};