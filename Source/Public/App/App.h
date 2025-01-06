#pragma once
#include "WindowDK.h"
#include "Graphic.h"
#include "GameTimerW.h"

class App
{
public:
    App();
    App(const App&) = delete;
    App& operator=(const App&) = delete;
    App(const App&&) = delete;
    App& operator=(const App&&) = delete;

    std::optional<int> Go();

    void OnResize();
    void OnStop();
    void OnStart();
    void Update(const GameTimerW& gt);
    void Draw(const GameTimerW& gt);

private:
    Window Wnd;
    Graphic Gfx;
    GameTimerW Timer;

    static constexpr UINT Width = 1600;
    static constexpr UINT Height = 1000;
    bool bAppPaused = false;

    void CalculateFrameStats();
};