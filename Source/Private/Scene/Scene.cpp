#include "Scene.h"
#include "Graphic.h"

Scene::Scene(GameTimerW& timer, Graphic* pgfx) : mTimer(timer), pGfx(pgfx) {
}

void Scene::InitScene()
{
    BuildStandartShapeGeometry();

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
    mGeometries["shapeGeo"] = mShapeGeometryBuilder.BuildShapeGeometry(pGfx->GetDevice().Get(), pGfx->GetCommandList().Get());
}
