#pragma once
#include "SceneObject.h"
#include "SceneComponent.h"
#include "memory"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "RenderData.h"
#include "Logger.h"

enum class EMeshType
{
    None,
    Static,
    Skeletal
};

class DrawableObject : public SceneObject
{
    std::unique_ptr<SceneComponent> mSceneComponent = nullptr;
    std::unique_ptr<MeshComponent> mMeshComponent = nullptr;
    EMeshType mMeshType = EMeshType::None;
    RenderData mRenderData;

public:
    DrawableObject(int id, std::string name);
    DrawableObject(int id, std::string name, EMeshType type);

    //void SetStaticMesh(StaticMesh* staticMesh);
    //void SetSkeletalMesh(SkeletalMesh* skeletalMesh);

    template <typename T>
    void SetMesh(T* mesh, EMeshType type);

    void SetMaterial(Material* material);

private:
    void InitAsStaticMesh();
    void InitAsSkeletalMesh();
    void UpdateMaterialRenderData();
    void UpdataMatTexRenderData(int index, Texture* tex);

    void UpdateStaticGeoRenderData();
    void UpdateSkeletalGeoRenderData();
};



template <typename T>
inline void DrawableObject::SetMesh(T* mesh, EMeshType type)
{
    if (!mesh)
    {
        LOG_WARNING(GetName(), " Set", typeName, " func incoming argument is nullptr");
        return;
    }

    if (mMeshType != type)
    {
        if (type == EMeshType::Static)
        {
            InitAsStaticMesh();
        }
        else if (type == EMeshType::Skeletal)
        {
            InitAsSkeletalMesh();
        }
    }

    auto meshComp = static_cast<T*>(mMeshComponent.get());
    meshComp->SetMesh(mesh);

    if (type == EMeshType::Static)
    {
        UpdateStaticGeoRenderData();
    }
    else if (type == EMeshType::Skeletal)
    {
        UpdateSkeletalGeoRenderData();
    }
}
