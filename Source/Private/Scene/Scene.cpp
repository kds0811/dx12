#include "Scene.h"
#include "PrimitiveSceneObject.h"
#include "WavesSceneObject.h"
#include "ResourceManager.h"

Scene::Scene(GameTimerW* timer, ResourceManager* resourceManager)
{
    assert(timer);
    assert(resourceManager);
    pTimer = timer;
    pResourceManager = resourceManager;
}

void Scene::InitScene()
{
    BuildScenePrimitives();

    mSceneObjects[0]->SetContiniusRotation(Rotator(0.0f, 5.0f, 0.0f));
    mSceneObjects[1]->SetContiniusRotation(Rotator(0.0f, 0.0f, 10.0f));
    mSceneObjects[2]->SetContiniusRotation(Rotator(0.0f, 0.0f, -10.0f));
}

void Scene::Update()
{
    for (const auto& object : mSceneObjects)
    {
        object->Update(pTimer->GetDeltaTime());
    }
}

void Scene::BuildScenePrimitives()
{
    std::vector<DataPrimitiveBuild> primitiveData{};
    // add skull
    primitiveData.emplace_back(
        ePrimitiveType::MESH, Transform(Vector(0.0f, 5.f, 20.0f), Rotator::Zero(), Vector::One()), EMaterialType::SKULLMAT);
    // add rotate boxes
    primitiveData.emplace_back(ePrimitiveType::BOX,
        Transform(Vector(20.0f, 5.0f, 20.0f), Rotator(0.0f, 0.0f, 90.0f), Vector(1.0f, 2.0f, 1.0f)), EMaterialType::STONE);
    primitiveData.emplace_back(ePrimitiveType::BOX,
        Transform(Vector(-20.0f, 5.0f, 20.0f), Rotator(0.0f, 0.0f, 90.0f), Vector(1.0f, 2.0f, 1.0f)), EMaterialType::STONE);

    // add static shapes
    primitiveData.emplace_back(
        ePrimitiveType::BOX, Transform(Vector(0.0f, 2.5f, 20.0f), Rotator::Zero(), Vector(1.0f, 1.0f, 1.0f)),  EMaterialType::STONE);
    primitiveData.emplace_back(ePrimitiveType::GRID, Transform(Vector::Zero(), Rotator::Zero(), Vector::One()), EMaterialType::TILE);

    // add Mounts
    primitiveData.emplace_back(ePrimitiveType::LAND,
        Transform(Vector(0.0f, 0.0f, 105.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)), EMaterialType::GRASS);

    // add colums and spheres
    for (int i = 0; i < 5; ++i)
    {
        primitiveData.emplace_back(ePrimitiveType::CYLINDER, Transform(Vector(-5.0f, 1.5f, i * 5.0f), Rotator::Zero(), Vector::One()), EMaterialType::STONE);
        primitiveData.emplace_back(
            ePrimitiveType::CYLINDER, Transform(Vector(5.0f, 1.5f, i * 5.0f), Rotator::Zero(), Vector::One()), EMaterialType::STONE);
        primitiveData.emplace_back(ePrimitiveType::SPHERE, Transform(Vector(-5.0f, 3.5f, i * 5.0f), Rotator::Zero(), Vector::One()), EMaterialType::BRICKS);
        primitiveData.emplace_back(
            ePrimitiveType::SPHERE, Transform(Vector(5.0f, 3.5f, i * 5.0f), Rotator::Zero(), Vector::One()), EMaterialType::BRICKS);
    }

    // Add Waves
    primitiveData.emplace_back(
        ePrimitiveType::WAVES, Transform(Vector(0.0f, 0.0f, 105.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)), EMaterialType::WATER);

    for (const auto& prim : primitiveData)
    {
        if (prim.ObjectType == ePrimitiveType::WAVES)
        {
            mSceneObjects.emplace_back(std::make_unique<WavesSceneObject>(
                prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter, pResourceManager->GetGeometries()));
            ++SceneObjectsCounter;

            pWavesObject = dynamic_cast<WavesSceneObject*>(mSceneObjects.back().get());
            assert(pWavesObject);
            continue;
        }

        mSceneObjects.emplace_back(std::make_unique<PrimitiveSceneObject>(
            prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter, pResourceManager->GetGeometries()));
        ++SceneObjectsCounter;
    }

    mSceneRenderItems.reserve(SceneObjectsCounter);
    for (const auto& obj : mSceneObjects)
    {
        mSceneRenderItems.push_back(obj->GetRenderItem());
    }
}
