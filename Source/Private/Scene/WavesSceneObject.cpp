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
    AnimateMaterial(dt);
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
    mObjCBIndexShadow = mRenderItem->ObjCBIndex + 2;
    mShadowMat = materials[EMaterialType::SHADOW].get();
}

void WavesSceneObject::AnimateMaterial(float dt) 
{
    // Scroll the water material texture coordinates.
    auto waterMat = mRenderItem->Mat;

    float& tu = waterMat->MatTransform(3, 0);
    float& tv = waterMat->MatTransform(3, 1);

    tu += 0.04f * dt;
    tv += 0.01f * dt;

    if (tu >= 1.0f) tu -= 1.0f;

    if (tv >= 1.0f) tv -= 1.0f;

    waterMat->MatTransform(3, 0) = tu;
    waterMat->MatTransform(3, 1) = tv;

    // Material has changed, so need to update cbuffer.
    waterMat->NumFramesDirty = GG::gNumFrameResources;
}
