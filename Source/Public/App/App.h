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


    static constexpr UINT Width = 1600;
    static constexpr UINT Height = 1000;
};