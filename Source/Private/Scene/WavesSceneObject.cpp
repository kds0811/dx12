#include "WavesSceneObject.h"

WavesSceneObject::WavesSceneObject(EPrimitiveType objectType, Transform objectTransformation, UINT scneneCounter,
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries, EMaterialType matType,
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials, ERenderLayer renderLayer)
{
    mObjectPrimitiveType = objectType;
    mMaterialType = matType;
    mRenderLayer = renderLayer;
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

void WavesSceneObject::CreateRenderItem(UINT sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries,
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials)
{
    mRenderItem = std::make_unique<RenderItem>();
    mRenderItem->ObjCBIndex = sceneCounter;
    mRenderItem->Geo = geometries["waterGeo"].get();
    mRenderItem->Mat = materials[mMaterialType].get();
    mRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    mRenderItem->IndexCount = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].IndexCount;
    mRenderItem->StartIndexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].StartIndexLocation;
    mRenderItem->BaseVertexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].BaseVertexLocation;

    mObjCBIndexRef = mRenderItem->ObjCBIndex + 1;
}
