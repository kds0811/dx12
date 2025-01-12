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
    if (dv != 0.0f)
    {
        DirectX::XMStoreFloat3A(&Data, (DirectX::XMVectorScale(DirectX::XMLoadFloat3A(&Data), 1 / dv)));
    }
    return *this;
}

Rotator Rotator::operator/(const float& dv) const noexcept
{
    if (dv != 0.0f)
    {
        return Rotator(DirectX::XMVectorScale(DirectX::XMLoadFloat3A(&Data), 1 / dv));
    }
    return Rotator::Zero();
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

bool Rotator::IsNearEqual(const Rotator& other, float epsilon) const noexcept
{
    return (std::abs(Data.x - other.Data.x) < epsilon) && (std::abs(Data.y - other.Data.y) < epsilon) &&
           (std::abs(Data.z - other.Data.z) < epsilon);
}

Rotator Rotator::Normalize180() const noexcept
{
    // Загружаем углы в SIMD вектор
    DirectX::XMVECTOR angles = ToSIMD();

    // Преобразуем в диапазон [-180, 180]
    angles = DirectX::XMVectorMod(angles, DirectX::XMVectorReplicate(360.0f));

    // Корректируем углы > 180
    DirectX::XMVECTOR mask = DirectX::XMVectorGreater(angles, DirectX::XMVectorReplicate(180.0f));
    angles = DirectX::XMVectorSelect(angles, DirectX::XMVectorSubtract(angles, DirectX::XMVectorReplicate(360.0f)), mask);

    // Корректируем углы < -180
    mask = DirectX::XMVectorLess(angles, DirectX::XMVectorReplicate(-180.0f));
    angles = DirectX::XMVectorSelect(angles, DirectX::XMVectorAdd(angles, DirectX::XMVectorReplicate(360.0f)), mask);

    return Rotator(angles);
}

Rotator Rotator::Normalize360() const noexcept
{
    // Загружаем углы в SIMD вектор
    DirectX::XMVECTOR angles = ToSIMD();

    // Преобразуем в диапазон [0, 360]
    angles = DirectX::XMVectorMod(angles, DirectX::XMVectorReplicate(360.0f));

    // Корректируем отрицательные углы
    DirectX::XMVECTOR mask = DirectX::XMVectorLess(angles, DirectX::XMVectorZero());
    angles = DirectX::XMVectorSelect(angles, DirectX::XMVectorAdd(angles, DirectX::XMVectorReplicate(360.0f)), mask);

    return Rotator(angles);
}


DirectX::XMVECTOR Rotator::ToQuaternion() const noexcept
{
    return DirectX::XMQuaternionRotationRollPitchYaw(
        DirectX::XMConvertToRadians(Data.x), DirectX::XMConvertToRadians(Data.y), DirectX::XMConvertToRadians(Data.z));
}

Rotator Rotator::FromQuaternion(DirectX::FXMVECTOR quaternion) noexcept
{
    using namespace DirectX;

    // Получаем компоненты кватерниона
    float w = XMVectorGetW(quaternion);
    float x = XMVectorGetX(quaternion);
    float y = XMVectorGetY(quaternion);
    float z = XMVectorGetZ(quaternion);

    const float RAD_TO_DEG = 180.0f / XM_PI;
    float Pitch, Yaw, Roll;

    // Нормализуем кватернион
    float length = std::sqrt(w * w + x * x + y * y + z * z);
    if (length != 0.0f)
    {
        w /= length;
        x /= length;
        y /= length;
        z /= length;
    }

    // Преобразуем в углы Эйлера
    float sinp = 2.0f * (w * y - z * x);
    if (std::abs(sinp) >= 1.0f)
    {
        // Случай gimbal lock
        Pitch = std::copysignf(90.0f, sinp);
        Roll = 0.0f;
        Yaw = 2.0f * std::atan2f(x, w) * RAD_TO_DEG;
    }
    else
    {
        // Нормальный случай
        Pitch = std::asinf(sinp) * RAD_TO_DEG;

        float sinr_cosp = 2.0f * (w * x + y * z);
        float cosr_cosp = 1.0f - 2.0f * (x * x + y * y);
        Roll = std::atan2f(sinr_cosp, cosr_cosp) * RAD_TO_DEG;

        float siny_cosp = 2.0f * (w * z + x * y);
        float cosy_cosp = 1.0f - 2.0f * (y * y + z * z);
        Yaw = std::atan2f(siny_cosp, cosy_cosp) * RAD_TO_DEG;
    }

    // Нормализуем углы
    if (Roll > 180.0f)
        Roll -= 360.0f;
    else if (Roll < -180.0f)
        Roll += 360.0f;

    if (Yaw > 180.0f)
        Yaw -= 360.0f;
    else if (Yaw < -180.0f)
        Yaw += 360.0f;

    return Rotator(Pitch, Yaw, Roll);
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

float Rotator::NormalizeAxis(float Angle)
{
    // returns Angle in the range [0,360)
    Angle = ClampAxis(Angle);

    if (Angle > 180.0f)
    {
        // shift to (-180,180]
        Angle -= 360.0f;
    }

    return Angle;
}

float Rotator::ClampAxis(float Angle)
{
    Angle = std::fmod(Angle, 360.0f);
    if (Angle < 0.0f)
    {
        Angle += 360.0f;
    }
    return Angle;
}

