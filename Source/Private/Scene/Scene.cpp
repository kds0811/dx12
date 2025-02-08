#include "Scene.h"
#include "PrimitiveSceneObject.h"
#include "WavesSceneObject.h"
#include "ResourceManager.h"

#define USE_PIX
#include <pix3.h>


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
    mSceneObjects[8]->SetContiniusRotation(Rotator(10.0f, 10.0f, 10.0f));
    mSceneObjects[9]->SetContiniusRotation(Rotator(-10.0f, -10.0f, -10.0f));


    mSceneObjects[4]->SetTextureScale(Vector(4.0f, 4.0f, 4.0f));
    mSceneObjects[5]->SetTextureScale(Vector(4.0f, 4.0f, 4.0f));

    // set scale mounts
    mSceneObjects[7]->SetTextureScale(Vector(6.0f, 6.0f, 6.0f));
}

void Scene::Update()
{
    PIXScopedEvent(PIX_COLOR(60, 150, 60), L"Update  Scene");

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
        EPrimitiveType::MESH, Transform(Vector(0.0f, 5.f, 20.0f), Rotator::Zero(), Vector::One()), EMaterialType::SKULLMAT);
    //
    // add rotate boxes
    primitiveData.emplace_back(EPrimitiveType::BOX,
        Transform(Vector(10.0f, 5.0f, 20.0f), Rotator(0.0f, 0.0f, 90.0f), Vector(1.0f, 5.0f, 1.0f)), EMaterialType::STONE);
    primitiveData.emplace_back(EPrimitiveType::BOX,
        Transform(Vector(-10.0f, 5.0f, 20.0f), Rotator(0.0f, 0.0f, 90.0f), Vector(1.0f, 5.0f, 1.0f)), EMaterialType::STONE);

    // add static shapes
    primitiveData.emplace_back(
        EPrimitiveType::BOX, Transform(Vector(0.0f, 2.5f, 20.0f), Rotator::Zero(), Vector(1.0f, 3.0f, 1.0f)), EMaterialType::STONE);

    primitiveData.emplace_back(
        EPrimitiveType::GRID, Transform(Vector(0.0f, 0.0f, -25.0f), Rotator::Zero(), Vector(2.f, 1.0f, 2.f)), EMaterialType::TILE);
    primitiveData.emplace_back(
        EPrimitiveType::GRID, Transform(Vector(100.0f, 0.0f, -25.0f), Rotator::Zero(), Vector(2.f, 1.0f, 2.f)), EMaterialType::GRASSX);
    primitiveData.emplace_back(
        EPrimitiveType::GRID, Transform(Vector(-100.0f, 0.0f, -25.0f), Rotator::Zero(), Vector(2.f, 1.0f, 2.f)), EMaterialType::GRASSY);

    // add Mounts
    primitiveData.emplace_back(EPrimitiveType::LAND,
        Transform(Vector(0.0f, 0.0f, 105.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)), EMaterialType::GRASS);

    // textured boxes
    primitiveData.emplace_back(
        EPrimitiveType::BOX, Transform(Vector(-20.0f, 5.f, -20.0f), Rotator::Zero(), Vector(5.0f, 5.0f, 5.0f)), EMaterialType::FIREBALL);
    primitiveData.emplace_back(
        EPrimitiveType::BOX, Transform(Vector(20.0f, 5.f, -20.0f), Rotator::Zero(), Vector(5.0f, 5.0f, 5.0f)), EMaterialType::COLDFIRE);

    // add colums and spheres
    for (int i = 0; i < 5; ++i)
    {
        primitiveData.emplace_back(EPrimitiveType::CYLINDER,
            Transform(Vector(-20.0f, 5.0f, i * 7.0f), Rotator::Zero(), Vector(1.0f, 4.0f, 1.0f)), EMaterialType::STONE);
        primitiveData.emplace_back(EPrimitiveType::CYLINDER,
            Transform(Vector(20.0f, 5.0f, i * 7.0f), Rotator::Zero(), Vector(1.0f, 4.0f, 1.0f)), EMaterialType::STONE);
        primitiveData.emplace_back(EPrimitiveType::SPHERE,
            Transform(Vector(-20.0f, 13.0f, i * 7.0f), Rotator::Zero(), Vector(2.0f, 2.0f, 2.0f)), EMaterialType::BRICKS);
        primitiveData.emplace_back(EPrimitiveType::SPHERE,
            Transform(Vector(20.0f, 13.0f, i * 7.0f), Rotator::Zero(), Vector(2.0f, 2.0f, 2.0f)), EMaterialType::BRICKS);
    }

    // Add Waves
    primitiveData.emplace_back(EPrimitiveType::WAVES,
        Transform(Vector(0.0f, 0.0f, 105.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)), EMaterialType::WATER);

    for (const auto& prim : primitiveData)
    {
        if (prim.ObjectType == EPrimitiveType::WAVES)
        {
            mSceneObjects.emplace_back(std::make_unique<WavesSceneObject>(prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter,
                pResourceManager->GetGeometries(), prim.MaterialType, pResourceManager->GetMaterials()));
            ++SceneObjectsCounter;

            pWavesObject = dynamic_cast<WavesSceneObject*>(mSceneObjects.back().get());
            assert(pWavesObject);
            continue;
        }

        mSceneObjects.emplace_back(std::make_unique<PrimitiveSceneObject>(prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter,
            pResourceManager->GetGeometries(), prim.MaterialType, pResourceManager->GetMaterials()));
        ++SceneObjectsCounter;
    }

    mSceneRenderItems.reserve(SceneObjectsCounter);
    for (const auto& obj : mSceneObjects)
    {
        mSceneRenderItems.push_back(obj->GetRenderItem());
    }
}
