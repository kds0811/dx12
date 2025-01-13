#include "Rotator.h"
#include <cstdlib>
#include <cmath>
#include "Vector.h"
#include <algorithm>
#include "EulerAngles.h"

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

    auto NormQuat = XMQuaternionNormalize(quaternion);
    Quat qt;
    qt.w = XMVectorGetW(quaternion);
    qt.x = XMVectorGetX(quaternion);
    qt.y = XMVectorGetY(quaternion);
    qt.z = XMVectorGetZ(quaternion); 
    
    const float RAD_TO_DEG = 180.0f / XM_PI;

    auto eul = Eul_FromQuat(qt, EulOrdYXZr);
    
    return Rotator(eul.y * RAD_TO_DEG, eul.x * RAD_TO_DEG, eul.z * RAD_TO_DEG);
}

DirectX::XMMATRIX Rotator::ToMatrix() const noexcept
{
    return DirectX::XMMatrixRotationRollPitchYaw(
        DirectX::XMConvertToRadians(GetPitch()), DirectX::XMConvertToRadians(GetYaw()), DirectX::XMConvertToRadians(GetRoll()));
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

DirectX::XMVECTOR Rotator::MatrixToQuat(DirectX::FXMMATRIX matrix)
{
    return DirectX::XMQuaternionRotationMatrix(matrix);
}

bool Rotator::QuatsIsEqual(DirectX::FXMVECTOR quat1, DirectX::FXMVECTOR quat2)
{
    return DirectX::XMQuaternionEqual(quat1, quat2);
}

bool Rotator::QuaternionsAreEqual(DirectX::XMVECTOR quat1, DirectX::XMVECTOR quat2, float epsilon)
{
    DirectX::XMVECTOR normQuat1 = DirectX::XMQuaternionNormalize(quat1);
    DirectX::XMVECTOR normQuat2 = DirectX::XMQuaternionNormalize(quat2);

    float dot = DirectX::XMVectorGetX(DirectX::XMQuaternionDot(normQuat1, normQuat2));

    return (std::abs(dot) >= 1.0f - epsilon);
}
