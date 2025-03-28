#include "Scene.h"
#include "PrimitiveSceneObject.h"
#include "WavesSceneObject.h"
#include "ResourceManager.h"
#include "TreesSceneObject.h"


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
    mSceneObjects[1]->SetContiniusRotation(Rotator(0.0f, 0.0f, 20.0f));
    mSceneObjects[2]->SetContiniusRotation(Rotator(0.0f, 0.0f, -20.0f));
    mSceneObjects[8]->SetContiniusRotation(Rotator(10.0f, 10.0f, 10.0f));
    mSceneObjects[9]->SetContiniusRotation(Rotator(-10.0f, -10.0f, -10.0f));

    mSceneObjects[4]->SetTextureScale(Vector(4.0f, 4.0f, 4.0f));
    mSceneObjects[5]->SetTextureScale(Vector(4.0f, 4.0f, 4.0f));

    // set scale mounts
    mSceneObjects[7]->SetTextureScale(Vector(6.0f, 6.0f, 6.0f));

    mSceneObjects[10]->SetTextureScale(Vector(6.0f, 6.0f, 6.0f));

    // set waves texture scale
    mSceneObjects[12]->SetTextureScale(Vector(10.0f, 10.0f, 10.0f));
    // set Three land and tesselation land texture scale
    mSceneObjects[13]->SetTextureScale(Vector(10.0f, 10.0f, 10.0f));
    mSceneObjects[14]->SetTextureScale(Vector(10.0f, 10.0f, 10.0f));



    // set briks wall texture scale
    mSceneObjects[24]->SetTextureScale(Vector(3.0f, 3.0f, 3.0f));
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
    primitiveData.emplace_back(EPrimitiveType::MESH, Transform(Vector(0.0f, 5.f, 0.0f), Rotator::Zero(), Vector::One()), EMaterialType::SKULLMAT, ERenderLayer::Opaque);

    // add rotate spinners
    primitiveData.emplace_back(
        EPrimitiveType::SPINER, Transform(Vector(50.0f, 10.0f, 0.0f), Rotator(0.0f, 180.0f, 90.0f), Vector(2.0f, 2.0f, 2.0f)), EMaterialType::COLDFIRE, ERenderLayer::Opaque);
    primitiveData.emplace_back(
        EPrimitiveType::SPINER, Transform(Vector(-50.0f, 10.0f, 0.0f), Rotator(0.0f, 180.0f, 90.0f), Vector(2.0f, 2.0f, 2.0f)), EMaterialType::FIREBALL, ERenderLayer::Opaque);

    // add static shapes
    primitiveData.emplace_back(EPrimitiveType::BOX, Transform(Vector(0.0f, 5.0f, 20.0f), Rotator::Zero(), Vector(1.0f, 5.0f, 1.0f)), EMaterialType::STONE, ERenderLayer::Opaque);

    primitiveData.emplace_back(EPrimitiveType::GRID, Transform(Vector(0.0f, 0.0f, -25.0f), Rotator::Zero(), Vector(2.f, 1.0f, 2.f)), EMaterialType::TILE, ERenderLayer::Opaque);
    primitiveData.emplace_back(EPrimitiveType::GRID, Transform(Vector(100.0f, 0.0f, -25.0f), Rotator::Zero(), Vector(2.f, 1.0f, 2.f)), EMaterialType::GRASSX, ERenderLayer::Opaque);
    primitiveData.emplace_back(
        EPrimitiveType::GRID, Transform(Vector(-100.0f, 0.0f, -25.0f), Rotator::Zero(), Vector(2.f, 1.0f, 2.f)), EMaterialType::GRASSY, ERenderLayer::Opaque);

    // add Mounts
    primitiveData.emplace_back(
        EPrimitiveType::LAND, Transform(Vector(0.0f, 0.0f, 105.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)), EMaterialType::GRASSX, ERenderLayer::Opaque);

    // textured boxes
    primitiveData.emplace_back(
        EPrimitiveType::BOX, Transform(Vector(-20.0f, 10.f, 10.0f), Rotator::Zero(), Vector(5.0f, 5.0f, 5.0f)), EMaterialType::FIREBALL, ERenderLayer::Opaque);
    primitiveData.emplace_back(
        EPrimitiveType::BOX, Transform(Vector(20.0f, 10.f, 10.0f), Rotator::Zero(), Vector(5.0f, 5.0f, 5.0f)), EMaterialType::COLDFIRE, ERenderLayer::Opaque);

    // Drone
    primitiveData.emplace_back(
        EPrimitiveType::DRONE, Transform(Vector(60.0f, 0.0f, -40.0f), Rotator(0.0f, 0.0f, 180.0f), Vector(10.f, 10.f, 10.f)), EMaterialType::METALL, ERenderLayer::Opaque);

    // Drone2
    primitiveData.emplace_back(
        EPrimitiveType::DRONE, Transform(Vector(100.0f, 0.0f, -40.0f), Rotator(0.0f, 0.0f, 180.0f), Vector(15.f, 15.f, 15.f)), EMaterialType::DRONEBASECOLOR, ERenderLayer::Opaque);

    // Add Waves
    primitiveData.emplace_back(
        EPrimitiveType::WAVES, Transform(Vector(0.0f, 0.0f, 105.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)), EMaterialType::WATER, ERenderLayer::Transparent);

    // Trees Land
    primitiveData.emplace_back(
        EPrimitiveType::GRID, Transform(Vector(-205.0f, 0.0f, 150.0f), Rotator::Zero(), Vector(5.f, 1.0f, 5.f)), EMaterialType::GRASSX, ERenderLayer::Opaque);

    // Tesselation demo Land
    primitiveData.emplace_back(EPrimitiveType::GRID, Transform(Vector(205.0f, 0.0f, 150.0f), Rotator::Zero(), Vector(5.f, 1.0f, 5.f)), EMaterialType::GRASSX, ERenderLayer::Opaque);

    // Trees
    primitiveData.emplace_back(
        EPrimitiveType::TREES, Transform(Vector(-205.0f, 5.5f, 150.0f), Rotator::Zero(), Vector(1.f, 1.0f, 1.f)), EMaterialType::TREES, ERenderLayer::GenerateTrees);

    // MIRROR
    primitiveData.emplace_back(
        EPrimitiveType::GRID, Transform(Vector(0.0f, 17.5f, -75.0f), Rotator(90.0f, 0.0f, 0.0f), Vector(2.f, 1.0f, 0.7f)), EMaterialType::MIRROR, ERenderLayer::Mirror);

    // GEOSPHERE
    primitiveData.emplace_back(
        EPrimitiveType::GEOSPHERE, Transform(Vector(-120.0f, 17.5f, -30.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(5.f, 5.0f, 5.f)), EMaterialType::WHITE, ERenderLayer::Opaque);

    primitiveData.emplace_back(EPrimitiveType::GEOSPHERE, Transform(Vector(-120.0f, 17.5f, -20.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(5.f, 5.0f, 5.f)), EMaterialType::WHITE,
        ERenderLayer::GeometrySubdivide);

    // Tesselation DEMO
    primitiveData.emplace_back(EPrimitiveType::GEOSPHERE, Transform(Vector(150.0f, 17.5f, 50.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(12.f, 12.0f, 12.f)), EMaterialType::WHITE,
        ERenderLayer::TesselationInteger);
    primitiveData.emplace_back(EPrimitiveType::GEOSPHERE, Transform(Vector(180.0f, 17.5f, 50.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(12.f, 12.0f, 12.f)), EMaterialType::WHITE,
        ERenderLayer::TesselationFracOdd);
    primitiveData.emplace_back(EPrimitiveType::GEOSPHERE, Transform(Vector(210.0f, 17.5f, 50.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(12.f, 12.0f, 12.f)), EMaterialType::WHITE,
        ERenderLayer::TesselationFracEven);
    primitiveData.emplace_back(EPrimitiveType::GEOSPHERE, Transform(Vector(240.0f, 17.5f, 50.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(12.f, 12.0f, 12.f)), EMaterialType::WHITE,
        ERenderLayer::TesselationPow2);

    // SKY 
    primitiveData.emplace_back(
        EPrimitiveType::SPHERE, Transform(Vector(0.0f, 0.0f, 0.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(5000.f, 5000.0f, 5000.f)), EMaterialType::SKYBOX, ERenderLayer::Skybox);

    // brick wall
    primitiveData.emplace_back(
        EPrimitiveType::GRID, Transform(Vector(-100.0f, 17.5f, -75.0f), Rotator(90.0f, 0.0f, 0.0f), Vector(2.f, 1.0f, 0.7f)), EMaterialType::BRICKS, ERenderLayer::Opaque);


    // add colums and spheres
    for (int i = 0; i < 5; ++i)
    {
        primitiveData.emplace_back(
            EPrimitiveType::CYLINDER, Transform(Vector(-20.0f, 6.0f, i * -10.0f), Rotator::Zero(), Vector(1.0f, 4.0f, 1.0f)), EMaterialType::BRICKS, ERenderLayer::Opaque);
        primitiveData.emplace_back(
            EPrimitiveType::CYLINDER, Transform(Vector(20.0f, 6.0f, i * -10.0f), Rotator::Zero(), Vector(1.0f, 4.0f, 1.0f)), EMaterialType::BRICKS, ERenderLayer::Opaque);
        primitiveData.emplace_back(
            EPrimitiveType::SPHERE, Transform(Vector(-20.0f, 14.0f, i * -10.0f), Rotator::Zero(), Vector(2.0f, 2.0f, 2.0f)), EMaterialType::STONE, ERenderLayer::Opaque);
        primitiveData.emplace_back(
            EPrimitiveType::SPHERE, Transform(Vector(20.0f, 14.0f, i * -10.0f), Rotator::Zero(), Vector(2.0f, 2.0f, 2.0f)), EMaterialType::STONE, ERenderLayer::Opaque);
    }

    // Model
    primitiveData.emplace_back(
        EPrimitiveType::MODEL, Transform(Vector(0.0f, 10.0f, -40.0f), Rotator(-90.0f, 0.0f, 0.0f), Vector(1.0f, 1.0f, 1.0f)), EMaterialType::COLDFIRE, ERenderLayer::Opaque);

    // MODELS spiders
    primitiveData.emplace_back(
        EPrimitiveType::MODELSPIDERFBX, Transform(Vector(-60.0f, 4.0f, -40.0f), Rotator(0.0f, 90.0f, 0.0f), Vector(0.1f, 0.1f, 0.1f)), EMaterialType::GRASS, ERenderLayer::Opaque);
    primitiveData.emplace_back(
        EPrimitiveType::MODELSPIDEROBJ, Transform(Vector(-90.0f, 4.0f, -40.0f), Rotator(0.0f, 90.0f, 0.0f), Vector(0.1f, 0.1f, 0.1f)), EMaterialType::STONE, ERenderLayer::Opaque);

    // HEUSITOS
    primitiveData.emplace_back(
        EPrimitiveType::HUESITOS, Transform(Vector(-50.0f, 4.0f, -40.0f), Rotator(0.0f, 90.0f, 0.0f), Vector(1.f, 1.f, 1.f)), EMaterialType::WATER, ERenderLayer::Opaque);

    // Wire Fence
    primitiveData.emplace_back(
        EPrimitiveType::BOX, Transform(Vector(-45.0f, 1.0f, 45.0f), Rotator(0.0f, 0.0f, 0.0f), Vector(10.f, 10.f, 10.f)), EMaterialType::WIREFENCE, ERenderLayer::AlphaTested);

    for (const auto& prim : primitiveData)
    {
        if (prim.ObjectType == EPrimitiveType::WAVES)
        {
            mSceneObjects.emplace_back(std::make_unique<WavesSceneObject>(prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter, pResourceManager->GetGeometries(),
                prim.MaterialType, pResourceManager->GetMaterials(), prim.RenderLayer));
            SceneObjectsCounter += 3;

            pWavesObject = static_cast<WavesSceneObject*>(mSceneObjects.back().get());
            assert(pWavesObject);
            continue;
        }

        if (prim.ObjectType == EPrimitiveType::TREES)
        {
            mSceneObjects.emplace_back(std::make_unique<TreesSceneObject>(prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter, pResourceManager->GetGeometries(),
                prim.MaterialType, pResourceManager->GetMaterials(), prim.RenderLayer));
            SceneObjectsCounter += 3;
            continue;
        }

        mSceneObjects.emplace_back(std::make_unique<PrimitiveSceneObject>(prim.ObjectType, prim.ObjectTransformation, SceneObjectsCounter, pResourceManager->GetGeometries(),
            prim.MaterialType, pResourceManager->GetMaterials(), prim.RenderLayer));
        SceneObjectsCounter += 3;
    }

    // sort object by render layer
    for (const auto& obj : mSceneObjects)
    {
        if (obj->GetRenderLayer() == ERenderLayer::Opaque)
        {
            mSortedSceneObjects.OpaqueObjects.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::AlphaTested)
        {
            mSortedSceneObjects.AlphaTestObjects.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::Transparent)
        {
            mSortedSceneObjects.TransparentObjects.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::Mirror)
        {
            mSortedSceneObjects.MirrorObjects.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::GenerateTrees)
        {
            mSortedSceneObjects.GenerateTrees.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::GeometrySubdivide)
        {
            mSortedSceneObjects.GeometrySubdivide.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::TesselationInteger)
        {
            mSortedSceneObjects.TesselationInteger.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::TesselationFracOdd)
        {
            mSortedSceneObjects.TesselationFracOdd.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::TesselationFracEven)
        {
            mSortedSceneObjects.TesselationFracEven.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::TesselationPow2)
        {
            mSortedSceneObjects.TesselationPow2.push_back(obj.get());
        }
        else if (obj->GetRenderLayer() == ERenderLayer::Skybox)
        {
            mSortedSceneObjects.SkyBox.push_back(obj.get());
        }

        if (obj->GetRenderLayer() == ERenderLayer::Opaque)
        {
            if (obj->GetPrimitiveType() != EPrimitiveType::GRID && obj->GetPrimitiveType() != EPrimitiveType::LAND && obj->GetPrimitiveType() != EPrimitiveType::WAVES)
            {
                mSortedSceneObjects.Models.push_back(obj.get());
            }
        }
    }
}
