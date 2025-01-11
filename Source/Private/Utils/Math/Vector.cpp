#include "Vector.h"
#include <cstdlib>

Vector& Vector::operator+=(const Vector& other) noexcept
{
    Data.x += other.Data.x;
    Data.y += other.Data.y;
    Data.z += other.Data.z;
    return *this;
}

Vector Vector::operator+(const Vector& other) const noexcept
{
    Vector result;
    result.Data.x = Data.x + other.Data.x;
    result.Data.y = Data.y + other.Data.y;
    result.Data.z = Data.z + other.Data.z;
    return result;
}

Vector& Vector::operator-=(const Vector& other) noexcept
{
    Data.x -= other.Data.x;
    Data.y -= other.Data.y;
    Data.z -= other.Data.z;
    return *this;
}

Vector Vector::operator-(const Vector& other) const noexcept
{
    Vector result;
    result.Data.x = Data.x - other.Data.x;
    result.Data.y = Data.y - other.Data.y;
    result.Data.z = Data.z - other.Data.z;
    return result;
}

Vector& Vector::operator*=(const float& sc) noexcept
{
    Data.x *= sc;
    Data.y *= sc;
    Data.z *= sc;
    return *this;
}

Vector Vector::operator*(const float& sc) const noexcept
{
    Vector result;
    result.Data.x = Data.x * sc;
    result.Data.y = Data.y * sc;
    result.Data.z = Data.z * sc;
    return result;
}

Vector& Vector::operator/=(const float& dv) noexcept
{
    Data.x /= dv;
    Data.y /= dv;
    Data.z /= dv;
    return *this;
}

Vector Vector::operator/(const float& dv) const noexcept
{
    Vector result;
    result.Data.x = Data.x / dv;
    result.Data.y = Data.y / dv;
    result.Data.z = Data.z / dv;
    return result;
}

bool Vector::operator==(const Vector& other) const noexcept
{
    return Data.x == other.Data.x && Data.y == other.Data.y && Data.z == other.Data.z;
}

bool Vector::operator!=(const Vector& other) const noexcept
{
    return !(*this == other);
}

bool Vector::NearEqual(const Vector& other, float epsilon) const noexcept
{
    return std::abs(Data.x - other.Data.x) < epsilon && std::abs(Data.y - other.Data.y) < epsilon &&
           std::abs(Data.z - other.Data.z) < epsilon;
}

Vector Vector::SIMDMul(const float& sc) const noexcept
{
    return Vector(DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Data), sc));
}

Vector Vector::SIMDDiv(const float& dv) const noexcept
{
    return Vector(DirectX::XMVectorScale(DirectX::XMLoadFloat3(&Data), 1 / dv));
}

Vector Vector::Abs() const noexcept
{
    return Vector(DirectX::XMVectorAbs(ToSIMD()));
}

float Vector::Length() const noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMLoadFloat3(&Data)));
}

float Vector::LengthSq() const noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthSq(DirectX::XMLoadFloat3(&Data)));
}

float Vector::LengthEst() const noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3LengthEst(DirectX::XMLoadFloat3(&Data)));
}

Vector Vector::Normalize() const noexcept
{
    return Vector(DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Data)));
}

float Vector::Dot(const Vector& other) const noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(DirectX::XMLoadFloat3(&Data), DirectX::XMLoadFloat3(&other.Data)));
}

Vector Vector::Cross(const Vector& other) const noexcept
{
    return Vector(DirectX::XMVector3Cross(ToSIMD(), other.ToSIMD()));
}

float Vector::Angle(const Vector& other) const noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3AngleBetweenVectors(ToSIMD(), other.ToSIMD()));
}

Vector Vector::Clamp(const Vector& min, const Vector& max) const noexcept
{
    return Vector(DirectX::XMVectorClamp(ToSIMD(), min.ToSIMD(), max.ToSIMD()));
}

float Vector::Distance(const Vector& a, const Vector& b) noexcept
{
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(DirectX::XMVectorSubtract(a.ToSIMD(), b.ToSIMD())));
}

Vector Vector::Lerp(const Vector& start, const Vector& end, float t) noexcept
{
    return Vector(DirectX::XMVectorLerp(start.ToSIMD(), end.ToSIMD(), t));
}



