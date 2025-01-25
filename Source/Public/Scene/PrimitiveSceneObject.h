#pragma once
#include "BaseSceneObject.h"
#include "Transform.h"


class PrimitiveSceneObject : public BaseSceneObject
{
    ePrimitiveType mObjectPrimitiveType;

public:
    PrimitiveSceneObject(ePrimitiveType objectType, Transform objectTransformation, DirectX::XMFLOAT4 Color, int scneneCounter,
        std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);

private:
    void CreateRenderItem(
        DirectX::XMFLOAT4 color, int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);
};