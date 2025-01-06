#include "App.h"
#include <string>

App::App() : Wnd(Width, Height), Gfx(Width, Height, Wnd.GetHwnd()) {}

std::optional<int> App::Go()
{
    Timer.Reset();

    while (true)
    {
        Timer.Tick();
        CalculateFrameStats(); 
        if (const auto ecode = Window::PrecessMessages())
        {
            return *ecode;
        }
    }
    return std::nullopt;
}

void App::CalculateFrameStats() 
{
    static int frameCnt = 0;
    static float timeElapsed = 0.0f;
    frameCnt++;

    if ((Timer.GetTotalTime() - timeElapsed) >= 1.0f)
    {
        float fps = static_cast<float>(frameCnt); 
        float mspf = 1000.0f / fps;

        std::string fpsStr = std::to_string(fps);
        std::string mspfStr = std::to_string(mspf);

        auto windowText = Wnd.GetTitle() + " fps :	" + fpsStr + " mspf :	" + mspfStr;

        SetWindowText(Wnd.GetHwnd(), windowText.c_str());

       
        frameCnt = 0;
        timeElapsed += 1.0f;
    }
}
