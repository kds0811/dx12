#include "Graphic.h"
#include "DirectXColors.h"
#include "UploadBuffer.h"
#include <array>

using namespace DirectX;
using namespace Kds::App;

static constexpr int gNumFrameResources = 3;

// Lightweight structure stores parameters to draw a shape.
struct RenderItem
{
    RenderItem() = default;

    // World matrix of the shape that describes the object's local space
    // relative to the world space, which defines the position, orientation,
    // and scale of the object in the world.
    XMFLOAT4X4 World = MathHelper::Identity4x4();

    // Dirty flag indicating the object data has changed and we need to update the constant buffer.
    // Because we have an object cbuffer for each FrameResource, we have to apply the
    // update to each FrameResource.
    int NumFramesDirty = gNumFrameResources;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    UINT ObjCBIndex = -1;

    MeshGeometry* Geo = nullptr;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};

Graphic::Graphic(UINT Width, UINT Height, HWND hwnd) : mClientWidth(Width), mClientHeight(Height), mWindowHandle(hwnd)
{
    InitPipeline();
    InitResources();


    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);

    XMMATRIX PTrans = XMMatrixTranslation(1.0f, 0.0f, 0.0f);
    XMMATRIX PyrRot = XMMatrixRotationRollPitchYaw(0.0f, 0.0f, 0.0f);
    XMMATRIX PyrScale = XMMatrixScaling(1.0f, 1.0f, 1.0f);

    XMMATRIX pyramidWorld = PTrans * PyrRot * PyrScale;
    XMStoreFloat4x4(&mWorldPyramid, pyramidWorld);
}

Graphic::~Graphic()
{
    if (mDevice != nullptr)
    {
        FlushCommandQueue();
    }
}

float Graphic::GetAspectRatio() const
{
    return static_cast<float>(mClientWidth) / static_cast<float>(mClientHeight);
}

void Graphic::OnResize(UINT nWidth, UINT nHeight)
{
    mClientWidth = nWidth;
    mClientHeight = nHeight;
    assert(mDevice);
    assert(mSwapChain);
    assert(mCommandAlloc);

    FlushCommandQueue();
    mCommandList->Reset(mCommandAlloc.Get(), nullptr) >> Check;

    for (int i = 0; i < mSwapChainBufferCount; ++i)
    {
        mSwapChainBuffer[i].Reset();
    }
   mDepthStencilBuffer.Reset();

    // Resize the swap chain.
    mSwapChain->ResizeBuffers(mSwapChainBufferCount, mClientWidth, mClientHeight, mBackBufferFormat,
        DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) >>
        Check;

    mCurrBackBuffer = 0;

    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < mSwapChainBufferCount; i++)
    {
        mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])) >> Check;
        mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, mRtvDescriptorSize);
    }

    // Create the depth/stencil buffer and view.
    D3D12_RESOURCE_DESC depthStencilDesc{};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = mClientWidth;
    depthStencilDesc.Height = mClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear{};
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    auto HeapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    mDevice->CreateCommittedResource(&HeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear,
        IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())) >>
        Check;

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = mDepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());

    // Transition the resource from its initial state to be used as a depth buffer.
    auto ResBar =
        CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    mCommandList->ResourceBarrier(1, &ResBar);

    // Execute the resize commands.
    mCommandList->Close() >> Check;
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until resize is complete.
    FlushCommandQueue();

    // Update the viewport transform to cover the client area.
    mScreenViewport.TopLeftX = 0;
    mScreenViewport.TopLeftY = 0;
    mScreenViewport.Width = static_cast<float>(mClientWidth);
    mScreenViewport.Height = static_cast<float>(mClientHeight);
    mScreenViewport.MinDepth = 0.0f;
    mScreenViewport.MaxDepth = 1.0f;

    mScissorRect = {0, 0, static_cast<LONG>(mClientWidth), static_cast<LONG>(mClientHeight)};

    // The window resized, so update the aspect ratio and recompute the projection matrix.
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 1000.0f);
    XMStoreFloat4x4(&mProj, P);
}

void Graphic::Draw()
{
    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    mCommandAlloc->Reset() >> Check;

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    mCommandList->Reset(mCommandAlloc.Get(), mPSO.Get()) >> Check;

    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    // Indicate a state transition on the resource usage.
    auto ResBarPresToRT =
        CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    mCommandList->ResourceBarrier(1, &ResBarPresToRT);

    // Clear the back buffer and depth buffer.
    mCommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    mCommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    auto CurBBV = GetCurrentBackBufferView();
    auto CurDSV = GetDepthStencilView();
    mCommandList->OMSetRenderTargets(1, &CurBBV, true, &CurDSV);

    ID3D12DescriptorHeap* descriptorHeaps[] = {mCbvHeap.Get()};
    mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

    auto BoxVB = mBoxGeo->VertexBufferView();
    mCommandList->IASetVertexBuffers(0, 1, &BoxVB);

    auto BoxVCB = mBoxGeo->ColorBufferView();
    mCommandList->IASetVertexBuffers(1, 1, &BoxVCB);

    auto BoxIBV = mBoxGeo->IndexBufferView();
    mCommandList->IASetIndexBuffer(&BoxIBV);

    mCommandList->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    auto handle = mCbvHeap->GetGPUDescriptorHandleForHeapStart();

    // Рисуем куб
    mCommandList->SetGraphicsRootDescriptorTable(0, handle);
    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["box"].IndexCount, 1, mBoxGeo->DrawArgs["box"].StartIndexLocation,
        mBoxGeo->DrawArgs["box"].BaseVertexLocation, 0);

    // Смещаем дескриптор для пирамиды
    handle.ptr += mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Рисуем пирамиду
    mCommandList->SetGraphicsRootDescriptorTable(0, handle);
    mCommandList->DrawIndexedInstanced(mBoxGeo->DrawArgs["pyramid"].IndexCount, 1, mBoxGeo->DrawArgs["pyramid"].StartIndexLocation,
        mBoxGeo->DrawArgs["pyramid"].BaseVertexLocation, 0);

    // Indicate a state transition on the resource usage.
    auto ResBarRTtoPresent =
        CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mCommandList->ResourceBarrier(1, &ResBarRTtoPresent);

    // Done recording commands.
    mCommandList->Close() >> Check;

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // swap the back and front buffers
    mSwapChain->Present(0, 0) >> Check;
    mCurrBackBuffer = (mCurrBackBuffer + 1) % mSwapChainBufferCount;

    // Wait until frame commands are complete.  This waiting is inefficient and is
    // done for simplicity.  Later we will show how to organize our rendering code
    // so we do not have to wait per frame.
    FlushCommandQueue();
}

void Graphic::Update(DirectX::FXMMATRIX ViewMat, float TotalTime)
{
    XMMATRIX view = ViewMat;
    XMStoreFloat4x4(&mView, ViewMat);

    XMMATRIX worldBox = XMLoadFloat4x4(&mWorldBox);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX worldViewProjBox = worldBox * view * proj;

    // Update the constant buffer with the latest worldViewProj matrix.
    ObjectConstants objConstantsBox;
    XMStoreFloat4x4(&objConstantsBox.WorldViewProj, XMMatrixTranspose(worldViewProjBox));
    objConstantsBox.time = TotalTime;
    mObjectCB->CopyData(0, objConstantsBox);

    XMMATRIX worldPyr = XMLoadFloat4x4(&mWorldPyramid);
    XMMATRIX worldViewProjPyr = worldPyr * view * proj;
    ObjectConstants objConstantsPyramid;

    XMStoreFloat4x4(&objConstantsPyramid.WorldViewProj, XMMatrixTranspose(worldViewProjPyr));
    objConstantsPyramid.time = TotalTime;
    mObjectCB->CopyData(1, objConstantsPyramid);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphic::GetCurrentBackBufferView() const noexcept
{
    return CD3DX12_CPU_DESCRIPTOR_HANDLE(mRtvHeap->GetCPUDescriptorHandleForHeapStart(), mCurrBackBuffer, mRtvDescriptorSize);
}

D3D12_CPU_DESCRIPTOR_HANDLE Graphic::GetDepthStencilView() const noexcept
{
    return mDsvHeap->GetCPUDescriptorHandleForHeapStart();
}

ID3D12Resource* Graphic::CurrentBackBuffer() const noexcept
{
    return mSwapChainBuffer[mCurrBackBuffer].Get();
}

void Graphic::InitPipeline()
{
    // enable debug layer
#if defined(_DEBUG)
    D3D12GetDebugInterface(IID_PPV_ARGS(&mDebugController)) >> Check;
    mDebugController->EnableDebugLayer();
#endif

    // create FACTORY
    UINT CreateFactoryFlags = 0;
#if defined(_DEBUG)
    CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&mFactory)) >> Check;

    // Create DEVICE
    // try create hardware device
    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&mDevice));
    // if create hardware device is failed, try create WARP device
    if (FAILED(hr))
    {
        ComPtr<IDXGIAdapter> pWarpAdapter;
        mFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)) >> Check;
        D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&mDevice)) >> Check;
    }

    // Create Fence
    mDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)) >> Check;

    // Set Descriptors Size
    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Create Command Objects
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.NodeMask = 0;
    mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)) >> Check;
    mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAlloc.GetAddressOf())) >> Check;
    mDevice->CreateCommandList(
        0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAlloc.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >>
        Check;
    mCommandList->Close();

    // Create SwapChain
    mSwapChain.Reset();
    DXGI_SWAP_CHAIN_DESC1 sd = {};
    sd.Width = mClientWidth;
    sd.Height = mClientHeight;
    sd.Format = mBackBufferFormat;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.BufferCount = mSwapChainBufferCount;
    sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Scaling = DXGI_SCALING_STRETCH;
    sd.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
    ComPtr<IDXGISwapChain1> swapChainTemp;
    mFactory->CreateSwapChainForHwnd(mCommandQueue.Get(), mWindowHandle, &sd, nullptr, nullptr, swapChainTemp.GetAddressOf()) >> Check;
    swapChainTemp.As(&mSwapChain);

    // Create RTV Descriptor Heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = mSwapChainBufferCount;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())) >> Check;

    // Create DSV Descriptor Heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())) >> Check;

    // Create RTV for SWAPCHAIN
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < mSwapChainBufferCount; i++)
    {
        mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])) >> Check;
        mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, mRtvDescriptorSize);
    }

    // Create DSV ForSwapChain
    D3D12_RESOURCE_DESC depthStencilDesc{};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = 0;
    depthStencilDesc.Width = mClientWidth;
    depthStencilDesc.Height = mClientHeight;
    depthStencilDesc.DepthOrArraySize = 1;
    depthStencilDesc.MipLevels = 1;
    depthStencilDesc.Format = mDepthStencilFormat;
    depthStencilDesc.SampleDesc.Count = 1;
    depthStencilDesc.SampleDesc.Quality = 0;
    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    D3D12_CLEAR_VALUE optClear{};
    optClear.Format = mDepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;
    CD3DX12_HEAP_PROPERTIES HeapProp(D3D12_HEAP_TYPE_DEFAULT);
    mDevice->CreateCommittedResource(&HeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear,
        IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())) >>
        Check;
    mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, GetDepthStencilView());
    auto ResBar =
        CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    mCommandList->ResourceBarrier(1, &ResBar);

    // Create And SetViewport
    mScreenViewport.TopLeftX = 0.0f;
    mScreenViewport.TopLeftY = 0.0f;
    mScreenViewport.Width = static_cast<float>(mClientWidth);
    mScreenViewport.Height = static_cast<float>(mClientHeight);
    mScreenViewport.MinDepth = 0.0f;
    mScreenViewport.MaxDepth = 1.0f;
    mCommandList->RSSetViewports(1, &mScreenViewport);

    // Create Scissor Rect
    mScissorRect = {0, 0, static_cast<long>(mClientWidth), static_cast<long>(mClientHeight)};
    mCommandList->RSSetScissorRects(1, &mScissorRect);



    
}

void Graphic::InitResources() {
    // Reset the command list to prep for initialization commands.
    mCommandList->Reset(mCommandAlloc.Get(), nullptr) >> Check;

    BuildDescriptorHeaps();
    BuildConstantBuffers();
    BuildRootSignature();
    BuildShadersAndInputLayout();
    BuildBoxGeometry();
    BuildPSO();

    // Execute the initialization commands.
    mCommandList->Close() >> Check;
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();
}

void Graphic::FlushCommandQueue()
{
    mCurrentFence++;
    mCommandQueue->Signal(mFence.Get(), mCurrentFence) >> Check;

    if (mFence->GetCompletedValue() < mCurrentFence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(mCurrentFence, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void Graphic::BuildDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc{};
    cbvHeapDesc.NumDescriptors = 2;
    cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    cbvHeapDesc.NodeMask = 0;
    mDevice->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&mCbvHeap)) >> Check;
}

void Graphic::BuildConstantBuffers()
{
    mObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(mDevice.Get(), 2, true);

    UINT objCBByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    D3D12_GPU_VIRTUAL_ADDRESS cbAddress = mObjectCB->Resource()->GetGPUVirtualAddress();
    // Offset to the ith object constant buffer in the buffer.
    int boxCBufIndex = 0;
    cbAddress += boxCBufIndex * objCBByteSize;

    D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc{};
    cbvDesc.BufferLocation = cbAddress;
    cbvDesc.SizeInBytes = objCBByteSize;

    // Получаем начальный CPU дескриптор
    auto cpuHandle = mCbvHeap->GetCPUDescriptorHandleForHeapStart();
    mDevice->CreateConstantBufferView(&cbvDesc, cpuHandle);

    // Второй CBV (для пирамиды)
    // Смещаем CPU дескриптор
    cpuHandle.ptr += mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    // Смещаем адрес буфера
    cbvDesc.BufferLocation = cbAddress + objCBByteSize;

    mDevice->CreateConstantBufferView(&cbvDesc, cpuHandle);
}

void Graphic::BuildRootSignature()
{
    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[1]{};

    // Create a single descriptor table of CBVs.
    CD3DX12_DESCRIPTOR_RANGE cbvTable{};
    cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 2, 0);
    slotRootParameter[0].InitAsDescriptorTable(1, &cbvTable);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(1, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr =
        D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());
    hr >> Check;
    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }

    mDevice->CreateRootSignature(
        0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(&mRootSignature)) >>
        Check;
}

void Graphic::BuildShadersAndInputLayout()
{
    HRESULT hr = S_OK;

    mvsByteCode = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Color.hlsl", nullptr, "VS", "vs_5_0");
    mpsByteCode = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Color.hlsl", nullptr, "PS", "ps_5_0");

    mInputLayout = {{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}};
}

void Graphic::BuildBoxGeometry()
{
    std::array<VertexPos, 13> verticesPos = {VertexPos({XMFLOAT3(-1.0f, -1.0f, -1.0f)}), VertexPos({XMFLOAT3(-1.0f, +1.0f, -1.0f)}),
        VertexPos({XMFLOAT3(+1.0f, +1.0f, -1.0f)}), VertexPos({XMFLOAT3(+1.0f, -1.0f, -1.0f)}), VertexPos({XMFLOAT3(-1.0f, -1.0f, +1.0f)}),
        VertexPos({XMFLOAT3(-1.0f, +1.0f, +1.0f)}), VertexPos({XMFLOAT3(+1.0f, +1.0f, +1.0f)}),
        VertexPos({XMFLOAT3(+1.0f, -1.0f, +1.0f)}),  // BOX 8
        VertexPos({XMFLOAT3(-0.5f, -0.5f, -0.5f)}),  // 8: передний левый
        VertexPos({XMFLOAT3(0.5f, -0.5f, -0.5f)}),   // 9: передний правый
        VertexPos({XMFLOAT3(0.5f, -0.5f, 0.5f)}),    // 10: задний правый
        VertexPos({XMFLOAT3(-0.5f, -0.5f, 0.5f)}),   // 11: задний левый
        VertexPos({XMFLOAT3(0.0f, 0.5f, 0.0f)})};    // 12: вершина

    std::array<VertexCol, 13> verticesCol = {VertexCol({XMFLOAT4(Colors::White)}), VertexCol({XMFLOAT4(Colors::Black)}),
        VertexCol({XMFLOAT4(Colors::Red)}), VertexCol({XMFLOAT4(Colors::Green)}), VertexCol({XMFLOAT4(Colors::Blue)}),
        VertexCol({XMFLOAT4(Colors::Yellow)}), VertexCol({XMFLOAT4(Colors::Cyan)}), VertexCol({XMFLOAT4(Colors::Magenta)}),  // Box 8
        VertexCol({XMFLOAT4(Colors::Red)}), VertexCol({XMFLOAT4(Colors::Green)}), VertexCol({XMFLOAT4(Colors::Blue)}),
        VertexCol({XMFLOAT4(Colors::Yellow)}), VertexCol({XMFLOAT4(Colors::White)})};  // Pyramid 5

    std::array<std::uint16_t, 54> indices = {
        // Куб (36 индексов) - оставляем как есть
        0, 1, 2, 0, 2, 3,  // front
        4, 6, 5, 4, 7, 6,  // back
        4, 5, 1, 4, 1, 0,  // left
        3, 2, 6, 3, 6, 7,  // right
        1, 5, 6, 1, 6, 2,  // top
        4, 0, 3, 4, 3, 7,  // bottom

        // Пирамида (18 индексов)
        8 + 0, 8 + 2, 8 + 1,  // основание треугольник 1
        8 + 0, 8 + 3, 8 + 2,  // основание треугольник 2
        8 + 0, 8 + 1, 8 + 4,  // передняя грань
        8 + 1, 8 + 2, 8 + 4,  // правая грань
        8 + 2, 8 + 3, 8 + 4,  // задняя грань
        8 + 3, 8 + 0, 8 + 4   // левая грань
    };

    const UINT vbPosByteSize = (UINT)verticesPos.size() * sizeof(VertexPos);
    const UINT vbColByteSize = (UINT)verticesCol.size() * sizeof(VertexCol);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    mBoxGeo = std::make_unique<MeshGeometry>();
    mBoxGeo->Name = "boxGeo";

    D3DCreateBlob(vbPosByteSize, &mBoxGeo->VertexBufferCPU) >> Check;
    CopyMemory(mBoxGeo->VertexBufferCPU->GetBufferPointer(), verticesPos.data(), vbPosByteSize);

    D3DCreateBlob(vbColByteSize, &mBoxGeo->ColorBufferCPU) >> Check;
    CopyMemory(mBoxGeo->ColorBufferCPU->GetBufferPointer(), verticesCol.data(), vbColByteSize);

    D3DCreateBlob(ibByteSize, &mBoxGeo->IndexBufferCPU) >> Check;
    CopyMemory(mBoxGeo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    mBoxGeo->VertexBufferGPU =
        D3D12Utils::CreateDefaultBuffer(mDevice.Get(), mCommandList.Get(), verticesPos.data(), vbPosByteSize, mBoxGeo->VertexBufferUploader);

    mBoxGeo->ColorBufferGPU =
        D3D12Utils::CreateDefaultBuffer(mDevice.Get(), mCommandList.Get(), verticesCol.data(), vbColByteSize, mBoxGeo->ColorBufferUploader);

    mBoxGeo->IndexBufferGPU =
        D3D12Utils::CreateDefaultBuffer(mDevice.Get(), mCommandList.Get(), indices.data(), ibByteSize, mBoxGeo->IndexBufferUploader);

    mBoxGeo->VertexByteStride = sizeof(VertexPos);
    mBoxGeo->VertexBufferByteSize = vbPosByteSize;
    mBoxGeo->ColorByteStride = sizeof(VertexCol);
    mBoxGeo->ColorBufferByteSize = vbColByteSize;
    mBoxGeo->IndexFormat = DXGI_FORMAT_R16_UINT;
    mBoxGeo->IndexBufferByteSize = ibByteSize;

    SubmeshGeometry submeshBox;
    submeshBox.IndexCount = 36u;
    submeshBox.StartIndexLocation = 0;
    submeshBox.BaseVertexLocation = 0;
    mBoxGeo->DrawArgs["box"] = submeshBox;

    SubmeshGeometry submeshPyramid;
    submeshPyramid.IndexCount = 18u;
    submeshPyramid.StartIndexLocation = 36u;
    submeshPyramid.BaseVertexLocation = 8;
    mBoxGeo->DrawArgs["pyramid"] = submeshPyramid;
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
    psoDesc.RTVFormats[0] = mBackBufferFormat;
    psoDesc.SampleDesc.Count = 1;
    psoDesc.SampleDesc.Quality = 0;
    psoDesc.DSVFormat = mDepthStencilFormat;
    mDevice->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&mPSO)) >> Check;
}
