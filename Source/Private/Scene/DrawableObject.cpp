#include "DrawableObject.h"
#include "Logger.h"
#include "Texture.h"
#include <cassert>


DrawableObject::DrawableObject(int id, std::string name) : SceneObject(id, name)
{
    mSceneComponent = std::make_unique<SceneComponent>(this);
}

DrawableObject::DrawableObject(int id, std::string name, EMeshType type) : SceneObject(id, name)
{
    mSceneComponent = std::make_unique<SceneComponent>(this);

    if (type == EMeshType::Static)
    {
        InitAsStaticMesh();
    }

    if (type == EMeshType::Skeletal)
    {
        InitAsSkeletalMesh();
    }
}

void DrawableObject::SetStaticMesh(StaticMesh* staticMesh)
{
    if (mMeshType == EMeshType::Static)
    {
        auto meshComp = dynamic_cast<StaticMeshComponent*>(mMeshComponent.get());
        if (meshComp)
        {
            meshComp->SetStaticMesh(staticMesh);
        }
    }
    else
    {
        InitAsStaticMesh();
        auto meshComp = dynamic_cast<StaticMeshComponent*>(mMeshComponent.get());
        if (meshComp)
        {
            meshComp->SetStaticMesh(staticMesh);
        }
    }
}

void DrawableObject::SetSkeletalMesh(SkeletalMesh* skeletalMesh)
{
    if (mMeshType == EMeshType::Skeletal)
    {
        auto meshComp = dynamic_cast<SkeletalMeshComponent*>(mMeshComponent.get());
        if (meshComp)
        {
            meshComp->SetSkeletalMesh(skeletalMesh);
        }
    }
    else
    {
        InitAsSkeletalMesh();
        auto meshComp = dynamic_cast<SkeletalMeshComponent*>(mMeshComponent.get());
        if (meshComp)
        {
            meshComp->SetSkeletalMesh(skeletalMesh);
        }
    }
}

void DrawableObject::SetMaterial(Material* material)
{
    mMeshComponent->SetMaterial(material);
    UpdateMaterialRenderData();
}

void DrawableObject::InitAsStaticMesh()
{
    mMeshType = EMeshType::Static;
    mMeshComponent = std::make_unique<StaticMeshComponent>(this);
}

void DrawableObject::InitAsSkeletalMesh()
{
    mMeshType = EMeshType::Skeletal;
    mMeshComponent = std::make_unique<SkeletalMeshComponent>(this);
}

void DrawableObject::UpdateMaterialRenderData()
{
    auto material = mMeshComponent->GetMaterial();
    if (!material)
    {
        Log::LogWarning("material is nullptr");
        return;
    }

    mRenderData.MatRenderData.Type = material->GetMaterialType();
    mRenderData.MatRenderData.MatCBIndex = material->GetMaterialCBIndex();

    UpdataMatTexRenderData(MatRenderData::BaseColorTexIndex, mMeshComponent->GetMaterial()->GetBaseColor());
    UpdataMatTexRenderData(MatRenderData::NormalMapTexIndex, mMeshComponent->GetMaterial()->GetNormalMap());
    UpdataMatTexRenderData(MatRenderData::RoughnessMapTexIndex, mMeshComponent->GetMaterial()->GetRoughnesslMap());
    UpdataMatTexRenderData(MatRenderData::MetallicMapTexIndex, mMeshComponent->GetMaterial()->GetMetalliclMap());
}



void DrawableObject::UpdataMatTexRenderData(int index, Texture* tex) 
{
    assert(index >= 0 && index <= MatRenderData::TextureArraySize - 1);
 
    std::string objectName = mName.empty() ? "Unnamed Object" : mName;

    if (index < 0 || index > MatRenderData::TextureArraySize - 1)
    {
        Log::LogWarning("Invalid texture index " + std::to_string(index) + " for Drawable Object " + objectName);
        return;
    }

    if (tex)
    {
        mRenderData.MatRenderData.TexStatus[index] = true;
        mRenderData.MatRenderData.SrvIndexes[index] = tex->GetSrvIndex();
        Log::LogMessage(" The texture of Drawable Object  " + objectName + "  has been changed to " + tex->GetName() + " to slot " + std::to_string(index));
    }
    else
    {
        mRenderData.MatRenderData.TexStatus[index] = false;
        mRenderData.MatRenderData.SrvIndexes[index] = -1;
        Log::LogMessage(" The texture of Drawable Object  " + objectName + "  has been cleared for slot " + std::to_string(index));
    }
}

void DrawableObject::UpdateGeoRenderData() {}