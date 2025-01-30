#pragma once
#include <vector>
#include <memory>
#include "BaseSceneObject.h"
#include "GameTimerW.h"
#include "ShapeGeometryBuilder.h"
#include "D3D12Utils.h"

class Graphic;


struct DataPrimitiveBuild
{
    DataPrimitiveBuild(ePrimitiveType objectType, Transform objectTransformation)
        : ObjectType(objectType), ObjectTransformation(objectTransformation)
    {}
    ePrimitiveType ObjectType;
    Transform ObjectTransformation;
};


class Scene
{
    std::vector<std::unique_ptr<BaseSceneObject>> mSceneObjects;
    GameTimerW* mTimer = nullptr;
    Graphic* pGfx = nullptr;
    
    std::vector<RenderItem*> mSceneRenderItems;

public:
    Scene(GameTimerW* timer, Graphic* pgfx);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(const Scene&&) = delete;
    Scene& operator=(const Scene&&) = delete;

    void InitScene();
    void Update();
    std::vector<RenderItem*>& GetSceneRenderItems() { return mSceneRenderItems; }
    std::vector<std::unique_ptr<BaseSceneObject>>& GetSceneObjects() { return mSceneObjects; }

private:
    void BuildScenePrimitives();
    void BuildWaves();
    int SceneObjectsCounter = 0;
};