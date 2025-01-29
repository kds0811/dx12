#include "Scene.h"
#include "Graphic.h"
#include "PrimitiveSceneObject.h"
#include "WavesSceneObject.h"

Scene::Scene(GameTimerW& timer, Graphic* pgfx) : mTimer(timer), pGfx(pgfx) {}

void Scene::InitScene()
{
    BuildScenePrimitives();
    BuildWaves();
    mSceneObjects[0]->SetContiniusRotation(Rotator(0.0f, 5.0f, 0.0f));
    mSceneObjects[1]->SetContiniusRotation(Rotator(0.0f, 0.0f, 10.0f));
    mSceneObjects[2]->SetContiniusRotation(Rotator(0.0f, 0.0f, -10.0f));
}

void Scene::Update()
{
    for (const auto& object : mSceneObjects)
    {
        object->Update(mTimer.GetDeltaTime());
    }
}

void Scene::BuildScenePrimitives()
{
    std::vector<DataPrimitiveBuild> primitiveData{};
    // add skull
    primitiveData.emplace_back(ePrimitiveType::MESH, Transform(Vector(0.0f, 5.f, 20.0f), Rotator::Zero(), Vector::One()));
    // add rotate boxes
    primitiveData.emplace_back(
        ePrimitiveType::BOX, Transform(Vector(20.0f, 5.0f, 20.0f), Rotator(0.0f, 0.0f, 90.0f), Vector(1.0f, 2.0f, 1.0f)));
    primitiveData.emplace_back(
        ePrimitiveType::BOX, Transform(Vector(-20.0f, 5.0f, 20.0f), Rotator(0.0f, 0.0f, 90.0f), Vector(1.0f, 2.0f, 1.0f)));
    
    // add static shapes
    primitiveData.emplace_back(ePrimitiveType::BOX, Transform(Vector(0.0f, 2.5f, 20.0f), Rotator::Zero(), Vector(1.0f, 1.0f, 1.0f)));
    primitiveData.emplace_back(ePrimitiveType::GRID, Transform(Vector::Zero(), Rotator::Zero(), Vector::One()));
   
    primitiveData.emplace_back(
        ePrimitiveType::LAND, Transform(Vector(0.0f, 0.0f, 105.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)));

    // add colums and spheres
    for (int i = 0; i < 5; ++i)
    {
        primitiveData.emplace_back(ePrimitiveType::CYLINDER, Transform(Vector(-5.0f, 1.5f, i * 5.0f), Rotator::Zero(), Vector::One()));
        primitiveData.emplace_back(ePrimitiveType::CYLINDER, Transform(Vector(5.0f, 1.5f, i * 5.0f),  Rotator::Zero(), Vector::One()));
        primitiveData.emplace_back(ePrimitiveType::SPHERE,   Transform(Vector(-5.0f, 3.5f, i * 5.0f), Rotator::Zero(), Vector::One()));
        primitiveData.emplace_back(ePrimitiveType::SPHERE,   Transform(Vector(5.0f, 3.5f, i * 5.0f),  Rotator::Zero(), Vector::One()));
    }

    

    for (const auto& prim : primitiveData)
    {
        mSceneObjects.emplace_back(
            std::make_unique<PrimitiveSceneObject>(prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter, pGfx->GetGeometries()));
        ++SceneObjectsCounter;
    }

    mSceneRenderItems.reserve(SceneObjectsCounter);
    for (const auto& obj : mSceneObjects)
    {
        mSceneRenderItems.push_back(obj->GetRenderItem());
    }
}

void Scene::BuildWaves() 
{
    

}
