#include "Graphic.h"

Graphic::Graphic(UINT Width, UINT Height, HWND hwnd) : m_ClientWidth(Width), m_ClientHeight(Height), m_hMainWnd(hwnd)
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
    CheckMSAASupport();
    CreateCommandObjects();
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();
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

void Graphic::CheckMSAASupport()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels;
    msQualityLevels.Format = m_BackBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    m_Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)) >> Check;
    m_4xMsaaQuality = msQualityLevels.NumQualityLevels;
    if (m_4xMsaaQuality > 0)
    {
        m_4xMsaaState = true;
    }
    
    assert(m_4xMsaaQuality > 0 && "Unexpected MSAA quality level");
}

void Graphic::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.NodeMask = 0;

    m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue)) >> Check;

    m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(m_CommandAlloc.GetAddressOf())) >> Check;

    m_Device->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAlloc.Get(), nullptr, IID_PPV_ARGS(m_CommandList.GetAddressOf())) >>
        Check;

    m_CommandList->Close();
}

void Graphic::CreateSwapChain() 
{
    m_SwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = m_ClientWidth;
    sd.Height = m_ClientHeight;
    sd.Format = m_BackBufferFormat;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = m_SwapChainBufferCount;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.SampleDesc.Count = 1;  
    sd.SampleDesc.Quality = 0;
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;


    ComPtr<IDXGISwapChain1> swapChain;
    m_Factory->CreateSwapChainForHwnd(m_CommandQueue.Get(), m_hMainWnd, &sd, nullptr, nullptr, swapChain.GetAddressOf()) >> Check;

    swapChain.As(&m_SwapChain);
}

void Graphic::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc;
    rtvHeapDesc.NumDescriptors = m_SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(m_RtvHeap.GetAddressOf())) >> Check;

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc;
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    m_Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(m_DsvHeap.GetAddressOf())) >>Check;
}
