#pragma once
#include <DirectXMath.h>

class alignas(16) Vector
{
public:
    DirectX::XMFLOAT3A Data;

public:
    Vector() noexcept : Data(0.0f, 0.0f, 0.0f) {}
    Vector(float x, float y, float z) noexcept : Data(x, y, z) {}
    explicit Vector(const DirectX::XMFLOAT3& vec) noexcept : Data(vec.x, vec.y, vec.z) {}
    explicit Vector(DirectX::FXMVECTOR vec) noexcept { DirectX::XMStoreFloat3(&Data, vec); }

    [[nodiscard]] inline DirectX::XMVECTOR ToSIMD() const noexcept { return DirectX::XMLoadFloat3A(&Data); }

    void SetX(float x) noexcept { Data.x = x; }
    void SetY(float y) noexcept { Data.y = y; }
    void SetZ(float z) noexcept { Data.z = z; }

    Vector& operator+= (const Vector& other) noexcept;
    Vector operator+ (const Vector& other) const noexcept;
    Vector& operator-= (const Vector& other) noexcept;
    Vector operator- (const Vector& other) const noexcept;
    Vector& operator*= (const float& sc) noexcept;
    Vector operator*(const float& sc) const noexcept;
    Vector& operator/=(const float& dv) noexcept;
    Vector operator/(const float& dv) const noexcept;
    bool operator==(const Vector& other) const noexcept;
    bool operator!=(const Vector& other) const noexcept;
    bool NearEqual(const Vector& other, float epsilon = 0.01) const noexcept;

    [[nodiscard]] Vector Abs() const noexcept;
    [[nodiscard]] float Length() const noexcept;
    [[nodiscard]] float LengthSq() const noexcept;
    [[nodiscard]] float LengthEst() const noexcept;
    [[nodiscard]] Vector Normalize() const noexcept;
    [[nodiscard]] float Dot(const Vector& other) const noexcept;
    [[nodiscard]] Vector Cross(const Vector& other) const noexcept;
    [[nodiscard]] float Angle(const Vector& other) const noexcept;
    [[nodiscard]] Vector Clamp(const Vector& min, const Vector& max) const noexcept;
    [[nodiscard]] static float Distance(const Vector& a, const Vector& b) noexcept;

    static Vector Zero() noexcept { return Vector(0.0f, 0.0f, 0.0f); }
    static Vector One() noexcept { return Vector(1.0f, 1.0f, 1.0f); }
    static Vector Up() noexcept { return Vector(0.0f, 1.0f, 0.0f); }
    static Vector Lerp(const Vector& start, const Vector& end, float t) noexcept;

};