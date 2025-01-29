#include "WavesSceneObject.h"
#include "Waves.h"

WavesSceneObject::WavesSceneObject(ePrimitiveType objectType, Transform objectTransformation, int sceneCounter)
{
    mObjectPrimitiveType = objectType;
    mWaves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
    mSceneComponent.SetTransformation(objectTransformation);
}



void WavesSceneObject::CreateRenderItem(ePrimitiveType objectType, int sceneCounter)
{
    mRenderItem = std::make_unique<RenderItem>();
    ObjCBIndex = sceneCounter;
   // mRenderItem->Geo = geometries["shapeGeo"].get();
    mRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    mRenderItem->IndexCount = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].IndexCount;
    mRenderItem->StartIndexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].StartIndexLocation;
    mRenderItem->BaseVertexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].BaseVertexLocation;
}
