#include "PrimitiveSceneObject.h"

PrimitiveSceneObject::PrimitiveSceneObject(ePrimitiveType objectType, Transform objectTransformation, DirectX::XMFLOAT4 color,
    int scneneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries)
    : mObjectPrimitiveType(objectType)
{
    mSceneComponent.SetTransformation(objectTransformation);
    CreateRenderItem(color, scneneCounter, geometries);
}


void PrimitiveSceneObject::CreateRenderItem(
    DirectX::XMFLOAT4 color, int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries)
{
    mRenderItem = std::make_unique<RenderItem>();
    XMStoreFloat4x4(&mRenderItem->World, mSceneComponent.GetWorldMatrix());
    mRenderItem->ObjCBIndex = sceneCounter;
    mRenderItem->Geo = geometries["shapeGeo"].get();
    mRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    mRenderItem->IndexCount = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].IndexCount;
    mRenderItem->StartIndexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].StartIndexLocation;
    mRenderItem->BaseVertexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].BaseVertexLocation;
}
