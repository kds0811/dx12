#pragma once
#include <DirectXMath.h>
#include "Vector.h"
#include "Rotator.h"
#include "Quat.h"

struct alignas(16) Transform
{
private:
    Vector Location;
    Rotator Rotation; 
    Vector Scale;

public:
    inline Transform() noexcept : Location(0.0f, 0.0f, 0.0f), Rotation(0.0f, 0.0f, 0.0f), Scale(1.0f, 1.0f, 1.0f) {}
    inline Transform(Vector location) noexcept : Location(location), Rotation(0.0f, 0.0f, 0.0f), Scale(1.0f, 1.0f, 1.0f) {}
    inline Transform(Vector location, Rotator rotation) noexcept : Location(location), Rotation(rotation), Scale(1.0f, 1.0f, 1.0f) {}
    inline Transform(Vector location, Rotator rotation, Vector scale) noexcept : Location(location), Rotation(rotation), Scale(scale) {}
    inline Transform(Rotator rotation) noexcept : Location(0.0f, 0.0f, 0.0f), Rotation(rotation), Scale(1.0f, 1.0f, 1.0f) {}
    inline Transform(Rotator rotation, Vector scale) noexcept : Location(0.0f, 0.0f, 0.0f), Rotation(rotation), Scale(scale) {}

    // standart members
    Transform& operator=(const Transform& rhs) noexcept
    {
        if (this != &rhs)
        {
            Location = rhs.Location;
            Rotation = rhs.Rotation;
            Scale = rhs.Scale;
        }
        return *this;
    }
    inline Transform(const Transform& rhs) noexcept
        : Location(rhs.Location), Rotation(rhs.Rotation), Scale(rhs.Scale)
    {}

    // Setters
    inline void SetLocation(Vector location) noexcept{ Location = location;}
    inline void SetRotation(Rotator rotation) noexcept { Rotation = rotation; }
    inline void SetScale(Vector scale) noexcept { Scale = scale; }
    inline void AddLocation(Vector location) noexcept { Location += location; }
    inline void AddRotation(Rotator rotation) noexcept { Rotation += rotation; }
    inline void AddScale(Vector scale) noexcept { Scale += scale; }

    // Getters
    [[nodiscard]] inline Vector GetLocation() const noexcept { return Location; }
    [[nodiscard]] inline Rotator GetRotation() const noexcept { return Rotation; }
    [[nodiscard]] inline Vector GetScale() const noexcept { return Scale; }
    [[nodiscard]] inline Vector GetForwardVector() const noexcept { return Rotation.GetForwardVector(); }
    [[nodiscard]] inline Vector GetRightVector() const noexcept { return Rotation.GetRightVector(); }
    [[nodiscard]] inline Vector GetUpVector() const noexcept { return Rotation.GetUpVector(); }

    //  operators
    inline Transform& operator+=(const Transform& other) noexcept
    {
        Location += other.Location;
        Rotation += other.Rotation;
        Scale += other.Scale;
        return *this;
    }
    inline Transform operator+(const Transform& other) const noexcept
    { 
        Transform result;
        result.Location = Location + other.Location;
        result.Rotation = Rotation + other.Rotation;
        result.Scale = Scale + other.Scale;
        return result;
    }
    inline Transform& operator-=(const Transform& other) noexcept
    {
        Location -= other.Location;
        Rotation -= other.Rotation;
        Scale -= other.Scale;
        return *this;
    }
    inline Transform operator-(const Transform& other) const noexcept
    {
        Transform result;
        result.Location = Location - other.Location;
        result.Rotation = Rotation - other.Rotation;
        result.Scale = Scale - other.Scale;
        return result;
    }



    // Look at matrix
    [[nodiscard]] inline DirectX::XMMATRIX GetLookAtMatrix() const noexcept
    {
        return DirectX::XMMatrixLookAtLH(Location.ToSIMD(), Rotation.GetForwardVector().ToSIMD(), Rotation.GetUpVector().ToSIMD());
    }

    [[nodiscard]] inline DirectX::XMMATRIX GetWorldMatrix() const noexcept
    {
        DirectX::XMMATRIX scale = DirectX::XMMatrixScalingFromVector(GetScale().ToSIMD());
        DirectX::XMMATRIX rotation = DirectX::XMMatrixRotationQuaternion(GetRotation().ToQuatSIMD());
        DirectX::XMMATRIX translation = DirectX::XMMatrixTranslationFromVector(GetLocation().ToSIMD());

        return scale * rotation * translation;  
    }
};