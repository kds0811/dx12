#include "Renderer.h"
#include "FrameResourceManager.h"
#include "SwapChain.h"
#include "DepthStencil.h"
#include "Settings.h"
#include "DescriptorHeapManager.h"
#include "CommandManager.h"
#include "Pso.h"
#include "Viewport.h"
#include "ScissorRect.h"


Renderer::Renderer(HWND windowHandle)
{
    mFrameResourceManager = std::make_unique<FrameResourceManager>();
    mSwapChain = std::make_unique<SwapChain>(windowHandle);
    mDepthStencil = std::make_unique<DepthStencil>(L"Depth Stencil", Settings::mWidth, Settings::mHeight, DXGI_FORMAT_D24_UNORM_S8_UINT, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    mViewport = std::make_unique<Viewport>(0.0f, 0.0f, static_cast<float>(Settings::mWidth), static_cast<float>(Settings::mHeight));
    mScissorRect = std::make_unique<ScissorRect>(0u, 0u, Settings::mWidth, Settings::mHeight);
}

Renderer::~Renderer() {}

void Renderer::StartDrawFrame() 
{
    Pso dummyPso{}; // need implement getting actual pso
    cmdList = CommandManager::GetFreeCommandListAndResetIt(&dummyPso);
    assert(cmdList);

    if (!cmdList)
    {
        LOG_ERROR("StartDrawFrame: Received CommandList is nullptr");
        return;
    }
    
    mViewport->SetToPipeline(cmdList);
    mScissorRect->SetToPipeline(cmdList);
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
