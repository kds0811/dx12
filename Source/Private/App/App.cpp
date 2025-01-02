#include "App.h"

App::App() : Wnd(Width, Height), Gfx(Width, Height, Wnd.GetHwnd()) {}

std::optional<int> App::Go()
{
    if (const auto ecode = Window::PrecessMessages())
    {
        return *ecode;
    }
    return std::nullopt;
}
