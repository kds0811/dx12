#include "App.h"

App::App() : Wnd(Width, Height) {}

std::optional<int> App::Go()
{
    if (const auto ecode = Window::PrecessMessages())
    {
        return *ecode;
    }
    return std::nullopt;
}
