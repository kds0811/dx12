#pragma once
#include "SceneComponent.h"
#include "RenderItem.h"
#include "MovementComponent.h"
#include <memory>

class alignas (16) BaseSceneObject
{

public:
    BaseSceneObject() : mMovementComponent(mSceneComponent)
    {  }
    virtual ~BaseSceneObject() {}

    virtual void Update(float dt);
    RenderItem* GetRenderItem() { return mRenderItem.get(); }
    void SetContiniusRotation(Rotator rot);

    inline int GetNumFramesDirty() const noexcept { return mSceneComponent.GetNumFramesDirty(); }
    inline void DecrementNumFrameDirty() noexcept { mSceneComponent.DecrementNumFrameDirty(); }

    [[nodiscard]] inline DirectX::XMMATRIX GetLookAtMatrix() const noexcept { return mSceneComponent.GetLookAtMatrix(); }
    [[nodiscard]] inline DirectX::XMMATRIX GetWorldMatrix() const noexcept { return mSceneComponent.GetWorldMatrix(); }
    [[nodiscard]] inline UINT GetObjCBIndex() const noexcept { return ObjCBIndex; }

protected:
    SceneComponent mSceneComponent;
    MovementComponent mMovementComponent;
    std::unique_ptr<RenderItem> mRenderItem;
    ePrimitiveType mObjectPrimitiveType;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    UINT ObjCBIndex = -1;
};