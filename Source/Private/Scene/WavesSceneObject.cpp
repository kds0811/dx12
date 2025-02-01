#include "WavesSceneObject.h"

WavesSceneObject::WavesSceneObject(ePrimitiveType objectType, Transform objectTransformation, int scneneCounter,
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries, EMaterialType matType,
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials)
{
    mObjectPrimitiveType = objectType;
    mMaterialType = matType;
    mWaves = std::make_unique<Waves>(145, 145, 1.0f, 0.03f, 4.0f, 0.2f);
    mSceneComponent.SetTransformation(objectTransformation);

    CreateRenderItem(scneneCounter, geometries, materials);
}

void WavesSceneObject::Update(float dt)
{
    BaseSceneObject::Update(dt);
}

int WavesSceneObject::GetVertexCount()
{
    return mWaves->VertexCount();
}

void WavesSceneObject::CreateRenderItem(int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials)
{
    mRenderItem = std::make_unique<RenderItem>();
    ObjCBIndex = sceneCounter;
    mRenderItem->Geo = geometries["waterGeo"].get();
    mRenderItem->Mat = materials[mMaterialType].get();
    mRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    mRenderItem->IndexCount = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].IndexCount;
    mRenderItem->StartIndexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].StartIndexLocation;
    mRenderItem->BaseVertexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].BaseVertexLocation;
}
