#pragma once
#include <DirectXMath.h>
#include <utility>



struct alignas(16) Vector final
{
public:
    DirectX::XMFLOAT3A Data;

public:
    // constructors
    inline Vector() noexcept : Data(0.0f, 0.0f, 0.0f) {}
    inline Vector(float x, float y, float z) noexcept : Data(x, y, z) {}
    inline explicit Vector(const DirectX::XMFLOAT3A vec) noexcept : Data(vec.x, vec.y, vec.z) {}
    inline explicit Vector(const DirectX::XMFLOAT3 vec) noexcept : Data(vec.x, vec.y, vec.z) {}
    inline explicit Vector(DirectX::FXMVECTOR vec) noexcept { DirectX::XMStoreFloat3A(&Data, vec); }

    // standart members
    Vector& operator=(const Vector& rhs) noexcept
    {
        if (this != &rhs)  
        {
            Data.x = rhs.Data.x;
            Data.y = rhs.Data.y;
            Data.z = rhs.Data.z;
        }
        return *this;  
    }
    Vector& operator=(Vector&& rhs) noexcept
    {
        if (this != &rhs)
        {
            Data.x = std::exchange(rhs.Data.x, 0.0f);
            Data.y = std::exchange(rhs.Data.y, 0.0f);
            Data.z = std::exchange(rhs.Data.z, 0.0f);
        }
        return *this; 
    }
    inline Vector(const Vector& rhs) noexcept : Data(rhs.Data.x, rhs.Data.y, rhs.Data.z) {}
    inline Vector(Vector&& rhs) noexcept
        : Data(std::exchange(rhs.Data.x, 0.0f), std::exchange(rhs.Data.y, 0.0f), std::exchange(rhs.Data.z, 0.0f))
    {}

    // Seters
    inline void SetX(const float& inX) noexcept { Data.x = inX; }
    inline void SetY(const float& inY) noexcept { Data.y = inY; }
    inline void SetZ(const float& inZ) noexcept { Data.z = inZ; }

    // Geters
    inline float GetX() const noexcept { return Data.x; }
    inline float GetY() const noexcept { return Data.y; }
    inline float GetZ() const noexcept { return Data.z; }
    inline DirectX::XMFLOAT3A GetData() const noexcept { return Data; }

    // operators
    Vector& operator+=(const Vector& other) noexcept;
    Vector operator+(const Vector& other) const noexcept;
    Vector& operator-=(const Vector& other) noexcept;
    Vector operator-(const Vector& other) const noexcept;
    Vector& operator*=(const float& sc) noexcept;
    Vector operator*(const float& sc) const noexcept;
    Vector& operator/=(const float& dv) noexcept;
    Vector operator/(const float& dv) const noexcept;
    bool operator==(const Vector& other) const noexcept;
    bool operator!=(const Vector& other) const noexcept;
    bool IsNearEqual(const Vector& other, float epsilon = 0.01) const noexcept;

    // Base methods
    [[nodiscard]] inline DirectX::XMVECTOR ToSIMD() const noexcept { return DirectX::XMLoadFloat3A(&Data); }
    [[nodiscard]] Vector Abs() const noexcept;
    [[nodiscard]] float Length() const noexcept;
    [[nodiscard]] float LengthSq() const noexcept;
    [[nodiscard]] float LengthEst() const noexcept;
    [[nodiscard]] Vector Normalize() const noexcept;
    [[nodiscard]] Vector NormalizeEst() const noexcept;
    [[nodiscard]] float Dot(const Vector& other) const noexcept;
    [[nodiscard]] Vector Cross(const Vector& other) const noexcept;
    [[nodiscard]] float Angle(const Vector& other) const noexcept;
    [[nodiscard]] Vector Clamp(const Vector& min, const Vector& max) const noexcept;
    [[nodiscard]] Vector Rotate(DirectX::FXMVECTOR quat) const noexcept;
    [[nodiscard]] Vector RotateInverse(DirectX::FXMVECTOR quat) const noexcept;

    // Static methods
    [[nodiscard]] static float Distance(const Vector& a, const Vector& b) noexcept;
    [[nodiscard]] static Vector Zero() noexcept { return Vector(0.0f, 0.0f, 0.0f); }
    [[nodiscard]] static Vector One() noexcept { return Vector(1.0f, 1.0f, 1.0f); }
    [[nodiscard]] static Vector Up() noexcept { return Vector(0.0f, 1.0f, 0.0f); }
    [[nodiscard]] static Vector Lerp(const Vector& start, const Vector& end, float t) noexcept;
};