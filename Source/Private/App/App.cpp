#include "App.h"
#include <string>
#include <thread>
#include <format>
#include <DirectXMath.h>
#include "Vector.h"
#include <chrono>



App::App() : mWnd(Width, Height, this), mGfx(Width, Height, mWnd.GetHwnd()), mCamera(this), mCameraController(&mWnd, &mCamera, &mTimer), mScene(mTimer) {}

std::optional<int> App::Go()
{
    mTimer.Reset();

    while (true)
    {
        mTimer.Tick();
        CalculateFrameStats();
        Update();
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
    mGfx.OnResize(nWidth, nHeight);
}

void App::OnStop()
{
    if (!bAppPaused)
    {
        bAppPaused = true;
        mTimer.Stop();
    }
}

void App::OnStart()
{
    if (bAppPaused)
    {
        bAppPaused = false;
        mTimer.Start();
    }
}

void App::Update()
{
    mCameraController.UpdateInput();
    mGfx.Update(mCamera.GetViewMatrix(), mCamera.GetCameraPos(), mTimer);
}

void App::Draw()
{
    mGfx.Draw();
}

void App::SetWireframe(bool wireframeIsEnabled) 
{
    mGfx.SetWireframe(wireframeIsEnabled);
}

void App::CalculateFrameStats()
{
    static int frameCnt = 0;
    static double timeElapsed = 0.0f;

    frameCnt++;

    if ((mTimer.GetTotalTime() - timeElapsed) >= 1.0)
    {
        double fps = static_cast<double>(frameCnt);
        double mspf = 1000.0 / fps;

        std::string windowText = std::format("{} FPS : {:.2f} MSPF {:.2f} TOTAL TIME : {:.2f} ",
            mWnd.GetTitle(), fps, mspf, mTimer.GetTotalTime());

        SetWindowText(mWnd.GetHwnd(), windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0;
    }
}
