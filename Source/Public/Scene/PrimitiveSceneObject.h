#pragma once
#include "BaseSceneObject.h"
#include "Transform.h"


class PrimitiveSceneObject final : public BaseSceneObject
{
public:
    PrimitiveSceneObject(EPrimitiveType objectType, Transform objectTransformation, int scneneCounter,
        std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries, EMaterialType matType,
        std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials, ERenderLayer renderLayer);

    virtual void Update(float dt) override final;

private:
    void CreateRenderItem(int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
        std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials);
};