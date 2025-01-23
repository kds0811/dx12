#include "Rotator.h"
#include <cstdlib>
#include <cmath>
#include "Vector.h"
#include <algorithm>
#include "Quat.h"

Rotator::Rotator(Quat quat)
{
    *this = Quat::QuatToRotator(quat);
}

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

DirectX::XMMATRIX Rotator::ToMatrix() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(GetPitch()), DirectX::XMConvertToRadians(GetYaw()), DirectX::XMConvertToRadians(GetRoll()));
}

Vector Rotator::GetForwardVector() const noexcept
{
    auto quat = ToQuatSIMD();
    // Forward = +Z
    auto forward = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), quat);
    return Vector(DirectX::XMVector3Normalize(forward));
}

Vector Rotator::GetRightVector() const noexcept
{
    auto quat = ToQuatSIMD();
    // Right = +X
    auto right = DirectX::XMVector3Rotate(DirectX::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), quat);
    return Vector(DirectX::XMVector3Normalize(right));
}

Vector Rotator::GetUpVector() const noexcept
{
    auto quat = ToQuatSIMD();
    // Up = +Y
    auto up = DirectX::XMVector3Rotate(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), quat);
    return Vector(DirectX::XMVector3Normalize(up));
}

Rotator Rotator::Lerp(const Rotator& start, const Rotator& end, float alpha) noexcept
{
    alpha = std::clamp(alpha, 0.0f, 1.0f);
    Rotator res;
    res.SetPitch(start.GetPitch() + alpha * (end.GetPitch() - start.GetPitch()));
    res.SetYaw(start.GetYaw() + alpha * (end.GetYaw() - start.GetYaw()));
    res.SetRoll(start.GetRoll() + alpha * (end.GetRoll() - start.GetRoll()));
    return res;
}

Rotator Rotator::Clamp(const Rotator& min, const Rotator& max) const noexcept
{
    return Rotator(
        std::clamp(Data.x, min.Data.x, max.Data.x), std::clamp(Data.y, min.Data.y, max.Data.y), std::clamp(Data.z, min.Data.z, max.Data.z));
}

DirectX::XMVECTOR Rotator::ToQuatSIMD() const noexcept
{
    auto quat = DirectX::XMQuaternionRotationRollPitchYaw(
        DirectX::XMConvertToRadians(GetPitch()), DirectX::XMConvertToRadians(GetYaw()), DirectX::XMConvertToRadians(GetRoll()));
    return DirectX::XMQuaternionNormalize(quat);
}

Quat Rotator::ToQuat() const noexcept
{
    return Quat(DirectX::XMQuaternionRotationRollPitchYaw(  
            DirectX::XMConvertToRadians(GetPitch()),  
            DirectX::XMConvertToRadians(GetYaw()),  
            DirectX::XMConvertToRadians(GetRoll())));
}

Rotator Rotator::FromQuat(DirectX::FXMVECTOR quat) noexcept
{
    return Quat::QuatToRotator(quat);
}

Rotator Rotator::FromQuat(const Quat& quat) noexcept
{
    return Quat::QuatToRotator(quat);
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
