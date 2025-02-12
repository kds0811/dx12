#pragma once
#include "BaseSceneObject.h"
#include <memory>
#include "Waves.h"


class WavesSceneObject final : public BaseSceneObject
{
    std::unique_ptr<Waves> mWaves = nullptr;

public:
    WavesSceneObject(EPrimitiveType objectType, Transform objectTransformation, int scneneCounter,
        std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries, EMaterialType matType,
        std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials, ERenderLayer renderLayer);

    virtual void Update(float dt) override final;

    int GetVertexCount();
    Waves* GetWaves() { return mWaves.get(); }
    RenderItem* GetRenderItem() { return mRenderItem.get(); }

private:
    void CreateRenderItem(int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
        std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials);
};