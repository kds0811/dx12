#include "DrawableObject.h"

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
