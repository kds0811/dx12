#include "Graphic.h"

Graphic::Graphic()
{
    InitPipeline();
}

void Graphic::InitPipeline()
{
    EnableDebugLayer();
    CreateFactory();
    CreateDevice();
    CreateFence();
    SetDescriptorSizes();

}

void Graphic::EnableDebugLayer()
{
#if defined(_DEBUG)
    D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController)) >> Check;
    m_DebugController->EnableDebugLayer();
#endif
}

void Graphic::CreateFactory()
{
    UINT CreateFactoryFlags = 0;
#if defined(_DEBUG)
    CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&m_Factory)) >> Check;
}

void Graphic::CreateDevice()
{
    // try create hardware device
    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&m_Device));

    // if create hardware device is failed, try create WARP device
    if (FAILED(hr))
    {
        ComPtr<IDXGIAdapter> pWarpAdapter;
        m_Factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)) >> Check;
        D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_Device)) >> Check;
    }
}

void Graphic::CreateFence()
{
    m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)) >> Check;
}

void Graphic::SetDescriptorSizes()
{
    m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_DsvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_CbvSrvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}
