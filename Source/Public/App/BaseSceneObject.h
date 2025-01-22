#pragma once
#include "Transform.h"

class BaseSceneObject
{

public:
    virtual void Update() = 0;

protected:
    Transform mTransform;
};