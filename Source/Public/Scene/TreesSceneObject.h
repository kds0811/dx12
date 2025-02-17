#pragma once
#include "BaseSceneObject.h"
#include "Transform.h"

class TreesSceneObject final : public BaseSceneObject
{
public:
    TreesSceneObject(EPrimitiveType objectType, Transform objectTransformation, UINT scneneCounter,
        std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries, EMaterialType matType,
        std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials, ERenderLayer renderLayer);

    virtual void Update(float dt) override final;

private:
    void CreateRenderItem(UINT sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
        std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials);
};