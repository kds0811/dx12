#include "PrimitiveSceneObject.h"
#include <cassert>

PrimitiveSceneObject::PrimitiveSceneObject(ePrimitiveType objectType, Transform objectTransformation,
    int scneneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries)
{
    mObjectPrimitiveType = objectType;
    mSceneComponent.SetTransformation(objectTransformation);
    CreateRenderItem(scneneCounter, geometries);
}


void PrimitiveSceneObject::CreateRenderItem(
     int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries)
{
    mRenderItem = std::make_unique<RenderItem>();
    ObjCBIndex = sceneCounter;
    mRenderItem->Geo = geometries["shapeGeo"].get();
    mRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    mRenderItem->IndexCount = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].IndexCount;
    mRenderItem->StartIndexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].StartIndexLocation;
    mRenderItem->BaseVertexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].BaseVertexLocation;
}
