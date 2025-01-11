#include "Graphic.h"
#include "DirectXColors.h"
#include "UploadBuffer.h"
#include <array>

using namespace DirectX;
using namespace Kds::App;

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

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void Graphic::Draw()
{

    // CommandList->Reset(CommandAlloc.Get(), nullptr) >> Check;

    // auto ResBar =
    //     CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    // CommandList->ResourceBarrier(1, &ResBar);
    // CommandList->RSSetViewports(1, &ScreenViewport);
    // CommandList->RSSetScissorRects(1, &ScissorRect);
    // CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), DirectX::Colors::LightSteelBlue, 0, nullptr);
    // CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    ////	Specify	the	buffers	we	are	going	to	render	to.
    // auto CBV = GetCurrentBackBufferView();
    // auto DSV = GetDepthStencilView();
    // CommandList->OMSetRenderTargets(1, &CBV, true, &DSV);

    ////	Indicate	a	state	transition	on	the	resource usage
    // auto ResBar1 =
    //     CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    // CommandList->ResourceBarrier(1, &ResBar1);
    ////	Done	recording	commands.
    // CommandList->Close() >> Check;

    ////	Add	the	command	list	to	the	queue	for	execution.
    // ID3D12CommandList* cmdsLists[] = {CommandList.Get()};
    // CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    ////	swap	the	back	and	front	buffers
    // SwapChain->Present(0, 0) >> Check;
    // CurrBackBuffer = (CurrBackBuffer + 1) % SwapChainBufferCount;
    // FlushCommandQueue();

    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    CommandAlloc->Reset() >> Check;

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    CommandList->Reset(CommandAlloc.Get(), mPSO.Get()) >> Check;

    CommandList->RSSetViewports(1, &ScreenViewport);
    CommandList->RSSetScissorRects(1, &ScissorRect);

    // Indicate a state transition on the resource usage.
    auto ResBarPresToRT =
        CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    CommandList->ResourceBarrier(1, &ResBarPresToRT);

    // Clear the back buffer and depth buffer.
    CommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    CommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    auto CurBBV = GetCurrentBackBufferView();
    auto CurDSV = GetDepthStencilView();
    CommandList->OMSetRenderTargets(1, &CurBBV, true, &CurDSV);

    ID3D12DescriptorHeap* descriptorHeaps[] = {CbvHeap.Get()};
    CommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    CommandList->SetGraphicsRootSignature(mRootSignature.Get());


    auto BoxVBC = mBoxGeo->VertexBufferView();
    CommandList->IASetVertexBuffers(0, 1, &BoxVBC);

    auto BoxIBV = mBoxGeo->IndexBufferView();
    CommandList->IASetIndexBuffer(&BoxIBV);
    CommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    CommandList->SetGraphicsRootDescriptorTable(0, CbvHeap->GetGPUDescriptorHandleForHeapStart());

    CommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount, 1, 0, 0, 0);

    // Indicate a state transition on the resource usage.
    auto ResBarRTtoPresent =
        CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    CommandList->ResourceBarrier(1, &ResBarRTtoPresent);

    // Done recording commands.
    CommandList->Close() >> Check;

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = {CommandList.Get()};
    CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // swap the back and front buffers
    SwapChain->Present(0, 0) >> Check;
    CurrBackBuffer = (CurrBackBuffer + 1) % SwapChainBufferCount;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    FlushCommandQueue();
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

    // Reset the command list to prep for initialization commands.
    CommandList->Reset(CommandAlloc.Get(), nullptr) >> Check;

    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildBoxGeometry();
    BuildPSO();

    // Execute the initialization commands.
    CommandList->Close() >> Check;
    ID3D12CommandList* cmdsLists[] = {CommandList.Get()};
    CommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();
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
        D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&Device)) >> Check;
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

void Graphic::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
    cbvHeapDesc.NumDescriptors = 1;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    Device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&CbvHeap)) >> Check;
}

void Graphic::BuildConstantBuffers()
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(Device.Get(), 1, true);

    UINT objCBByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer.
    int boxCBufIndex = 0;
    cbAddress += boxCBufIndex * objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = objCBByteSize;

    Device->CreateConstantBufferView(&cbvDesc, CbvHeap->GetCPUDescriptorHandleForHeapStart());
}

void Graphic::BuildRootSignature()
{
    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[1]{};

    // Create a single descriptor table of CBVs.
    CD3DX12_DESCRIPTOR_RANGE cbvTable{};
    cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
    slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr =
        D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }

    Device->CreateRootSignature(
        0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)) >>
        Check;
}

void Graphic::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    mvsByteCode = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Color.hlsl", nullptr, "VS", "vs_5_0");
    mpsByteCode = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Color.hlsl", nullptr, "PS", "ps_5_0");

    mInputLayout = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};
}

void Graphic::BuildBoxGeometry()
{
    std::array<Vertex, 8> vertices = {Vertex({XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White)}),
        Vertex({XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black)}), Vertex({XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red)}),
        Vertex({XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green)}), Vertex({XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue)}),
        Vertex({XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow)}), Vertex({XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan)}),
        Vertex({XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta)})};

    std::array<std::uint16_t, 36> indices = {// front face
        0, 1, 2, 0, 2, 3,

        // back face
        4, 6, 5, 4, 7, 6,

        // left face
        4, 5, 1, 4, 1, 0,

        // right face
        3, 2, 6, 3, 6, 7,

        // top face
        1, 5, 6, 1, 6, 2,

        // bottom face
        4, 0, 3, 4, 3, 7};

    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    mBoxGeo = std::make_unique<MeshGeometry>();
    mBoxGeo->Name = "boxGeo";

    D3DCreateBlob(vbByteSize, &mBoxGeo->VertexBufferCPU) >> Check;
    CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU) >> Check;
    CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mBoxGeo->VertexBufferGPU =
        D3D12Utils::CreateDefaultBuffer(Device.Get(), CommandList.Get(), vertices.data(), vbByteSize, mBoxGeo->VertexBufferUploader);

    mBoxGeo->IndexBufferGPU =
        D3D12Utils::CreateDefaultBuffer(Device.Get(), CommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

    mBoxGeo->VertexByteStride = sizeof(Vertex);
    mBoxGeo->VertexBufferByteSize = vbByteSize;
    mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
    mBoxGeo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submesh;
    submesh.IndexCount = (UINT)indices.size();
    submesh.StartIndexLocation = 0;
    submesh.BaseVertexLocation = 0;

    mBoxGeo->DrawArgs["box"] = submesh;
}

void Graphic::BuildPSO()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc;
    ZeroMemory(&psoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    psoDesc.InputLayout = {mInputLayout.data(), (UINT)mInputLayout.size()};
    psoDesc.pRootSignature = mRootSignature.Get();
    psoDesc.VS = {reinterpret_cast<BYTE*>(mvsByteCode->GetBufferPointer()), mvsByteCode->GetBufferSize()};
    psoDesc.PS = {reinterpret_cast<BYTE*>(mpsByteCode->GetBufferPointer()), mpsByteCode->GetBufferSize()};
    psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    psoDesc.SampleMask = UINT_MAX;
    psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    psoDesc.NumRenderTargets = 1;
    psoDesc.RTVFormats[0] = BackBufferFormat;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = DepthStencilFormat;
    Device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)) >> Check;
}
