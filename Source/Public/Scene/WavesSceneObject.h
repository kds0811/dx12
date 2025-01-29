#pragma once
#include "BaseSceneObject.h"

#include <memory>

class WavesSceneObject : public BaseSceneObject
{
    std::unique_ptr<Waves> mWaves = nullptr;

public:
    WavesSceneObject();
};