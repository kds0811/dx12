#pragma once
#include "BaseSceneObject.h"
#include <memory>

class Waves;

class WavesSceneObject : public BaseSceneObject
{
    std::unique_ptr<Waves> mWaves = nullptr;

public:
    WavesSceneObject(ePrimitiveType objectType, Transform objectTransformation, int scneneCounter);

private:
    void CreateRenderItem(ePrimitiveType objectType, int sceneCounter);
};