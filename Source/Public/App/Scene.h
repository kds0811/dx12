#pragma once
#include <vector>
#include <memory>
#include "BaseSceneObject.h"
#include "GameTimerW.h"


class Scene
{
    std::vector<std::unique_ptr<BaseSceneObject>> mSceneObjects;
    GameTimerW& mTimer;

public:
    Scene(GameTimerW& timer) : mTimer(timer) {}
    Scene(const Scene&) = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(const Scene&&) = delete;
    Scene& operator=(const Scene&&) = delete;

    void InitScene();

    void Update();
};