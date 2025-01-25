#pragma once
#include <vector>
#include <memory>
#include "BaseSceneObject.h"
#include "GameTimerW.h"
#include "ShapeGeometryBuilder.h"
#include "D3D12Utils.h"

class Graphic;

class Scene
{
    std::vector<std::unique_ptr<BaseSceneObject>> mSceneObjects;
    GameTimerW& mTimer;
    Graphic* pGfx = nullptr;
    ShapeGeometryBuilder mShapeGeometryBuilder;
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;

public:
    Scene(GameTimerW& timer, Graphic* pgfx);
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(const Scene&&) = delete;
    Scene& operator=(const Scene&&) = delete;

    void InitScene();

    void Update();

private:
    void BuildStandartShapeGeometry();

};