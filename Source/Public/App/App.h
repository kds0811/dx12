#pragma once
#include "WindowDK.h"
#include "Graphic.h"
#include "GameTimerW.h"
#include "Camera.h"
#include "CameraController.h"
#include "Scene.h"
#include <memory>
#include "ResourceManager.h"


class App
{
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(const App&&) = delete;
    App& operator=(const App&&) = delete;

    std::optional<int> Go();

    void OnResize(UINT nWidth, UINT nHeight);
    void OnStop();
    void OnStart();
    void Update();
    void Draw();
    void SetWireframe(bool wireframeIsEnabled);

private:
    UINT mWidth = 1600;
    UINT mHeight = 1000;

    Window mWnd;
    Graphic mGfx;
    GameTimerW mTimer;
    Camera mCamera;
    CameraController mCameraController;
    Scene mScene;
    ResourceManager mResourceManager;
    
    bool bAppPaused = false;

    void CalculateFrameStats();
};