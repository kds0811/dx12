#include "Rotator.h"
#include <cstdlib>
#include <cmath>
#include "Vector.h"
#include <algorithm>

Rotator& Rotator::operator+=(const Rotator& other) noexcept
{
    Data.x += other.Data.x;
    Data.y += other.Data.y;
    Data.z += other.Data.z;
    return *this;
}

Rotator Rotator::operator+(const Rotator& other) const noexcept
{
    Rotator result;
    result.Data.x = Data.x + other.Data.x;
    result.Data.y = Data.y + other.Data.y;
    result.Data.z = Data.z + other.Data.z;
    return result;
}

Rotator& Rotator::operator-=(const Rotator& other) noexcept
{
    Data.x -= other.Data.x;
    Data.y -= other.Data.y;
    Data.z -= other.Data.z;
    return *this;
}

Rotator Rotator::operator-(const Rotator& other) const noexcept
{
    Rotator result;
    result.Data.x = Data.x - other.Data.x;
    result.Data.y = Data.y - other.Data.y;
    result.Data.z = Data.z - other.Data.z;
    return result;
}

Rotator& Rotator::operator*=(const float& sc) noexcept
{
    DirectX::XMStoreFloat3A(&Data, (DirectX::XMVectorScale(DirectX::XMLoadFloat3A(&Data), sc)));
    return *this;
}

Rotator Rotator::operator*(const float& sc) const noexcept
{
    return Rotator(DirectX::XMVectorScale(DirectX::XMLoadFloat3A(&Data), sc));
}

Rotator& Rotator::operator/=(const float& dv) noexcept
{
    DirectX::XMStoreFloat3A(&Data, (DirectX::XMVectorScale(DirectX::XMLoadFloat3A(&Data), 1 / dv)));
    return *this;
}

Rotator Rotator::operator/(const float& dv) const noexcept
{
    return Rotator(DirectX::XMVectorScale(DirectX::XMLoadFloat3A(&Data), 1 / dv));
}

bool Rotator::operator==(const Rotator& other) const noexcept
{
    const float epsilon = 0.001;
    return (std::abs(Data.x - other.Data.x) < epsilon) && (std::abs(Data.y - other.Data.y) < epsilon) &&
           (std::abs(Data.z - other.Data.z) < epsilon);
}

bool Rotator::operator!=(const Rotator& other) const noexcept
{
    return !(*this == other);
}

bool Rotator::NearEqual(const Rotator& other, float epsilon) const noexcept
{
    return (std::abs(Data.x - other.Data.x) < epsilon) && (std::abs(Data.y - other.Data.y) < epsilon) &&
           (std::abs(Data.z - other.Data.z) < epsilon);
}

Rotator Rotator::Normalize180() const noexcept
{
    auto NormalizeAngle = [](float angle) -> float
    {
        angle = std::fmod(angle, 360.0f);
        if (angle > 180.0f)
        {
            angle -= 360.0f;
        }
        else if (angle < -180.0f)
        {
            angle += 360.0f;
        }

        return angle;
    };

    return Rotator(NormalizeAngle(Data.x), NormalizeAngle(Data.y), NormalizeAngle(Data.z));
}

Rotator Rotator::Normalize360() const noexcept
{
    auto NormalizeAngle = [](float angle) -> float
    {
        angle = std::fmod(angle, 360.0f);
        if (angle < 0.0f)
        {
            angle += 360.0f;
        }
        return angle;
    };

    return Rotator(NormalizeAngle(Data.x), NormalizeAngle(Data.y), NormalizeAngle(Data.z));
}

DirectX::XMVECTOR Rotator::ToQuaternion() const noexcept
{
    return DirectX::XMQuaternionRotationRollPitchYaw(
        DirectX::XMConvertToRadians(Data.x), DirectX::XMConvertToRadians(Data.y), DirectX::XMConvertToRadians(Data.z));
}

Rotator Rotator::FromQuaternion(DirectX::FXMVECTOR quaternion) noexcept
{
    DirectX::XMMATRIX rotMatrix = DirectX::XMMatrixRotationQuaternion(quaternion);

    DirectX::XMVECTOR angles = DirectX::XMVectorSet(
        -DirectX::XMVectorGetY(rotMatrix.r[2]), DirectX::XMVectorGetX(rotMatrix.r[2]), DirectX::XMVectorGetZ(rotMatrix.r[2]), 0.0f);

    DirectX::XMVECTOR eulerAngles =
        DirectX::XMVectorSet(std::atan2(DirectX::XMVectorGetY(rotMatrix.r[2]), DirectX::XMVectorGetZ(rotMatrix.r[2])),  // pitch
            -std::asin(DirectX::XMVectorGetX(rotMatrix.r[2])),                                                          // yaw
            std::atan2(DirectX::XMVectorGetX(rotMatrix.r[1]), DirectX::XMVectorGetX(rotMatrix.r[0])),                   // roll
            0.0f);

    eulerAngles = DirectX::XMVectorMultiply(eulerAngles, DirectX::XMVectorReplicate(180.0f / DirectX::XM_PI));

    return Rotator(eulerAngles);
}

Vector Rotator::GetForwardVector() const noexcept
{
    auto quat = ToQuaternion();
    // Forward = +Z
    auto forward = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), quat);
    return Vector(DirectX::XMVector3Normalize(forward));
}

Vector Rotator::GetRightVector() const noexcept
{
    auto quat = ToQuaternion();
    // Right = +X
    auto right = DirectX::XMVector3Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), quat);
    return Vector(DirectX::XMVector3Normalize(right));
}

Vector Rotator::GetUpVector() const noexcept
{
    auto quat = ToQuaternion();
    // Up = +Y
    auto up = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), quat);
    return Vector(DirectX::XMVector3Normalize(up));
}  

Rotator Rotator::Lerp(const Rotator& start, const Rotator& end, float alpha) noexcept
{
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    auto startQuat = start.ToQuaternion();
    auto endQuat = end.ToQuaternion();
    auto resultQuat = DirectX::XMQuaternionSlerp(startQuat, endQuat, alpha);
    return FromQuaternion(resultQuat);
}


Rotator Rotator::Clamp(const Rotator& min, const Rotator& max) const noexcept
{
    return Rotator(
        std::clamp(Data.x, min.Data.x, max.Data.x), std::clamp(Data.y, min.Data.y, max.Data.y), std::clamp(Data.z, min.Data.z, max.Data.z));
}

