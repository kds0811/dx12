#include "WavesSceneObject.h"
#include "Waves.h"

WavesSceneObject::WavesSceneObject()
{
    mWaves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);

}
