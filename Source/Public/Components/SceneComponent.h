#pragma once
#include "Transform.h"
#include <DirectXMath.h>
#include "GraphicGlobals.h"
#include "MovementComponent.h"
#include "BaseComponent.h"

class SceneComponent : public BaseComponent
{
    Transform mTrans;
    MovementComponent mMovementComponent;
    int mNumFramesDirty = GG::gNumFrameResources;
    int mObjCBIndex = -1;

public:
    SceneComponent(SceneObject* owner);
    SceneComponent(SceneObject* owner, Transform trans);
    SceneComponent(SceneObject* owner, Vector loc);
    SceneComponent(SceneObject* owner,Vector loc, Rotator rot);

    // Movement Component Interface
    void Update(float dt);
    void SetTargetLocation(Vector targetLocation);
    void SetTargetRotation(Rotator targetRotation);
    void SetContinuesRotation(Rotator rotation);

    // Update mObjCBIndex interface
    inline void SetObjCBIndex(int objCBIndex) noexcept { mObjCBIndex = objCBIndex; }
    inline int GetObjCBIndex() noexcept { return mObjCBIndex; }

    // Setters
    inline void SetTransformation(Transform trans) noexcept
    {
        mTrans = trans;
        mNumFramesDirty = GG::gNumFrameResources;
    }
    inline void SetLocation(Vector location) noexcept
    {
        mTrans.SetLocation(location);
        mNumFramesDirty = GG::gNumFrameResources;
    }
    inline void SetRotation(Rotator rotation) noexcept
    {
        mTrans.SetRotation(rotation);
        mNumFramesDirty = GG::gNumFrameResources;
    }
    inline void SetScale(Vector scale) noexcept { mTrans.SetScale(scale); }
    inline void AddLocation(Vector location) noexcept
    {
        mTrans.AddLocation(location);
        mNumFramesDirty = GG::gNumFrameResources;
    }
    inline void AddRotation(Rotator rotation) noexcept
    {
        mTrans.AddRotation(rotation);
        mNumFramesDirty = GG::gNumFrameResources;
    }
    inline void AddScale(Vector scale) noexcept
    {
        mTrans.AddScale(scale);
        mNumFramesDirty = GG::gNumFrameResources;
    }

    inline int GetNumFramesDirty() const noexcept { return mNumFramesDirty; }
    inline void DecrementNumFrameDirty() noexcept { --mNumFramesDirty; }

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