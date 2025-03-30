#pragma once
#include "GraphicCommonHeaders.h"

class SwapChain
{
    static constexpr UINT mSwapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
    Microsoft::WRL::ComPtr<ID3D12Resource> mSwapChainBuffer[mSwapChainBufferCount];
    int mCurrBackBuffer = 0;

public:
    SwapChain(HWND windowHandle);
    ~SwapChain();

private:
    void Initialize(HWND windowHandle);
};