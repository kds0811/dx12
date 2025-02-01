#include "WavesSceneObject.h"
#include "Waves.h"

WavesSceneObject::WavesSceneObject(ePrimitiveType objectType, Transform objectTransformation, int scneneCounter,
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries)
{
    mObjectPrimitiveType = objectType;
    mWaves = std::make_unique<Waves>(128, 128, 1.0f, 0.03f, 4.0f, 0.2f);
    mSceneComponent.SetTransformation(objectTransformation);

    CreateRenderItem(scneneCounter, geometries);
}

void WavesSceneObject::Update(float dt)
{
    BaseSceneObject::Update(dt);


}

int WavesSceneObject::GetVertexCount()
{
    return mWaves->VertexCount();
}

void WavesSceneObject::CreateRenderItem(int sceneCounter, std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& geometries)
{
    mRenderItem = std::make_unique<RenderItem>();
    ObjCBIndex = sceneCounter;
    mRenderItem->Geo = geometries["waterGeo"].get();
    mRenderItem->PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    mRenderItem->IndexCount = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].IndexCount;
    mRenderItem->StartIndexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].StartIndexLocation;
    mRenderItem->BaseVertexLocation = mRenderItem->Geo->DrawArgs[mObjectPrimitiveType].BaseVertexLocation;

    //
    // need add dynamic vertex buffer and solve Constant Buffer index which come from scene counter
    //
}
