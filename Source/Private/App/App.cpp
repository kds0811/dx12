#include "App.h"
#include <string>
#include <thread>
#include <format>

App::App() :
    Wnd(Width, Height, this),
    Gfx(Width, Height, Wnd.GetHwnd()) {}

std::optional<int> App::Go()
{
    Timer.Reset();

    while (true)
    {
        Timer.Tick();
        CalculateFrameStats();
        Draw();
        if (const auto ecode = Window::PrecessMessages())
        {
            return *ecode;
        }
    }
    return std::nullopt;
}

void App::OnResize(UINT nWidth, UINT nHeight)
{
    Width = nWidth;
    Height = nHeight;
    Gfx.OnResize(nWidth, nHeight);
}

void App::OnStop()
{
    if (!bAppPaused)
    {
        bAppPaused = true;
        Timer.Stop();
    }
}

void App::OnStart()
{
    if (bAppPaused)
    {
        bAppPaused = false;
        Timer.Start();
    }
}

void App::Update(const GameTimerW& gt) {}

void App::Draw() 
{
    Gfx.Draw();
}

void App::CalculateFrameStats()
{
    static int frameCnt = 0;
    static double timeElapsed = 0.0f;
    frameCnt++;

    if ((Timer.GetTotalTime() - timeElapsed) >= 1.0)
    {
        double fps = static_cast<double>(frameCnt);
        double mspf = 1000.0 / fps;

        std::string windowText = std::format("{} FPS : {:.2f} MSPF {:.2f} TOTAL TIME : {:.2f}", Wnd.GetTitle(), fps, mspf, Timer.GetTotalTime());

        SetWindowText(Wnd.GetHwnd(), windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0;
    }
}
