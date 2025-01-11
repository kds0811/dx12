#pragma once
#include "WindowDK.h"
#include "Graphic.h"
#include "GameTimerW.h"
#include "Camera.h"

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
    void Update(const GameTimerW& gt);
    void Draw();

private:
    UINT Width = 1600;
    UINT Height = 1000;

    Window mWindow;
    Graphic mGraphics;
    GameTimerW mTimer;
    Camera mCamera;
    
    bool bAppPaused = false;

    void CalculateFrameStats();
};