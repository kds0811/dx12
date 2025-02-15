#include "PrimitiveSceneObject.h"
#include <cassert>

PrimitiveSceneObject::PrimitiveSceneObject(EPrimitiveType objectType, Transform objectTransformation, UINT scneneCounter,
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries, EMaterialType matType,
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials, ERenderLayer renderLayer)
{
    mObjectPrimitiveType = objectType;
    mMaterialType = matType;
    mRenderLayer = renderLayer;
    mSceneComponent.SetTransformation(objectTransformation);
    CreateRenderItem(scneneCounter, geometries, materials);
}

void PrimitiveSceneObject::Update(float dt) 
{
    BaseSceneObject::Update(dt);
}


void PrimitiveSceneObject::CreateRenderItem(UINT sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials)
{
    mRenderItem = std::make_unique<RenderItem>();
    mRenderItem->ObjCBIndex = sceneCounter;
    mRenderItem->Geo = geometries["shapeGeo"].get();
    mRenderItem->Mat = materials[mMaterialType].get();
    mRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    mRenderItem->IndexCount = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].IndexCount;
    mRenderItem->StartIndexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].StartIndexLocation;
    mRenderItem->BaseVertexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].BaseVertexLocation;

    mObjCBIndexRef = mRenderItem->ObjCBIndex + 1;
    mObjCBIndexShadow = mRenderItem->ObjCBIndex + 2;
    mShadowMat = materials[EMaterialType::SHADOW].get();
    
}
