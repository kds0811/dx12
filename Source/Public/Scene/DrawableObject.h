#pragma once
#include "SceneObject.h"
#include "SceneComponent.h"
#include "memory"
#include "StaticMeshComponent.h"
#include "SkeletalMeshComponent.h"
#include "RenderData.h"


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
    DrawableObject(int id, std::string name, EMeshType type);

private:
    void InitAsStaticMesh();
    void InitAsSkeletalMesh();
};