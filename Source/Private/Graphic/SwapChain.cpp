#include "SwapChain.h"
#include "Settings.h"
#include "Device.h"
#include "CommandManager.h"

using namespace DirectX;
using namespace Kds::App;
using namespace Microsoft::WRL;

SwapChain::SwapChain(HWND windowHandle)
{
    Initialize(windowHandle);
}

SwapChain::~SwapChain() = default;

void SwapChain::Initialize( HWND windowHandle)
{
    // Create SwapChain
    mSwapChain.Reset();
    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = Settings::mWidth;
    sd.Height = Settings::mHeight;
    sd.Format = Settings::mBackBufferFormat;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = mSwapChainBufferCount;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    ComPtr<IDXGISwapChain1> swapChainTemp;

    auto factory = Device::GetFactory7();
    auto commandQueue = CommandManager::GetCommandQueue();

    assert(factory && commandQueue);

    if (factory && commandQueue)
    {
        factory->CreateSwapChainForHwnd(commandQueue, windowHandle, &sd, nullptr, nullptr, swapChainTemp.GetAddressOf()) >> Check;
        swapChainTemp.As(&mSwapChain);
        assert(mSwapChain);
    }
    else
    {
        LOG_ERROR("Create Swap chain is failed");
    }

  
}
