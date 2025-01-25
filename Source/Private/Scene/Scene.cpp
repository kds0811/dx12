#include "Scene.h"
#include "Graphic.h"
#include "PrimitiveSceneObject.h"

Scene::Scene(GameTimerW& timer, Graphic* pgfx) : mTimer(timer), pGfx(pgfx) 
{
    
}

void Scene::InitScene()
{
    //BuildStandartShapeGeometry();
    BuildScenePrimitives();
}

void Scene::Update()
{
    for (const auto& object : mSceneObjects)
    {
        object->Update(mTimer.GetDeltaTime());
    }
}


void Scene::BuildStandartShapeGeometry()
{
    
}

void Scene::BuildScenePrimitives() 
{
    std::vector<DataPrimitiveBuild> primitiveData{};
    primitiveData.emplace_back(ePrimitiveType::BOX, Transform(Vector(0.0f, 5.f, 0.0f), Rotator::Zero(), Vector(2.0f, 2.0f, 2.0f)));
    primitiveData.emplace_back(ePrimitiveType::GRID, Transform(Vector::Zero(), Rotator::Zero(), Vector::One()));

    for (int i = 0; i < 5; ++i)
    {
        primitiveData.emplace_back(ePrimitiveType::CYLINDER, Transform(Vector(-5.0f, 1.5f, i * 5.0f), Rotator::Zero(), Vector::One()));
        primitiveData.emplace_back(ePrimitiveType::CYLINDER, Transform(Vector(5.0f, 1.5f, i * 5.0f), Rotator::Zero(), Vector::One()));
        primitiveData.emplace_back(ePrimitiveType::SPHERE, Transform(Vector(-5.0f, 3.5f, i * 5.0f), Rotator::Zero(), Vector::One()));
        primitiveData.emplace_back(ePrimitiveType::SPHERE, Transform(Vector(5.0f, 3.5f, i * 5.0f), Rotator::Zero(), Vector::One()));
    }

    for (const auto& prim : primitiveData)
    {
        mSceneObjects.emplace_back(std::make_unique<PrimitiveSceneObject>(prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter, pGfx->GetGeometries()));
        ++SceneObjectsCounter;
    }

    mSceneRenderItems.reserve(SceneObjectsCounter);
    for (const auto& obj : mSceneObjects)
    {
        mSceneRenderItems.push_back(obj->GetRenderItem());
    }
}


