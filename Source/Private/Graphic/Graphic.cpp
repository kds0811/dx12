#include "Graphic.h"

Graphic::Graphic(UINT Width, UINT Height, HWND hwnd) : ClientWidth(Width), ClientHeight(Height), WindowHandle(hwnd)
{
    InitPipeline();
}

Graphic::~Graphic()
{
    if (Device != nullptr)
    {
        FlushCommandQueue();
    }
}

float Graphic::GetAspectRatio() const
{
    return static_cast<float>(ClientWidth) / static_cast<float>(ClientHeight);
}

void Graphic::OnResize(UINT nWidth, UINT nHeight)
{
    ClientWidth = nWidth;
    ClientHeight = nHeight;
    assert(Device);
    assert(SwapChain);
    assert(CommandAlloc);

    FlushCommandQueue();
    CommandList->Reset(CommandAlloc.Get(), nullptr) >> Check;

    for (int i = 0; i < SwapChainBufferCount; ++i)
    {
        SwapChainBuffer[i].Reset();
    }
    DepthStencilBuffer.Reset();

    // Resize the swap chain.
    SwapChain->ResizeBuffers(SwapChainBufferCount, ClientWidth, ClientHeight, BackBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) >>
        Check;

    CurrBackBuffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i])) >> Check;
        Device->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, RtvDescriptorSize);
    }

    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc{};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = ClientWidth;
    depthStencilDesc.Height = ClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear{};
    optClear.Format = DepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    auto HeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    Device->CreateCommittedResource(&HeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear,
        IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf())) >>
        Check;

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    Device->CreateDepthStencilView(DepthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());

    // Transition the resource from its initial state to be used as a depth buffer.
    auto ResBar =
        CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    CommandList->ResourceBarrier(1, &ResBar);

    // Execute the resize commands.
    CommandList->Close() >> Check;
    ID3D12CommandList* cmdsLists[] = {CommandList.Get()};
    CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until resize is complete.
    FlushCommandQueue();

    // Update the viewport transform to cover the client area.
    ScreenViewport.TopLeftX = 0;
    ScreenViewport.TopLeftY = 0;
    ScreenViewport.Width = static_cast<float>(ClientWidth);
    ScreenViewport.Height = static_cast<float>(ClientHeight);
    ScreenViewport.MinDepth = 0.0f;
    ScreenViewport.MaxDepth = 1.0f;

    ScissorRect = {0, 0, static_cast<LONG>(ClientWidth), static_cast<LONG>(ClientHeight)};
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphic::GetCurrentBackBufferView() const noexcept
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(RtvHeap->GetCPUDescriptorHandleForHeapStart(), CurrBackBuffer, RtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphic::GetDepthStencilView() const noexcept
{
    return DsvHeap->GetCPUDescriptorHandleForHeapStart();
}

ID3D12Resource* Graphic::CurrentBackBuffer() const noexcept
{
    return SwapChainBuffer[CurrBackBuffer].Get();
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
    CreateRtvforSwapChain();
    CreateDsvForSwapChain();
    CreateAndSetViewport();
    CreateScissorRect();
}

void Graphic::EnableDebugLayer()
{
#if defined(_DEBUG)
    D3D12GetDebugInterface(IID_PPV_ARGS(&DebugController)) >> Check;
    DebugController->EnableDebugLayer();
#endif
}

void Graphic::CreateFactory()
{
    UINT CreateFactoryFlags = 0;
#if defined(_DEBUG)
    CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&Factory)) >> Check;
}

void Graphic::CreateDevice()
{
    // try create hardware device
    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&Device));

    // if create hardware device is failed, try create WARP device
    if (FAILED(hr))
    {
        ComPtr<IDXGIAdapter> pWarpAdapter;
        Factory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)) >> Check;
        D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&Device)) >> Check;
    }
}

void Graphic::CreateFence()
{
    Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&Fence)) >> Check;
}

void Graphic::SetDescriptorSizes()
{
    RtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    DsvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    CbvSrvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Graphic::CheckMSAASupport()
{
    D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS msQualityLevels{};
    msQualityLevels.Format = BackBufferFormat;
    msQualityLevels.SampleCount = 4;
    msQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
    msQualityLevels.NumQualityLevels = 0;
    Device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &msQualityLevels, sizeof(msQualityLevels)) >> Check;
    MsaaQuality4x = msQualityLevels.NumQualityLevels;
    if (MsaaQuality4x > 0)
    {
        MsaaState4x = true;
    }

    assert(MsaaQuality4x > 0 && "Unexpected MSAA quality level");
}

void Graphic::CreateCommandObjects()
{
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.NodeMask = 0;

    Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&CommandQueue)) >> Check;

    Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(CommandAlloc.GetAddressOf())) >> Check;

    Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, CommandAlloc.Get(), nullptr, IID_PPV_ARGS(CommandList.GetAddressOf())) >>
        Check;

    CommandList->Close();
}

void Graphic::CreateSwapChain()
{
    SwapChain.Reset();

    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = ClientWidth;
    sd.Height = ClientHeight;
    sd.Format = BackBufferFormat;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = SwapChainBufferCount;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;

    ComPtr<IDXGISwapChain1> swapChainTemp;
    Factory->CreateSwapChainForHwnd(CommandQueue.Get(), WindowHandle, &sd, nullptr, nullptr, swapChainTemp.GetAddressOf()) >> Check;

    swapChainTemp.As(&SwapChain);
}

void Graphic::CreateRtvAndDsvDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = SwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(RtvHeap.GetAddressOf())) >> Check;

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    Device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(DsvHeap.GetAddressOf())) >> Check;
}

void Graphic::CreateRtvforSwapChain()
{
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(RtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainBufferCount; i++)
    {
        SwapChain->GetBuffer(i, IID_PPV_ARGS(&SwapChainBuffer[i])) >> Check;
        Device->CreateRenderTargetView(SwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, RtvDescriptorSize);
    }
}

void Graphic::CreateDsvForSwapChain()
{
    D3D12_RESOURCE_DESC depthStencilDesc{};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = ClientWidth;
    depthStencilDesc.Height = ClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DepthStencilFormat;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear{};
    optClear.Format = DepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    CD3DX12_HEAP_PROPERTIES HeapProp(D3D12_HEAP_TYPE_DEFAULT);

    Device->CreateCommittedResource(&HeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear,
        IID_PPV_ARGS(DepthStencilBuffer.GetAddressOf())) >>
        Check;

    Device->CreateDepthStencilView(DepthStencilBuffer.Get(), nullptr, GetDepthStencilView());

    auto ResBar =
        CD3DX12_RESOURCE_BARRIER::Transition(DepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    CommandList->ResourceBarrier(1, &ResBar);
}

void Graphic::CreateAndSetViewport()
{
    ScreenViewport.TopLeftX = 0.0f;
    ScreenViewport.TopLeftY = 0.0f;
    ScreenViewport.Width = static_cast<float>(ClientWidth);
    ScreenViewport.Height = static_cast<float>(ClientHeight);
    ScreenViewport.MinDepth = 0.0f;
    ScreenViewport.MaxDepth = 1.0f;
    CommandList->RSSetViewports(1, &ScreenViewport);
}

void Graphic::CreateScissorRect()
{
    ScissorRect = {0, 0, static_cast<long>(ClientWidth / 2), static_cast<long>(ClientHeight / 2)};

    CommandList->RSSetScissorRects(1, &ScissorRect);
}

void Graphic::FlushCommandQueue()
{
    CurrentFence++;
    CommandQueue->Signal(Fence.Get(), CurrentFence) >> Check;

    if (Fence->GetCompletedValue() < CurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        Fence->SetEventOnCompletion(CurrentFence, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}
