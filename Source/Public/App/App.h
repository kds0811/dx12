#pragma once
#include "WindowDK.h"
#include "Graphic.h"
#include "GameTimerW.h"
#include "Camera.h"
#include "CameraController.h"

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

private:
    UINT Width = 1600;
    UINT Height = 1000;

    Window Wnd;
    Graphic Gfx;
    GameTimerW Timer;
    Camera Cam;
    CameraController CamController;
    
    bool bAppPaused = false;

    void CalculateFrameStats();
};