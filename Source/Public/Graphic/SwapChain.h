#pragma once
#include "GraphicCommonHeaders.h"
#include <array>
#include <RenderTarget.h>


class SwapChain
{
    static constexpr UINT mSwapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
    std::array<RenderTarget, mSwapChainBufferCount> mSwapChainBuffer;
    int mCurrBackBuffer = 0;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    UINT mWidth = 1600;
    UINT mHeight = 800;
    DirectX::XMFLOAT4 mClearColor{0.7f, 0.7f, 0.7f, 1.0f};
    

public:
    SwapChain(HWND windowHandle);
    ~SwapChain();

private:
    void Initialize(HWND windowHandle);
};