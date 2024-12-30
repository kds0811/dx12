#pragma once
#include "WindowDK.h"
#include "Graphic.h"


class App
{
public:
    App();
    std::optional<int> Go();


private:
    Window Wnd;
    Graphic Gfx;

    static constexpr int Width = 1200;
    static constexpr int Height = 800;
};