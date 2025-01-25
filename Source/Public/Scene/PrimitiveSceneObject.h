#pragma once
#include "BaseSceneObject.h"
#include "Transform.h"

class PrimitiveSceneObject : public BaseSceneObject
{
    ePrimitiveType mObjectPrimitiveType;

public:
    PrimitiveSceneObject(ePrimitiveType objectType, Transform objectTransformation, int scneneCounter,
        std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);

private:
    void CreateRenderItem(int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);
};