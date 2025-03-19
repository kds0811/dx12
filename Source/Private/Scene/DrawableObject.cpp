#include "DrawableObject.h"

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
