#include "PrimitiveSceneObject.h"

PrimitiveSceneObject::PrimitiveSceneObject(ePrimitiveType objectType, Transform objectTransformation) : mObjectType(objectType)
{
    mSceneComponent.SetTransformation(objectTransformation);
}