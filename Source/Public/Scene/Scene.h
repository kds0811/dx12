#pragma once
#include <vector>
#include <memory>
#include "BaseSceneObject.h"
#include "GameTimerW.h"
#include "D3D12Utils.h"
#include "WavesSceneObject.h"
#include "Material.h"

class ResourceManager;

struct SortedSceneObjects
{
    std::vector<BaseSceneObject*> OpaqueObjects;
    std::vector<BaseSceneObject*> TransparentObjects;
    std::vector<BaseSceneObject*> AlphaTestObjects;
    std::vector<BaseSceneObject*> MirrorObjects;
    std::vector<BaseSceneObject*> Models;
    std::vector<BaseSceneObject*> GenerateTrees;
    std::vector<BaseSceneObject*> GeometrySubdivide;
    std::vector<BaseSceneObject*> TesselationInteger;
    std::vector<BaseSceneObject*> TesselationFracOdd;
    std::vector<BaseSceneObject*> TesselationFracEven;
    std::vector<BaseSceneObject*> TesselationPow2;
    std::vector<BaseSceneObject*> SkyBox;
};

struct DataPrimitiveBuild
{
    DataPrimitiveBuild(EPrimitiveType objectType, Transform objectTransformation, EMaterialType materialType, ERenderLayer renderLayer)
        :
        ObjectType(objectType),
        ObjectTransformation(objectTransformation),
        MaterialType(materialType),
        RenderLayer(renderLayer)
    {}
    DataPrimitiveBuild() = delete;

    EPrimitiveType ObjectType;
    Transform ObjectTransformation;
    EMaterialType MaterialType;
    ERenderLayer RenderLayer;
};


class Scene
{
    std::vector<std::unique_ptr<BaseSceneObject>> mSceneObjects;
    WavesSceneObject* pWavesObject = nullptr;
    GameTimerW* pTimer = nullptr;
    ResourceManager* pResourceManager = nullptr;

    SortedSceneObjects mSortedSceneObjects;

public:
    Scene(GameTimerW* timer, ResourceManager* resourceManager);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(const Scene&&) = delete;
    Scene& operator=(const Scene&&) = delete;

    void InitScene();
    void Update();
    std::vector<std::unique_ptr<BaseSceneObject>>& GetSceneObjects() { return mSceneObjects; }
    WavesSceneObject* GetWavesPtr() { return pWavesObject; }
    SortedSceneObjects& GetSortedSceneObjects() { return mSortedSceneObjects; }
    UINT GetSceneObjectsCounter() { return SceneObjectsCounter; }
   
private:
    void BuildScenePrimitives();
    UINT SceneObjectsCounter = 0;
};