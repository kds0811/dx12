#pragma once
#include "WindowDK.h"
#include "Graphic.h"
#include "GameTimerW.h"
#include "Camera.h"
#include "CameraController.h"
#include "Scene.h"
#include <memory>
#include "ResourceManager.h"
#include "ImguiWrapper.h"



class App
{
public:
    App();
    ~App();
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
    UINT mWidth = 2000;
    UINT mHeight = 1200;
    std::unique_ptr<Window> mWnd = nullptr;
    std::unique_ptr<Graphic> mGfx = nullptr;
    std::unique_ptr<GameTimerW> mTimer = nullptr;
    std::unique_ptr<Camera> mCamera = nullptr;
    std::unique_ptr<CameraController> mCameraController = nullptr;
    std::unique_ptr<ResourceManager> mResourceManager = nullptr;
    std::unique_ptr<Scene> mScene = nullptr;
    std::unique_ptr<ImguiWrapper> mImguiWrapper = nullptr;
    bool bAppPaused = false;
    std::uint64_t mFrameCount = 0;

    void CalculateFrameStats();

};