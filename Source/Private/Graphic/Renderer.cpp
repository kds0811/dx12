#include "Renderer.h"
#include "FrameResourceManager.h"
#include "SwapChain.h"
#include "DepthStencil.h"
#include "Settings.h"


Renderer::Renderer(HWND windowHandle)
{
    mFrameResourceManager = std::make_unique<FrameResourceManager>();
    mSwapChain = std::make_unique<SwapChain>(windowHandle);
    InitDepthStencil();

}

Renderer::~Renderer() {}

void Renderer::InitDepthStencil() 
{
    mDepthStencil = std::make_unique<DepthStencil>();
    mDepthStencil->Initialize(L"Depth Stencil", Settings::mWidth, Settings::mHeight, DXGI_FORMAT_D24_UNORM_S8_UINT);
    if (!mDepthStencil->IsInitialized())
    {
        LOG_ERROR("Depth Stencil is not initialized");
    }
}

D3D12_CPU_DESCRIPTOR_HANDLE Renderer::GetDepthStencilView()
{
    assert(mDepthStencil);
    if (mDepthStencil)
    {
        auto handle = mDepthStencil->GetDsvDescriptorHandle();
        if (!handle.IsNull())
        {
            return handle.GetCpuHandle();
        }
        else
        {
            LOG_ERROR("Current Back Buffer handle to view is Null");
            return D3D12_CPU_DESCRIPTOR_HANDLE{};
        }
    }
    else
    {
        LOG_ERROR("mCurrBackBuffer is nullptr");
        return D3D12_CPU_DESCRIPTOR_HANDLE{};
    }
}
