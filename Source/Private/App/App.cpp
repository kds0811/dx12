#include "App.h"
#include <string>
#include <thread>
#include <format>
#include <DirectXMath.h>
#include "Vector.h"
#include <chrono>
#include <cassert>
#include "WavesSceneObject.h"

#include "PixProfile.h"

#if defined PIXPROFILE
#define USE_PIX
#include <pix3.h>
#endif

App::App()
{
    mTimer = std::make_unique<GameTimerW>();
    mWnd = std::make_unique<Window>(mWidth, mHeight, this);
    mGfx = std::make_unique<Graphic>(mWidth, mHeight, mWnd->GetHwnd());
    mMainInputController = std::make_unique<MainInputController>(mWnd.get());
    mCamera = std::make_unique<Camera>(mMainInputController.get(), mTimer.get());
    mResourceManager = std::make_unique<ResourceManager>(mGfx->GetDevice(), mGfx->GetCommandQueue());
    mScene = std::make_unique<Scene>(mTimer.get(), mResourceManager.get());
    mImguiWrapper = std::make_unique<ImguiWrapper>();

    assert(mWnd);
    assert(mGfx);
    assert(mTimer);
    assert(mCamera);
    assert(mMainInputController);

    assert(mScene);
    assert(mResourceManager);

    if (mScene)
    {
        mScene->InitScene();
    }
    if (mGfx)
    {
        assert(mScene);
        assert(mScene->GetWavesPtr());
        mGfx->InitResources(mScene->GetSceneObjectsCounter(), mScene->GetWavesPtr()->GetVertexCount(),
            mResourceManager->GetMaterials().size(), mResourceManager->GetTextures());
    }
    mImguiWrapper->InitImgui(mGfx.get(), mWnd->GetHwnd(), mResourceManager->GetTextures().size(), mCamera.get());
}

App::~App()
{}

std::optional<int> App::Go()
{
    mTimer->Reset();

    while (true)
    {
#if defined PIXPROFILE
        PIXBeginEvent(PIX_COLOR_DEFAULT, "Main LOOP %llu", mFrameCount);
#endif

        mTimer->Tick();
        CalculateFrameStats();
        Update();
        Draw();

#if defined PIXPROFILE
        PIXEndEvent();
#endif

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
#if defined PIXPROFILE
    PIXScopedEvent(PIX_COLOR(0, 255, 0), L"Update");
#endif


    mMainInputController->Update();
    mCamera->UpdateInput();
    UpdateInput();
    mScene->Update();
    mGfx->Update(mCamera->GetViewMatrix(), mCamera->GetCameraPos(), mTimer.get(), mScene->GetSceneObjects(), mScene->GetWavesPtr(),
        mResourceManager->GetMaterials());
}

void App::Draw()
{
#if defined PIXPROFILE
    PIXBeginEvent(mGfx->GetCommandQueue(), PIX_COLOR(0, 0, 255), L"RENDER");
#endif

    mImguiWrapper->StartImguiFrame();
    mGfx->StartDrawFrame(mScene->GetSortedSceneObjects());
    mImguiWrapper->EndImguiFrame();
    mGfx->EndDrawFrame();

#if defined PIXPROFILE
    PIXEndEvent(mGfx->GetCommandQueue());
#endif
}

void App::CalculateFrameStats()
{
#if defined PIXPROFILE
    PIXScopedEvent(PIX_COLOR(255, 0, 255), L"CalculateFrameStats");
#endif
    static int frameCnt = 0;
    static double timeElapsed = 0.0f;

    ++frameCnt;
    ++mFrameCount;

    if ((mTimer->GetTotalTime() - timeElapsed) >= 1.0)
    {
        double fps = static_cast<double>(frameCnt);
        double mspf = 1000.0 / fps;

        std::string windowText = std::format(
            "{} FPS : {:.2f} MSPF {:.2f} TOTAL TIME : {:.2f} Frame : {}", mWnd->GetTitle(), fps, mspf, mTimer->GetTotalTime(), mFrameCount);

        SetWindowText(mWnd->GetHwnd(), windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0;
    }
}

void App::UpdateInput() 
{
    if (mMainInputController->KeyboardKeyIsPressed('1'))
    {
        mGfx->SetWireframe(true);
    }
    if (mMainInputController->KeyboardKeyIsPressed('2'))
    {
        mGfx->SetWireframe(false);
    }

    if (mMainInputController->KeyboardKeyIsPressed('3'))
    {
        mGfx->SetSobel(true);
    }
    if (mMainInputController->KeyboardKeyIsPressed('4'))
    {
        mGfx->SetSobel(false);
    }

    if (mMainInputController->KeyboardKeyIsPressed('5'))
    {
        mGfx->SetBlur(true);
    }
    if (mMainInputController->KeyboardKeyIsPressed('6'))
    {
        mGfx->SetBlur(false);
    }
}


