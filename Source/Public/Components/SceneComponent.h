#pragma once
#include "Transform.h"
#include <DirectXMath.h>


class alignas(16) SceneComponent
{
    Transform mTrans;

public:
    SceneComponent() : mTrans(Vector::Zero(), Rotator::Zero(), Vector::One()) {}
    SceneComponent(Transform trans) : mTrans(trans) {}
    SceneComponent(Vector loc) : mTrans(loc) {}
    SceneComponent(Vector loc, Rotator rot) : mTrans(loc, rot) {}

    // Setters
    inline void SetTransformation(Transform trans) noexcept { mTrans = trans; }
    inline void SetLocation(Vector location) noexcept { mTrans.SetLocation(location); }
    inline void SetRotation(Rotator rotation) noexcept { mTrans.SetRotation(rotation); }
    inline void SetScale(Vector scale) noexcept { mTrans.SetScale(scale); }
    inline void AddLocation(Vector location) noexcept { mTrans.AddLocation(location); }
    inline void AddRotation(Rotator rotation) noexcept { mTrans.AddRotation(rotation); }
    inline void AddScale(Vector scale) noexcept { mTrans.AddScale(scale); }

    // Getters
    [[nodiscard]] inline Vector GetLocation() const noexcept { return mTrans.GetLocation(); }
    [[nodiscard]] inline Rotator GetRotation() const noexcept { return mTrans.GetRotation(); }
    [[nodiscard]] inline Quat GetQuat() const noexcept { return mTrans.GetRotation().ToQuat(); }
    [[nodiscard]] inline Vector GetScale() const noexcept { return mTrans.GetScale(); }
    [[nodiscard]] inline Vector GetForwardVector() const noexcept { return mTrans.GetForwardVector(); }
    [[nodiscard]] inline Vector GetRightVector() const noexcept { return mTrans.GetRightVector(); }
    [[nodiscard]] inline Vector GetUpVector() const noexcept { return mTrans.GetUpVector(); }
    [[nodiscard]] inline DirectX::XMMATRIX GetLookAtMatrix() const noexcept { return mTrans.GetLookAtMatrix(); }
    [[nodiscard]] inline DirectX::XMMATRIX GetWorldMatrix() const noexcept { return mTrans.GetWorldMatrix(); }

    

};