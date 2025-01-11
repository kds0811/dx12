#include "Vector.h"

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
    Vector result;
    DirectX::XMStoreFloat3(&result.Data, DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&Data)));
    return result;
}

float Vector::Dot(const Vector& other) const noexcept
{
    auto v1 = DirectX::XMLoadFloat3(&Data);
    auto v2 = DirectX::XMLoadFloat3(&other.Data);
    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1, v2));
}


