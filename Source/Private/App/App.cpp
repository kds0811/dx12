#include "App.h"
#include <string>
#include <thread>
#include <format>
#include <DirectXMath.h>
#include "Vector.h"

using namespace DirectX;
std::string SIMDSupport{};

App::App() : mWindow(Width, Height, this), mGraphics(Width, Height, mWindow.GetHwnd()) 
{
    if (DirectX::XMVerifyCPUSupport())
    {
        SIMDSupport =  "DirectXMath SIMD Extensions Supported\n";
    }
    else
    {
        SIMDSupport =  "DirectXMath SIMD Extensions NOT Supported\n";
    }
}

std::optional<int> App::Go()
{
    mTimer.Reset();

    while (true)
    {
        mTimer.Tick();
        CalculateFrameStats();
        Update(mTimer);
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
    mGraphics.OnResize(nWidth, nHeight);
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

void App::Update(const GameTimerW& gt)
{
    //// Convert Spherical to Cartesian coordinates.
    // float x = mRadius * sinf(mPhi) * cosf(mTheta);
    // float z = mRadius * sinf(mPhi) * sinf(mTheta);
    // float y = mRadius * cosf(mPhi);

    //// Build the view matrix.
    // XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    // XMVECTOR target = XMVectorZero();
    // XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    // XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    // XMStoreFloat4x4(&mView, view);

    // XMMATRIX world = XMLoadFloat4x4(&mWorld);
    // XMMATRIX proj = XMLoadFloat4x4(&mProj);
    // XMMATRIX worldViewProj = world * view * proj;

    //// Update the constant buffer with the latest worldViewProj matrix.
    // ObjectConstants objConstants;
    // XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    // mObjectCB->CopyData(0, objConstants);
}

void App::Draw()
{
    mGraphics.Draw();
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
        int xMouse = mWindow.mouse.GetPosX();
        int yMouse = mWindow.mouse.GetPosX();


        std::string windowText = std::format("{} FPS : {:.2f} MSPF {:.2f} TOTAL TIME : {:.2f}  SIMD - {} ",
            mWindow.GetTitle(), fps, mspf, mTimer.GetTotalTime(), SIMDSupport);

        SetWindowText(mWindow.GetHwnd(), windowText.c_str());

        frameCnt = 0;
        timeElapsed += 1.0;
    }
}
