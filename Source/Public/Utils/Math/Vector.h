#pragma once
#include <DirectXMath.h>

class alignas(16) Vector
{
public:
    DirectX::XMFLOAT3A Data;

public:
    constexpr Vector() noexcept : Data(0.0f, 0.0f, 0.0f) {}
    constexpr Vector(float x, float y, float z) noexcept : Data(x, y, z) {}
    explicit Vector(const DirectX::XMFLOAT3& vec) noexcept : Data(vec.x, vec.y, vec.z) {}
    explicit Vector(DirectX::FXMVECTOR vec) noexcept { DirectX::XMStoreFloat3(&Data, vec); }

    [[nodiscard]] DirectX::XMVECTOR ToSIMD() const noexcept { DirectX::XMLoadFloat3A(&Data); }
    [[nodiscard]] inline float X() const noexcept { return Data.x; }
    [[nodiscard]] inline float Y() const noexcept { return Data.y; }
    [[nodiscard]] inline float Z() const noexcept { return Data.z; }

    void SetX(float x) noexcept { Data.x = x; }
    void SetY(float y) noexcept { Data.y = y; }
    void SetZ(float z) noexcept { Data.z = z; }

    Vector& operator+=(const Vector& other) noexcept;
    Vector operator+(const Vector& other) const noexcept;
    Vector& operator-=(const Vector& other) noexcept;
    Vector operator-(const Vector& other) const noexcept;

    Vector& AddAs(const Vector& other) noexcept;
    Vector Add(const Vector& other) const noexcept;
    Vector& SubAs(const Vector& other) noexcept;
    Vector Sub(const Vector& other) const noexcept;


    [[nodiscard]] float Length() const noexcept;

    [[nodiscard]] Vector Normalize() const noexcept;

    [[nodiscard]] float Dot(const Vector& other) const noexcept;

    static Vector Zero() noexcept { return Vector(0.0f, 0.0f, 0.0f); }
    static Vector One() noexcept { return Vector(1.0f, 1.0f, 1.0f); }
    static Vector Up() noexcept { return Vector(0.0f, 1.0f, 0.0f); }
};