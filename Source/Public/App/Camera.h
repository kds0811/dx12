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
    void RotateYaw(float direction, float dt);

    [[nodiscard]] inline DirectX::XMMATRIX GetViewMatrix() const noexcept
    { 
        return DirectX::XMMatrixLookAtLH(Trans.GetLocation().ToSIMD(), Trans.GetForwardVector().ToSIMD(), Trans.GetUpVector().ToSIMD());
    }

private:
    static constexpr float SpeedCamera = 100.0f;
    static constexpr float SpeedRotateCamera = 100.0f;
};