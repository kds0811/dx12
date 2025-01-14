#pragma once
#include <DirectXMath.h>
#include <utility>

struct Vector;
struct Rotator;

struct alignas(16) Quat
{
private:
    DirectX::XMFLOAT4A Data;

public:
    // constructors
    inline Quat() : Data(0.0f, 0.0f, 0.0f, 1.0f) {}
    inline explicit Quat(DirectX::FXMVECTOR quat) noexcept { DirectX::XMStoreFloat4A(&Data, quat); }
    inline explicit Quat(float pitch, float yaw, float roll) noexcept
    {
        DirectX::XMStoreFloat4A(&Data, DirectX::XMQuaternionRotationRollPitchYaw(pitch, yaw, roll));
    }
    inline explicit Quat(DirectX::XMMATRIX RotationMatrix) noexcept
    {
        DirectX::XMStoreFloat4A(&Data, DirectX::XMQuaternionRotationMatrix(RotationMatrix));
    }

    // standart members
    Quat& operator=(const Quat& rhs) noexcept
    {
        if (this != &rhs)
        {
            Data.x = rhs.Data.x;
            Data.y = rhs.Data.y;
            Data.z = rhs.Data.z;
            Data.w = rhs.Data.w;
        }
        return *this;
    }
    Quat& operator=(Quat&& rhs) noexcept
    {
        if (this != &rhs)
        {
            Data.x = std::exchange(rhs.Data.x, 0.0f);
            Data.y = std::exchange(rhs.Data.y, 0.0f);
            Data.z = std::exchange(rhs.Data.z, 0.0f);
            Data.w = std::exchange(rhs.Data.w, 0.0f);
        }
        return *this;
    }
    inline Quat(const Quat& rhs) noexcept : Data(rhs.Data.x, rhs.Data.y, rhs.Data.z, rhs.Data.w) {}
    inline Quat(Quat&& rhs) noexcept
        : Data(std::exchange(rhs.Data.x, 0.0f), std::exchange(rhs.Data.y, 0.0f), std::exchange(rhs.Data.z, 0.0f),
              std::exchange(rhs.Data.w, 0.0f))
    {
    }

    // operators
    bool operator==(const Quat& other) const noexcept;
    bool operator!=(const Quat& other) const noexcept;
    bool IsNearEqual(const Quat& other, float epsilon = 1e-4f) const noexcept;

    // Methods
    [[nodiscard]] inline DirectX::XMVECTOR ToSIMD() const noexcept { return DirectX::XMLoadFloat4A(&Data); }
    [[nodiscard]] Rotator ToRotator() const noexcept;

    [[nodiscard]] DirectX::XMVECTOR GetNormalizeSIMD() const noexcept;
    [[nodiscard]] Quat GetNormalize() const noexcept;
    [[nodiscard]] DirectX::XMVECTOR GetNormalizeEstSIMD() const noexcept;
    [[nodiscard]] Quat GetNormalizeEst() const noexcept;

    [[nodiscard]] DirectX::XMVECTOR MutiplySIMD(DirectX::FXMVECTOR otherQuat) const noexcept;
    [[nodiscard]] Quat Mutiply(const Quat& otherQuat) const noexcept;

    [[nodiscard]] DirectX::XMVECTOR InverseSIMD() const noexcept;
    [[nodiscard]] Quat Inverse() const noexcept;

    // Statics
    [[nodiscard]] static inline DirectX::XMVECTOR IdentitySIMD() noexcept { return DirectX::XMQuaternionIdentity(); }
    [[nodiscard]] static inline Quat Identity() noexcept { return Quat(IdentitySIMD()); }

    [[nodiscard]] static DirectX::XMVECTOR MultiplyTwoQuatsSIMD(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2) noexcept;
    [[nodiscard]] static Quat MultiplyTwoQuats(const Quat& quat1, const Quat& quat2) noexcept;

    [[nodiscard]] static DirectX::XMVECTOR InverseQuatSIMD(DirectX::FXMVECTOR quat) noexcept;
    [[nodiscard]] static Quat InverseQuat(const Quat& quat1) noexcept;

    [[nodiscard]] static DirectX::XMVECTOR SlerpQuatsSIMD(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2, float t) noexcept;
    [[nodiscard]] static Quat SlerpQuats(const Quat& quat1, const Quat& quat2, float t) noexcept;

    [[nodiscard]] static Rotator QuatToRotator(DirectX::FXMVECTOR quaternion) noexcept;
    [[nodiscard]] static Rotator QuatToRotator(const Quat& quat1) noexcept;

    [[nodiscard]] static DirectX::FXMVECTOR RotatorToQuatSIMD(const Rotator& rot) noexcept;
    [[nodiscard]] static Quat RotatorToQuat(const Rotator& rot) noexcept;

    [[nodiscard]] static DirectX::XMVECTOR MatrixToQuatSIMD(DirectX::FXMMATRIX matrix) noexcept;
    [[nodiscard]] static Quat MatrixToQuat(DirectX::FXMMATRIX matrix) noexcept;

    [[nodiscard]] static DirectX::XMMATRIX QuatToMatrix(DirectX::FXMVECTOR quat) noexcept;
    [[nodiscard]] static DirectX::XMMATRIX QuatToMatrix(const Quat& quat) noexcept;

    [[nodiscard]] static bool QuatsIsEqual(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2) noexcept;
    [[nodiscard]] static bool QuatsIsEqual(const Quat& quat1, const Quat& quat2) noexcept;

    [[nodiscard]] static bool QuatsIsNearEqual(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2, float epsilon = 1e-4f) noexcept;
    [[nodiscard]] static bool QuatsIsNearEqual(const Quat& quat1, const Quat& quat2, float epsilon = 1e-4f) noexcept;

    [[nodiscard]] static Vector RotateVector(const Vector& Vec, DirectX::FXMVECTOR quat) noexcept;
    [[nodiscard]] static Vector RotateVector(const Vector& Vec, const Quat& quat) noexcept;
};