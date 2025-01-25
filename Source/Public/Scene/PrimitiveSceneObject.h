#pragma once
#include "BaseSceneObject.h"
#include "Transform.h"

enum class ePrimitiveType
{
    BOX,
    GRID,
    CYLINDER,
    SPHERE
};

class PrimitiveSceneObject : public BaseSceneObject
{
    ePrimitiveType mObjectType;

public:
    PrimitiveSceneObject(ePrimitiveType objectType, Transform objectTransformation);
    


};