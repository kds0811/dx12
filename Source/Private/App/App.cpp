#include "App.h"
#include <string>
#include <thread>
#include <format>
#include <DirectXMath.h>
#include "Vector.h"
#include <chrono>


App::App()
{
    mWnd = std::make_unique<Window>(mWidth, mHeight, this);
    mGfx = std::make_unique<Graphic>(mWidth, mHeight, mWnd->GetHwnd());
    mTimer = std::make_unique<GameTimerW>();
    mCamera = std::make_unique<Camera>(this);
    mCameraController = std::make_unique<CameraController>(mWnd.get(), mCamera.get(), mTimer.get());
    mScene = std::make_unique<Scene>(mTimer.get(), mGfx.get());
    mResourceManager = std::make_unique<ResourceManager>(mGfx->GetDevice(), mGfx->GetCommandQueue());


    mGfx->BuildStandartShapeGeometry();
    mScene->InitScene();
    mGfx->InitResources(mScene->GetSceneRenderItems().size());
}

std::optional<int> App::Go()
{
    mTimer->Reset();

    while (true)
    {
        mTimer->Tick();
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

void App::OnResize(UINT width, UINT height)
{
    mWidth = width;
    mHeight = height;
    mGfx->OnResize(width, height);
}

void App::OnStop()
{
    if (!bAppPaused)
    {
        bAppPaused = true;
        mTimer->Stop();
    }
}

void App::OnStart()
{
    if (bAppPaused)
    {
        bAppPaused = false;
        mTimer->Start();
    }
}

void App::Update()
{
    mCameraController->UpdateInput();
    mScene->Update();
    mGfx->Update(mCamera->GetViewMatrix(), mCamera->GetCameraPos(), mTimer.get(), mScene->GetSceneObjects());
}

void App::Draw()
{
    mGfx->Draw(mScene->GetSceneObjects());
}

void App::SetWireframe(bool wireframeIsEnabled) 
{
    mGfx->SetWireframe(wireframeIsEnabled);
}

void App::CalculateFrameStats()
{
    static int frameCnt = 0;
    static double timeElapsed = 0.0f;

    frameCnt++;

    if ((mTimer->GetTotalTime() - timeElapsed) >= 1.0)
    {
        double fps = static_cast<double>(frameCnt);
        double mspf = 1000.0 / fps;

        std::string windowText = std::format("{} FPS : {:.2f} MSPF {:.2f} TOTAL TIME : {:.2f} ",
            mWnd->GetTitle(), fps, mspf, mTimer->GetTotalTime());

        SetWindowText(mWnd->GetHwnd(), windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0;
    }
}
