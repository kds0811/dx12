#pragma once
#include <DirectXMath.h>

class Camera
{
public:
    Camera();

    inline DirectX::XMFLOAT3 GetLocation() const noexcept { return Location; }
    inline DirectX::XMFLOAT3 GetRotation() const noexcept { return Rotation; }

    inline DirectX::XMFLOAT4 GetForwardVector() const noexcept { return ForwardVector; }
    inline DirectX::XMFLOAT4 GetRightVector() const noexcept { return RightVector; }
    inline DirectX::XMFLOAT4 GetUpVector() const noexcept { return UpVector; }

private:
    static constexpr DirectX::XMFLOAT3 StartLocation = {0.0f, -10.f, 0.0f};
    static constexpr DirectX::XMFLOAT3 StartRotation = {0.0f, 0.0f, 0.0f};

    DirectX::XMFLOAT3 Location;
    DirectX::XMFLOAT3 Rotation;

    DirectX::XMFLOAT4 ForwardVector;
    DirectX::XMFLOAT4 RightVector;
    DirectX::XMFLOAT4 UpVector;
};