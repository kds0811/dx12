#pragma once
#include <vector>
#include <memory>
#include "BaseSceneObject.h"
#include "GameTimerW.h"
#include "ShapeGeometryBuilder.h"
#include "D3D12Utils.h"
#include "WavesSceneObject.h"

class ResourceManager;

struct DataPrimitiveBuild
{
    DataPrimitiveBuild(EPrimitiveType objectType, Transform objectTransformation, EMaterialType materialType)
        : ObjectType(objectType), ObjectTransformation(objectTransformation), MaterialType(materialType)
    {}
    DataPrimitiveBuild() = delete;

    EPrimitiveType ObjectType;
    Transform ObjectTransformation;
    EMaterialType MaterialType;
};


class Scene
{
    std::vector<std::unique_ptr<BaseSceneObject>> mSceneObjects;
    WavesSceneObject* pWavesObject = nullptr;
    GameTimerW* pTimer = nullptr;
    ResourceManager* pResourceManager = nullptr;
    
    std::vector<RenderItem*> mSceneRenderItems;

public:
    Scene(GameTimerW* timer, ResourceManager* resourceManager);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(const Scene&&) = delete;
    Scene& operator=(const Scene&&) = delete;

    void InitScene();
    void Update();
    std::vector<RenderItem*>& GetSceneRenderItems() { return mSceneRenderItems; }
    std::vector<std::unique_ptr<BaseSceneObject>>& GetSceneObjects() { return mSceneObjects; }
    WavesSceneObject* GetWavesPtr() { return pWavesObject; }
   
private:
    void BuildScenePrimitives();
    int SceneObjectsCounter = 0;
};