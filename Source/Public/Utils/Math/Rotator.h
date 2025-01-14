#pragma once
#include <DirectXMath.h>
#include <utility>
#include <cmath>

struct Vector;
struct Quat;

struct alignas(16) Rotator final
{
private:
    DirectX::XMFLOAT3A Data;

public:
    inline Rotator() noexcept : Data(0.0f, 0.0f, 0.0f) {}
    inline Rotator(float pitch, float yaw, float roll) noexcept : Data(pitch, yaw, roll) {}
    inline explicit Rotator(const DirectX::XMFLOAT3A rot) noexcept : Data(rot.x, rot.y, rot.z) {}
    inline explicit Rotator(const DirectX::XMFLOAT3 rot) noexcept : Data(rot.x, rot.y, rot.z) {}
    inline explicit Rotator(DirectX::FXMVECTOR rot) noexcept { DirectX::XMStoreFloat3A(&Data, rot); }
    explicit Rotator(Quat quat);

    // standart members
    Rotator& operator=(const Rotator& rhs) noexcept
    {
        if (this != &rhs)
        {
            Data.x = rhs.Data.x;
            Data.y = rhs.Data.y;
            Data.z = rhs.Data.z;
        }
        return *this;
    }
    Rotator& operator=(Rotator&& rhs) noexcept
    {
        if (this != &rhs)
        {
            Data.x = std::exchange(rhs.Data.x, 0.0f);
            Data.y = std::exchange(rhs.Data.y, 0.0f);
            Data.z = std::exchange(rhs.Data.z, 0.0f);
        }
        return *this;
    }
    inline Rotator(const Rotator& rhs) noexcept : Data(rhs.Data.x, rhs.Data.y, rhs.Data.z) {}
    inline Rotator(Rotator&& rhs) noexcept
        : Data(std::exchange(rhs.Data.x, 0.0f), std::exchange(rhs.Data.y, 0.0f), std::exchange(rhs.Data.z, 0.0f)) {}

    // Seters
    inline void SetPitch(const float& Pitch) noexcept { Data.x = Pitch; }
    inline void SetYaw(const float& Yaw) noexcept { Data.y = Yaw; }
    inline void SetRoll(const float& Roll) noexcept { Data.z = Roll; }

    // Geters
    inline float GetPitch() const noexcept { return Data.x; }
    inline float GetYaw() const noexcept { return Data.y; }
    inline float GetRoll() const noexcept { return Data.z; }
    inline DirectX::XMFLOAT3A GetData() const noexcept { return Data; }

    // operators
    Rotator& operator+=(const Rotator& other) noexcept;
    Rotator operator+(const Rotator& other) const noexcept;
    Rotator& operator-=(const Rotator& other) noexcept;
    Rotator operator-(const Rotator& other) const noexcept;
    Rotator& operator*=(const float& sc) noexcept;
    Rotator operator*(const float& sc) const noexcept;
    Rotator& operator/=(const float& dv) noexcept;
    Rotator operator/(const float& dv) const noexcept;
    bool operator==(const Rotator& other) const noexcept;
    bool operator!=(const Rotator& other) const noexcept;
    bool IsNearEqual(const Rotator& other, float epsilon = 0.01) const noexcept;

    // Base methods
    [[nodiscard]] inline DirectX::XMVECTOR ToSIMD() const noexcept { return DirectX::XMLoadFloat3A(&Data); }
    [[nodiscard]] Rotator Normalize180() const noexcept;
    [[nodiscard]] Rotator Normalize360() const noexcept;
    [[nodiscard]] Rotator Clamp(const Rotator& min, const Rotator& max) const noexcept;

    [[nodiscard]] DirectX::XMVECTOR ToQuatSIMD() const noexcept;
    [[nodiscard]] Quat ToQuat() const noexcept;
    [[nodiscard]] DirectX::XMMATRIX ToMatrix() const noexcept;

    //Direction Vectors
    [[nodiscard]] Vector GetForwardVector() const noexcept;
    [[nodiscard]] Vector GetRightVector() const noexcept;
    [[nodiscard]] Vector GetUpVector() const noexcept;
    

    // Statics
    [[nodiscard]] static Rotator FromQuat(DirectX::FXMVECTOR quat) noexcept;
    [[nodiscard]] static Rotator FromQuat(const Quat& quat) noexcept;
    [[nodiscard]] static Rotator Zero() noexcept { return Rotator(0.0f, 0.0f, 0.0f); }
    [[nodiscard]] static Rotator Lerp(const Rotator& start, const Rotator& end, float alpha) noexcept;
    [[nodiscard]] static float NormalizeAxis(float Angle);
    [[nodiscard]] static float ClampAxis(float Angle);
};