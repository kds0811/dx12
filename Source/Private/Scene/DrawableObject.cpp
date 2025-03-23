#include "DrawableObject.h"
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
    auto objectName = GetName();
    if (!staticMesh)
    {
        LOG_WARNING(objectName, " SetStaticMesh func incoming argument is nullptr");
    }

    if (mMeshType == EMeshType::Static)
    {
        auto meshComp = dynamic_cast<StaticMeshComponent*>(mMeshComponent.get());
        if (meshComp)
        {
            meshComp->SetStaticMesh(staticMesh);
        }
        else
        {
            LOG_WARNING(objectName, " SetStaticMesh func - mMeshComponent is not StaticMeshComponent");
            assert(false);
            return;
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
        else
        {
            LOG_WARNING(objectName, " SetStaticMesh func - mMeshComponent is not StaticMeshComponent");
            assert(false);
            return;
        }
    }

    UpdateStaticGeoRenderData();
}

void DrawableObject::SetSkeletalMesh(SkeletalMesh* skeletalMesh)
{
    std::string objectName = GetName();

    if (!skeletalMesh)
    {
        LOG_WARNING(objectName, " SetSkeletalMesh func incoming argument is nullptr");
    }

    if (mMeshType == EMeshType::Skeletal)
    {
        auto meshComp = dynamic_cast<SkeletalMeshComponent*>(mMeshComponent.get());
        if (meshComp)
        {
            meshComp->SetSkeletalMesh(skeletalMesh);
        }
        else
        {
            LOG_WARNING(objectName, " SetSkeletalMesh func - mMeshComponent is not SkeletalMeshComponent");
            assert(false);
            return;
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
        else
        {
            LOG_WARNING(objectName, " SetSkeletalMesh func - mMeshComponent is not SkeletalMeshComponent");
            assert(false);
            return;
        }
    }

    UpdateSkeletalGeoRenderData();
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
        LOG_WARNING(GetName(), " material is nullptr");
        return;
    }

    mRenderData.MatRenderData.Type = material->GetMaterialType();
    mRenderData.MatRenderData.MatCBIndex = material->GetMaterialCBIndex();

    UpdataMatTexRenderData(MatRenderData::BaseColorTexIndex,    material->GetBaseColor());
    UpdataMatTexRenderData(MatRenderData::NormalMapTexIndex,    material->GetNormalMap());
    UpdataMatTexRenderData(MatRenderData::RoughnessMapTexIndex, material->GetRoughnesslMap());
    UpdataMatTexRenderData(MatRenderData::MetallicMapTexIndex,  material->GetMetalliclMap());
}

void DrawableObject::UpdataMatTexRenderData(int index, Texture* tex)
{
    assert(index >= 0 && index <= MatRenderData::TextureArraySize - 1);

    auto objectName = GetName();

    if (index < 0 || index > MatRenderData::TextureArraySize - 1)
    {
        LOG_WARNING("Invalid texture index ", index, " for Drawable Object ", objectName);
        return;
    }

    if (tex)
    {
        mRenderData.MatRenderData.TexStatus[index] = true;
        mRenderData.MatRenderData.SrvIndexes[index] = tex->GetSrvIndex();
        LOG_MESSAGE(" The texture of Drawable Object  ",  objectName, "  has been changed to ", tex->GetName(), " to slot ", index);
    }
    else
    {
        mRenderData.MatRenderData.TexStatus[index] = false;
        mRenderData.MatRenderData.SrvIndexes[index] = -1;
        LOG_MESSAGE(" The texture of Drawable Object  ", objectName, "  has been cleared for slot ", index);
    }
}

void DrawableObject::UpdateStaticGeoRenderData()
{
    std::string objectName = GetName();

    auto meshComp = dynamic_cast<StaticMeshComponent*>(mMeshComponent.get());
    if (!meshComp)
    {
        assert(false);
        LOG_WARNING(objectName, " StaticMeshComponent is nullpt or is not Static");
        return;
    }

    const auto staticMesh = meshComp->GetStaticMesh();
    if (!staticMesh)
    {
        assert(false);
        LOG_WARNING(objectName, " StaticMesh pointer is nullptr");
        return;
    }

    auto& geoRD = mRenderData.GeoRenderData;

    geoRD = staticMesh->GetGeoRenderData();
    LOG_MESSAGE(objectName, " geo render data updated");
}

void DrawableObject::UpdateSkeletalGeoRenderData() {}
