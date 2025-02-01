#pragma once
#include "BaseSceneObject.h"
#include <memory>

class Waves;

class WavesSceneObject : public BaseSceneObject
{
    std::unique_ptr<Waves> mWaves = nullptr;

public:
    WavesSceneObject(ePrimitiveType objectType, Transform objectTransformation, int scneneCounter,
        std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);

    virtual void Update(float dt) override;

    int GetVertexCount();

private:
    void CreateRenderItem(int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries);
};