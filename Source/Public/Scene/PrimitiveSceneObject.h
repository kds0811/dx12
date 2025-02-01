#pragma once
#include "BaseSceneObject.h"
#include "Transform.h"

class PrimitiveSceneObject final : public BaseSceneObject
{
public:
    PrimitiveSceneObject(ePrimitiveType objectType, Transform objectTransformation, int scneneCounter,
        std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);

    virtual void Update(float dt) override final;

private:
    void CreateRenderItem(int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);
};