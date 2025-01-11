#include "Vector.h"

Vector& Vector::operator+=(const Vector& other) noexcept
{
    auto v1 = DirectX::XMLoadFloat3(&Data);
    auto v2 = DirectX::XMLoadFloat3(&other.Data);
    DirectX::XMStoreFloat3(&Data, DirectX::XMVectorAdd(v1, v2));
    return *this;
}

Vector Vector::operator+(const Vector& other) const noexcept
{
    Vector result;
    auto v1 = DirectX::XMLoadFloat3(&Data);
    auto v2 = DirectX::XMLoadFloat3(&other.Data);
    DirectX::XMStoreFloat3(&result.Data, DirectX::XMVectorAdd(v1, v2));
    return result;
}

Vector& Vector::operator-=(const Vector& other) noexcept
{
    auto v1 = DirectX::XMLoadFloat3(&Data);
    auto v2 = DirectX::XMLoadFloat3(&other.Data);
    DirectX::XMStoreFloat3(&Data, DirectX::XMVectorSubtract(v1, v2));
    return *this;
}

Vector Vector::operator-(const Vector& other) const noexcept
{
    Vector result;
    auto v1 = DirectX::XMLoadFloat3(&Data);
    auto v2 = DirectX::XMLoadFloat3(&other.Data);
    DirectX::XMStoreFloat3(&result.Data, DirectX::XMVectorSubtract(v1, v2));
    return result;
}

Vector& Vector::AddAs(const Vector& other) noexcept
{
    Data.x += other.X();
    Data.y += other.Y();
    Data.z += other.Z();
    return *this;
}

Vector Vector::Add(const Vector& other) const noexcept
{
    Vector result;
    result.Data.x = Data.x + other.Data.x;
    result.Data.y = Data.y + other.Data.y;
    result.Data.z = Data.z + other.Data.z;
    return result;
}

Vector& Vector::SubAs(const Vector& other) noexcept
{
    Data.x -= other.X();
    Data.y -= other.Y();
    Data.z -= other.Z();
    return *this;
}

Vector Vector::Sub(const Vector& other) const noexcept
{
    Vector result;
    result.Data.x = Data.x - other.Data.x;
    result.Data.y = Data.y - other.Data.y;
    result.Data.z = Data.z - other.Data.z;
    return result;
}

float Vector::Length() const noexcept
{
    auto v = DirectX::XMLoadFloat3(&Data);
    return DirectX::XMVectorGetX(DirectX::XMVector3Length(v));
}

Vector Vector::Normalize() const noexcept
{
    Vector result;
    auto v = DirectX::XMLoadFloat3(&Data);
    DirectX::XMStoreFloat3(&result.Data, DirectX::XMVector3Normalize(v));
    return result;
}

float Vector::Dot(const Vector& other) const noexcept
{
    auto v1 = DirectX::XMLoadFloat3(&Data);
    auto v2 = DirectX::XMLoadFloat3(&other.Data);
    return DirectX::XMVectorGetX(DirectX::XMVector3Dot(v1, v2));
}
