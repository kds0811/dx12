#include "App.h"
#include <string>
#include <thread>
#include <format>
#include <DirectXMath.h>

using namespace DirectX;

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
        Update(Timer);
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

void App::Update(const GameTimerW& gt) 
{
    // Convert Spherical to Cartesian coordinates.
    float x = mRadius * sinf(mPhi) * cosf(mTheta);
    float z = mRadius * sinf(mPhi) * sinf(mTheta);
    float y = mRadius * cosf(mPhi);

    // Build the view matrix.
    XMVECTOR pos = XMVectorSet(x, y, z, 1.0f);
    XMVECTOR target = XMVectorZero();
    XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

    XMMATRIX view = XMMatrixLookAtLH(pos, target, up);
    XMStoreFloat4x4(&mView, view);

    XMMATRIX world = XMLoadFloat4x4(&mWorld);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProj = world * view * proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstants;
    XMStoreFloat4x4(&objConstants.WorldViewProj, XMMatrixTranspose(worldViewProj));
    mObjectCB->CopyData(0, objConstants);

}

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
