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


    void SetMaterialTransformation(Transform trs) noexcept;
    void SetMaterialTranslation(Vector vec) noexcept;
    void SetMaterialRotation(Rotator rot) noexcept;
    void SetMaterialScale(Vector scale) noexcept;

    void SetTextureTransformation(Transform trs) noexcept;
    void SetTextureTranslation(Vector vec) noexcept;
    void SetTextureRotation(Rotator rot) noexcept;
    void SetTextureScale(Vector scale) noexcept;

    ERenderLayer& GetRenderLayer() noexcept;

protected:
    SceneComponent mSceneComponent;
    MovementComponent mMovementComponent;
    std::unique_ptr<RenderItem> mRenderItem;
    EPrimitiveType mObjectPrimitiveType;
    EMaterialType mMaterialType;
    ERenderLayer mRenderLayer;

};