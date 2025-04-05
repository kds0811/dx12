#pragma once
#include "GraphicCommonHeaders.h"
#include <array>
#include <RenderTarget.h>

class CommandList;

class SwapChain
{
    static constexpr UINT mSwapChainBufferCount = 2;
    Microsoft::WRL::ComPtr<IDXGISwapChain4> mSwapChain;
    std::array<std::unique_ptr<RenderTarget>, mSwapChainBufferCount> mSwapChainBuffer;
    int mCurrBackBufferIndex = 0;
    RenderTarget* mCurrBackBuffer;
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    UINT mWidth = 1600;
    UINT mHeight = 800;
    

public:
    SwapChain(HWND windowHandle);
    ~SwapChain();

    void PreperingToStartDrawingFrame(CommandList* cmdList);

    void CycleToNextBackBuffer();

private:
    void Initialize(HWND windowHandle);

    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView();

}; 