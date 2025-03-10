#include "Graphic.h"
#include "DirectXColors.h"
#include "UploadBuffer.h"
#include <array>
#include "GameTimerW.h"
#include <cassert>
#include "PixProfile.h"

#if defined PIXPROFILE
#define USE_PIX
#include <pix3.h>
#endif

using namespace DirectX;
using namespace Kds::App;
using namespace Microsoft::WRL;

Graphic::Graphic(UINT Width, UINT Height, HWND hwnd) : mClientWidth(Width), mClientHeight(Height), mWindowHandle(hwnd)
{
    InitPipeline();
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
    mSwapChain->ResizeBuffers(mSwapChainBufferCount, mClientWidth, mClientHeight, mBackBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING) >>
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
    mDevice->CreateCommittedResource(
        &HeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())) >>
        Check;

    // Create descriptor to mip level 0 of entire resource using the format of the resource.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = mDepthStencilFormat;
    dsvDesc.Texture2D.MipSlice = 0;
    mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), &dsvDesc, GetDepthStencilView());

    // Transition the resource from its initial state to be used as a depth buffer.
    auto ResBar = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
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
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 100000.0f);
    XMStoreFloat4x4(&mProj, P);

    if (mBilateralFilter)
    {
        mBilateralFilter->OnResize(mClientWidth, mClientHeight);
    }

    if (mSobelFilter)
    {
        mSobelFilter->OnResize(mClientWidth, mClientHeight);
    }

    if (mOffscreenRT)
    {
        mOffscreenRT->OnResize(mClientWidth, mClientHeight);
    }
}

void Graphic::StartDrawFrame(const SortedSceneObjects& sortedSceneObjects)
{
#if defined PIXPROFILE
    PIXScopedEvent(PIX_COLOR(60, 40, 130), L"StartDrawFrame");
#endif

    auto cmdListAlloc = mCurrFrameResource->CmdListAlloc;

    // Reuse the memory associated with command recording.
    // We can only reset when the associated command lists have finished execution on the GPU.
    cmdListAlloc->Reset() >> Check;

    // A command list can be reset after it has been added to the command queue via ExecuteCommandList.
    // Reusing the command list reuses memory.
    if (bIsWireframe)
    {
        mCommandList->Reset(cmdListAlloc.Get(), mPSOs["skyWireframe"].Get()) >> Check;
    }
    else
    {
        mCommandList->Reset(cmdListAlloc.Get(), mPSOs["sky"].Get()) >> Check;
    }

    mCommandList->RSSetViewports(1, &mScreenViewport);
    mCommandList->RSSetScissorRects(1, &mScissorRect);

    if (bIsSobelEnabled)
    {
        // Change offscreen texture to be used as a a render target output.
        auto ResbarOffScreenRTGRtoRT = CD3DX12_RESOURCE_BARRIER::Transition(mOffscreenRT->Resource(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
        mCommandList->ResourceBarrier(1, &ResbarOffScreenRTGRtoRT);

        mCommandList->ClearRenderTargetView(mOffscreenRT->Rtv(), (float*)&mMainPassCB.FogColor, 0, nullptr);
    }
    else
    {
        // Indicate a state transition on the resource usage.
        const auto ResBarrPresentToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        mCommandList->ResourceBarrier(1, &ResBarrPresentToRenderTarget);
        //// Clear the back buffer and depth buffer.
        mCommandList->ClearRenderTargetView(GetCurrentBackBufferView(), Colors::LightSteelBlue, 0, nullptr);
    }

   

    mCommandList->ClearDepthStencilView(GetDepthStencilView(), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0, 0, nullptr);

    // Specify the buffers we are going to render to.
    auto CurBackBuferView = GetCurrentBackBufferView();
    auto OffScreeRTV = mOffscreenRT->Rtv();
    auto DSV = GetDepthStencilView();

    if (bIsSobelEnabled)
    {
        mCommandList->OMSetRenderTargets(1, &OffScreeRTV, true, &DSV);
    }
    else
    {
        mCommandList->OMSetRenderTargets(1, &CurBackBuferView, true, &DSV);
    }
    
    // set textures SRV heaps
    ID3D12DescriptorHeap* descriptorHeaps[] = {mCbvSrvUavDescriptorHeap.Get()};
    mCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    mCommandList->SetGraphicsRootSignature(mRootSignature.Get());

    auto passCB = mCurrFrameResource->PassCB->Resource();
    mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());

    UINT passCBByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(PassConstants));

    CD3DX12_GPU_DESCRIPTOR_HANDLE skyTexDescriptor(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
    skyTexDescriptor.Offset(sortedSceneObjects.SkyBox[0]->GetRenderItem()->Mat->Tex->DiffuseSrvHeapIndex, mCbvSrvUavDescriptorSize);
    mCommandList->SetGraphicsRootDescriptorTable(0, skyTexDescriptor);
    DrawRenderItems(sortedSceneObjects.SkyBox, false);

    if (bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["opaque_wireframe"].Get());
    }
    else
    {
        mCommandList->SetPipelineState(mPSOs["opaque"].Get());
    }
    // render opaque objects
    DrawRenderItems(sortedSceneObjects.OpaqueObjects, false);

    // render geometry sudivided objects
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["geometrySubdivide"].Get());
    }
    else
    {
        mCommandList->SetPipelineState(mPSOs["geometrySubdivideWireFrame"].Get());
    }
    DrawRenderItems(sortedSceneObjects.GeometrySubdivide, false);

    // Draw Tesselation Demo
    // Integer tesselation
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["tess"].Get());
    }
    else
    {
        mCommandList->SetPipelineState(mPSOs["tessWireframe"].Get());
    }
    DrawRenderItems(sortedSceneObjects.TesselationInteger, false);
    // FractionalOdd Tesselation
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["tessFracOdd"].Get());
    }
    else
    {
        mCommandList->SetPipelineState(mPSOs["tessFracOddWireframe"].Get());
    }
    DrawRenderItems(sortedSceneObjects.TesselationFracOdd, false);
    // Fractional even Tesselation
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["tessFracEven"].Get());
    }
    else
    {
        mCommandList->SetPipelineState(mPSOs["tessFracEvenWireframe"].Get());
    }
    DrawRenderItems(sortedSceneObjects.TesselationFracEven, false);
    // Pow2 Tesselation
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["tessPow"].Get());
    }
    else
    {
        mCommandList->SetPipelineState(mPSOs["tessPowWireframe"].Get());
    }
    DrawRenderItems(sortedSceneObjects.TesselationPow2, false);

    // render alpha tested objects
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["alphaTested"].Get());
    }
    else
    {
        mCommandList->SetPipelineState(mPSOs["opaque_wireframe"].Get());
    }
    DrawRenderItems(sortedSceneObjects.AlphaTestObjects, false);

    // render trees
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["treeSprites"].Get());
        DrawRenderItems(sortedSceneObjects.GenerateTrees, false);
    }
    // render transparent objects
    if (!bIsWireframe)
    {
        mCommandList->SetPipelineState(mPSOs["transparent"].Get());
    }
    DrawRenderItems(sortedSceneObjects.TransparentObjects, false);

    // Mark the visible mirror pixels in the stencil buffer with the value 1
    mCommandList->OMSetStencilRef(1);
    mCommandList->SetPipelineState(mPSOs["markStencilMirrors"].Get());
    DrawRenderItems(sortedSceneObjects.MirrorObjects, false);

    // Draw the reflection into the mirror only (only for pixels where the stencil buffer is 1).
    // Note that we must supply a different per-pass constant buffer--one with the lights reflected.
    mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress() + 1 * passCBByteSize);
    mCommandList->SetPipelineState(mPSOs["drawStencilReflections"].Get());
    DrawRenderItems(sortedSceneObjects.OpaqueObjects, true);
    DrawRenderItems(sortedSceneObjects.AlphaTestObjects, true);
    DrawRenderItems(sortedSceneObjects.TransparentObjects, true);

    // Restore main pass constants and stencil ref.
    mCommandList->SetGraphicsRootConstantBufferView(2, passCB->GetGPUVirtualAddress());
    mCommandList->OMSetStencilRef(0);

    // Draw mirror with transparency so reflection blends through.
    mCommandList->SetPipelineState(mPSOs["transparent"].Get());
    DrawRenderItems(sortedSceneObjects.MirrorObjects, false);

    // Draw shadows
    mCommandList->SetPipelineState(mPSOs["shadow"].Get());
    DrawShadows(sortedSceneObjects.Models);

    if (bIsSobelEnabled)
    {
        // Change offscreen texture to be used as an input.
        auto ResBarOffScreeRTRTtoGR = CD3DX12_RESOURCE_BARRIER::Transition(mOffscreenRT->Resource(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
        mCommandList->ResourceBarrier(1, &ResBarOffScreeRTRTtoGR);

        mSobelFilter->Execute(mCommandList.Get(), mPostProcessRootSignature.Get(), mPSOs["sobel"].Get(), mOffscreenRT->Srv());

        // Switching back to back buffer rendering.
        // Indicate a state transition on the resource usage.
        const auto ResBarrPresentToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
        mCommandList->ResourceBarrier(1, &ResBarrPresentToRenderTarget);

        // Specify the buffers we are going to render to.
        mCommandList->OMSetRenderTargets(1, &CurBackBuferView, true, &DSV);

        mCommandList->SetGraphicsRootSignature(mPostProcessRootSignature.Get());
        mCommandList->SetPipelineState(mPSOs["composite"].Get());
        mCommandList->SetGraphicsRootDescriptorTable(0, mOffscreenRT->Srv());
        mCommandList->SetGraphicsRootDescriptorTable(1, mSobelFilter->OutputSrv());
        DrawFullscreenQuad(mCommandList.Get());
    }
    
    if (bIsBlurEnabled)
    {
         mBilateralFilter->Execute(mCommandList.Get(), mPostBilateralRootSignature.Get(), mPSOs["horzBilateral"].Get(), mPSOs["vertBilateral"].Get(), CurrentBackBuffer(),
            mBlurCount, mBlurSigma, mBlurWeights);

         //Prepare to copy blurred output to the back buffer.
         auto ResBarCurBufCopeSRCtoCopeDest = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_SOURCE, D3D12_RESOURCE_STATE_COPY_DEST);
         mCommandList->ResourceBarrier(1, &ResBarCurBufCopeSRCtoCopeDest);

         mCommandList->CopyResource(CurrentBackBuffer(), mBilateralFilter->Output());

         auto ResBar1488 = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_RENDER_TARGET);
         mCommandList->ResourceBarrier(1, &ResBar1488);
    }
}

void Graphic::EndDrawFrame()
{
#if defined PIXPROFILE
    PIXScopedEvent(PIX_COLOR(50, 50, 150), L"EndDrawFrame");
#endif
    // Indicate a state transition on the resource usage.
    const auto ResBarrRenderTargetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(CurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    mCommandList->ResourceBarrier(1, &ResBarrRenderTargetToPresent);

    // Done recording commands.
    mCommandList->Close() >> Check;

    // Add the command list to the queue for execution.
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Swap the back and front buffers
    mSwapChain->Present(0, DXGI_PRESENT_ALLOW_TEARING) >> Check;  // vsync off
    // mSwapChain->Present(1, 0) >> Check;  // vsync on
    mCurrBackBuffer = (mCurrBackBuffer + 1) % mSwapChainBufferCount;

    // Advance the fence value to mark commands up to this fence point.
    mCurrFrameResource->Fence = ++mCurrentFenceValue;

    // Add an instruction to the command queue to set a new fence point.
    // Because we are on the GPU timeline, the new fence point won't be
    // set until the GPU finishes processing all the commands prior to this Signal().
    mCommandQueue->Signal(mFence.Get(), mCurrentFenceValue);
}

void Graphic::Update(DirectX::FXMMATRIX ViewMat, DirectX::XMFLOAT3 CameraPos, const GameTimerW* gt, const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects,
    WavesSceneObject* waveObject, std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials)
{
#if defined PIXPROFILE
    PIXScopedEvent(PIX_COLOR(60, 180, 60), L"Update Buffers");
#endif

    // Update Camera
    DirectX::XMStoreFloat4x4(&mView, ViewMat);
    mEyePos = CameraPos;

    // Cycle through the circular frame resource array.
    mCurrFrameResourceIndex = (mCurrFrameResourceIndex + 1) % gNumFrameResources;
    mCurrFrameResource = mFrameResources[mCurrFrameResourceIndex].get();

    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    if (mCurrFrameResource->Fence != 0 && mFence->GetCompletedValue() < mCurrFrameResource->Fence)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
        mFence->SetEventOnCompletion(mCurrFrameResource->Fence, eventHandle) >> Check;
        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }

    UpdateObjectCBs(sceneObjects);
    UpdateMaterialCBs(materials);
    UpdateWavesMesh(gt, waveObject);
    UpdateMainPassCB(gt);
    UpdateReflectedPassCB();
}

void Graphic::SetWireframe(bool state)
{
    if (state != bIsWireframe)
    {
        bIsWireframe = state;
    }
}

void Graphic::SetSobel(bool state)
{
    if (state != bIsSobelEnabled)
    {
        bIsSobelEnabled = state;
    }
}

void Graphic::SetBlur(bool state)
{
    if (state != bIsBlurEnabled)
    {
        bIsBlurEnabled = state;
    }
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
    ComPtr<ID3D12Debug3> debugInterface;
    D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface)) >> Check;
    debugInterface->EnableDebugLayer();
#endif

    // create FACTORY
    UINT CreateFactoryFlags = 0;
#if defined(_DEBUG)
    CreateFactoryFlags = DXGI_CREATE_FACTORY_DEBUG;
#endif
    CreateDXGIFactory2(CreateFactoryFlags, IID_PPV_ARGS(&mFactory)) >> Check;

    // Create DEVICE
    //
    D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0, D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0};

    // try create hardware device
    HRESULT hr = D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_2, IID_PPV_ARGS(&mDevice));

    if (FAILED(hr))
    {
        for (const auto& fLevel : featureLevels)
        {
            hr = D3D12CreateDevice(nullptr, fLevel, IID_PPV_ARGS(&mDevice));
            if (SUCCEEDED(hr))
            {
                break;
            }
        }
        // if create hardware device is failed, try create WARP device
        if (FAILED(hr))
        {
            ComPtr<IDXGIAdapter> pWarpAdapter;
            mFactory->EnumWarpAdapter(IID_PPV_ARGS(&pWarpAdapter)) >> Check;
            D3D12CreateDevice(pWarpAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&mDevice)) >> Check;
        }
    }
    assert(mDevice);

#if defined(_DEBUG)
    ComPtr<ID3D12InfoQueue> pInfoQueue = nullptr;
    mDevice->QueryInterface(IID_PPV_ARGS(&pInfoQueue));
    if (pInfoQueue)
    {
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
        pInfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);
    }
#endif

    // Create Fence
    mDevice->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)) >> Check;
    assert(mFence);

    // Set Descriptors Size
    mRtvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvUavDescriptorSize = mDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    assert(mRtvDescriptorSize);
    assert(mDsvDescriptorSize);
    assert(mCbvSrvUavDescriptorSize);

    // Create Command Objects
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.NodeMask = 0;
    mDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)) >> Check;
    mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAlloc.GetAddressOf())) >> Check;
    mDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAlloc.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >> Check;
    mCommandList->Close();
    assert(mCommandQueue);
    assert(mCommandAlloc);
    assert(mCommandList);

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
    assert(mSwapChain);

    // Create RTV Descriptor Heap
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc{};
    rtvHeapDesc.NumDescriptors = mSwapChainBufferCount + 1;
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0;
    mDevice->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(mRtvHeap.GetAddressOf())) >> Check;
    assert(mRtvHeap);

    // Create DSV Descriptor Heap
    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc{};
    dsvHeapDesc.NumDescriptors = 1;
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0;
    mDevice->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(mDsvHeap.GetAddressOf())) >> Check;
    assert(mDsvHeap);

    // Create RTV for SWAPCHAIN
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(mRtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < mSwapChainBufferCount; i++)
    {
        mSwapChain->GetBuffer(i, IID_PPV_ARGS(&mSwapChainBuffer[i])) >> Check;
        mDevice->CreateRenderTargetView(mSwapChainBuffer[i].Get(), nullptr, rtvHeapHandle);

        std::wstring Name = L"SwapChainBuffer " + i;
        mSwapChainBuffer[i]->SetName(Name.c_str());
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

    mDevice->CreateCommittedResource(
        &HeapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc, D3D12_RESOURCE_STATE_COMMON, &optClear, IID_PPV_ARGS(mDepthStencilBuffer.GetAddressOf())) >>
        Check;

    mDepthStencilBuffer->SetName(L"DepthStencilBuffer") >> Check;

    mDevice->CreateDepthStencilView(mDepthStencilBuffer.Get(), nullptr, GetDepthStencilView());

    auto ResBar = CD3DX12_RESOURCE_BARRIER::Transition(mDepthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);

    // Reset the command list to prep for initialization commands.
    mCommandList->Reset(mCommandAlloc.Get(), nullptr) >> Check;

    mCommandList->ResourceBarrier(1, &ResBar);

    assert(mDepthStencilBuffer);

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

    // Init Projection matrix
    XMMATRIX P = XMMatrixPerspectiveFovLH(0.25f * MathHelper::Pi, GetAspectRatio(), 1.0f, 100000.0f);
    XMStoreFloat4x4(&mProj, P);

    // Execute the initialization commands.
    mCommandList->Close() >> Check;
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();
}

void Graphic::InitResources(size_t sceneObjectCount, size_t wavesVertCount, size_t materialsCount, std::unordered_map<EMaterialType, std::unique_ptr<Texture>>& textures)
{
    // Reset the command list to prep for initialization commands.
    mCommandList->Reset(mCommandAlloc.Get(), nullptr) >> Check;

    mBilateralFilter = std::make_unique<BlurFilter>(mDevice.Get(), mClientWidth, mClientHeight, mBackBufferFormat);
    mSobelFilter = std::make_unique<SobelFilter>(mDevice.Get(), mClientWidth, mClientHeight, mBackBufferFormat);
    mOffscreenRT = std::make_unique<RenderTarget>(mDevice.Get(), mClientWidth, mClientHeight, mBackBufferFormat);

    mSceneObjectCount = sceneObjectCount;
    mWavesVerticesCount = wavesVertCount;
    mMaterialCount = materialsCount;

    BuildRootSignature();
    BuildPostProcessRootSignature();
    BuildBilateralRootSignature();
    BuildDescriptorHeaps(textures);
    BuildShadersAndInputLayout();
    BuildFrameResources();

    BuildPSOs();

    // Execute the initialization commands.
    mCommandList->Close() >> Check;
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    // Wait until initialization is complete.
    FlushCommandQueue();
}

void Graphic::FlushCommandQueue()
{
    ++mCurrentFenceValue;
    mCommandQueue->Signal(mFence.Get(), mCurrentFenceValue) >> Check;

    if (mFence->GetCompletedValue() < mCurrentFenceValue)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(mCurrentFenceValue, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

void Graphic::UpdateObjectCBs(const std::vector<std::unique_ptr<BaseSceneObject>>& sceneObjects)
{
    auto currObjectCB = mCurrFrameResource->ObjectCB.get();
    for (auto& scObj : sceneObjects)
    {
        // Only update the cbuffer data if the constants have changed.
        // This needs to be tracked per frame resource.
        if (scObj->GetNumFramesDirty() > 0)
        {
            ObjectConstants objConstants;
            XMStoreFloat4x4(&objConstants.World, XMMatrixTranspose(scObj->GetWorldMatrix()));

            XMMATRIX texTransform = XMLoadFloat4x4(&scObj->GetRenderItem()->TexTransform);
            XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(texTransform));

            auto WorldDeterminant = DirectX::XMMatrixDeterminant(scObj->GetWorldMatrix());
            XMMATRIX WorldInverse = DirectX::XMMatrixInverse(&WorldDeterminant, scObj->GetWorldMatrix());
            XMStoreFloat4x4(&objConstants.WorldInvTranspose, XMMatrixTranspose(WorldInverse));

            currObjectCB->CopyData(scObj->GetRenderItem()->ObjCBIndex, objConstants);

            ObjectConstants objConstantsReflected = objConstants;
            XMStoreFloat4x4(&objConstantsReflected.World, XMMatrixTranspose(DirectX::XMLoadFloat4x4(&scObj->GetMatrixReflectedObject())));
            currObjectCB->CopyData(scObj->GetReflectedObjCBIndex(), objConstantsReflected);

            ObjectConstants objConstantsShadows = objConstants;

            XMStoreFloat4x4(&objConstantsShadows.World, XMMatrixTranspose(DirectX::XMLoadFloat4x4(&scObj->GetMatrixObjectShadow())));
            currObjectCB->CopyData(scObj->GetObjCBIndexShadow(), objConstantsShadows);

            scObj->DecrementNumFrameDirty();
        }
    }
}

void Graphic::UpdateMaterialCBs(std::unordered_map<EMaterialType, std::unique_ptr<Material>>& materials)
{
    auto currMaterialCB = mCurrFrameResource->MaterialCB.get();
    for (auto& e : materials)
    {
        // Only update the cbuffer data if the constants have changed.  If the cbuffer
        // data changes, it needs to be updated for each FrameResource.
        Material* mat = e.second.get();
        if (mat->NumFramesDirty > 0)
        {
            XMMATRIX matTransform = XMLoadFloat4x4(&mat->MatTransform);

            MaterialConstants matConstants;
            matConstants.DiffuseAlbedo = mat->DiffuseAlbedo;
            matConstants.FresnelR0 = mat->FresnelR0;
            matConstants.Roughness = mat->Roughness;
            XMStoreFloat4x4(&matConstants.MatTransform, XMMatrixTranspose(matTransform));

            currMaterialCB->CopyData(mat->MatCBIndex, matConstants);

            // Next FrameResource need to be updated too.
            mat->NumFramesDirty--;
        }
    }
}

void Graphic::UpdateMainPassCB(const GameTimerW* gt)
{
    XMMATRIX view = XMLoadFloat4x4(&mView);
    XMMATRIX proj = XMLoadFloat4x4(&mProj);
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    auto ViewDet = XMMatrixDeterminant(view);
    auto ProjDet = XMMatrixDeterminant(proj);
    auto ViewProjDet = XMMatrixDeterminant(viewProj);

    XMMATRIX invView = XMMatrixInverse(&ViewDet, view);
    XMMATRIX invProj = XMMatrixInverse(&ProjDet, proj);
    XMMATRIX invViewProj = XMMatrixInverse(&ViewProjDet, viewProj);

    XMStoreFloat4x4(&mMainPassCB.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&mMainPassCB.InvView, XMMatrixTranspose(invView));
    XMStoreFloat4x4(&mMainPassCB.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&mMainPassCB.InvProj, XMMatrixTranspose(invProj));
    XMStoreFloat4x4(&mMainPassCB.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&mMainPassCB.InvViewProj, XMMatrixTranspose(invViewProj));
    mMainPassCB.EyePosW = mEyePos;
    mMainPassCB.RenderTargetSize = XMFLOAT2((float)mClientWidth, (float)mClientHeight);
    mMainPassCB.InvRenderTargetSize = XMFLOAT2(1.0f / mClientWidth, 1.0f / mClientHeight);
    mMainPassCB.NearZ = 1.0f;
    mMainPassCB.FarZ = 1000.0f;
    mMainPassCB.TotalTime = gt->GetTotalTime();
    mMainPassCB.DeltaTime = gt->GetDeltaTime();
    mMainPassCB.AmbientLight = mAmbientLight;
    mMainPassCB.Lights[0].Direction = mLightsDirection;
    mMainPassCB.Lights[0].Strength = mLightsStrength;
    mMainPassCB.Lights[1].Direction = {-0.57735f, -0.57735f, 0.57735f};
    mMainPassCB.Lights[1].Strength = {0.3f, 0.3f, 0.3f};
    mMainPassCB.Lights[2].Direction = {0.0f, -0.707f, -0.707f};
    mMainPassCB.Lights[2].Strength = {0.15f, 0.15f, 0.15f};

    auto currPassCB = mCurrFrameResource->PassCB.get();
    currPassCB->CopyData(0, mMainPassCB);
}

void Graphic::UpdateReflectedPassCB()
{
    mReflectedPassCB = mMainPassCB;

    XMVECTOR mirrorPlane = XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);  // xy plane
    XMMATRIX R = XMMatrixReflect(mirrorPlane);

    // Reflect the lighting.
    for (int i = 0; i < 1; ++i)
    {
        XMVECTOR lightDir = XMLoadFloat3(&mMainPassCB.Lights[i].Direction);
        XMVECTOR reflectedLightDir = XMVector3TransformNormal(lightDir, R);
        XMStoreFloat3(&mReflectedPassCB.Lights[i].Direction, reflectedLightDir);
    }

    // Reflected pass stored in index 1
    auto currPassCB = mCurrFrameResource->PassCB.get();
    currPassCB->CopyData(1, mReflectedPassCB);
}

void Graphic::BuildDescriptorHeaps(std::unordered_map<EMaterialType, std::unique_ptr<Texture>>& textures)
{
    assert(!textures.empty());

    if (textures.empty()) return;

    //
    // Create the SRV heap.
    //
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    srvHeapDesc.NumDescriptors = (UINT)textures.size() + 64;
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    mDevice->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(&mCbvSrvUavDescriptorHeap)) >> Check;

    //
    // Fill out the heap with actual descriptors.
    //
    CD3DX12_CPU_DESCRIPTOR_HANDLE hDescriptor(mCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
    int index = 0;

    for (const auto& [type, TexPtr] : textures)
    {
        auto texRes = TexPtr->Resource;
        TexPtr->DiffuseSrvHeapIndex = index;
        TexPtr->NormalSrvHeapIndex = index;

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Format = texRes->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = texRes->GetDesc().MipLevels;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        if (type == EMaterialType::SKYBOX)
        {
            srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        }

        mDevice->CreateShaderResourceView(texRes.Get(), &srvDesc, hDescriptor);
        ++index;
        hDescriptor.Offset(1, mCbvSrvUavDescriptorSize);
    }

    auto srvCpuStart = mCbvSrvUavDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    auto srvGpuStart = mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

    int rtvOffset = mSwapChainBufferCount;
    auto rtvCpuStart = mRtvHeap->GetCPUDescriptorHandleForHeapStart();

    mSobelFilter->BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, index, mCbvSrvUavDescriptorSize),
        CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, index, mCbvSrvUavDescriptorSize), mCbvSrvUavDescriptorSize);

    mOffscreenRT->BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, index + mSobelFilter->DescriptorCount(), mCbvSrvUavDescriptorSize),
        CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, index + mSobelFilter->DescriptorCount(), mCbvSrvUavDescriptorSize),
        CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart, rtvOffset, mRtvDescriptorSize));

    mBilateralFilter->BuildDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, index + mSobelFilter->DescriptorCount() + 1, mCbvSrvUavDescriptorSize),
        CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, index + mSobelFilter->DescriptorCount() + 1, mCbvSrvUavDescriptorSize), mCbvSrvUavDescriptorSize);
}

void Graphic::BuildRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE texTable0;
    texTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE texTable1;
    texTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[5];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &texTable0, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[1].InitAsConstantBufferView(0);
    slotRootParameter[2].InitAsConstantBufferView(1);
    slotRootParameter[3].InitAsConstantBufferView(2);
    slotRootParameter[4].InitAsDescriptorTable(1, &texTable1, D3D12_SHADER_VISIBILITY_PIXEL);

    auto staticSamplers = GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(5, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    hr >> Check;

    mDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mRootSignature.GetAddressOf())) >> Check;
}

void Graphic::BuildPostProcessRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE srvTable0;
    srvTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE srvTable1;
    srvTable1.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);

    CD3DX12_DESCRIPTOR_RANGE uavTable0;
    uavTable0.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[3];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsDescriptorTable(1, &srvTable0);
    slotRootParameter[1].InitAsDescriptorTable(1, &srvTable1);
    slotRootParameter[2].InitAsDescriptorTable(1, &uavTable0);

    auto staticSamplers = GetStaticSamplers();

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, (UINT)staticSamplers.size(), staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    hr >> Check;

    mDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mPostProcessRootSignature.GetAddressOf())) >> Check;
}

void Graphic::BuildBilateralRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE srvTable;
    srvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

    CD3DX12_DESCRIPTOR_RANGE uavTable;
    uavTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[3];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[0].InitAsConstants(16, 0);
    slotRootParameter[1].InitAsDescriptorTable(1, &srvTable);
    slotRootParameter[2].InitAsDescriptorTable(1, &uavTable);

    // A root signature is an array of root parameters.
    CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(3, slotRootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    // create a root signature with a single slot which points to a descriptor range consisting of a single constant buffer
    ComPtr<ID3DBlob> serializedRootSig = nullptr;
    ComPtr<ID3DBlob> errorBlob = nullptr;
    HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

    if (errorBlob != nullptr)
    {
        ::OutputDebugStringA((char*)errorBlob->GetBufferPointer());
    }
    hr >> Check;

    mDevice->CreateRootSignature(0, serializedRootSig->GetBufferPointer(), serializedRootSig->GetBufferSize(), IID_PPV_ARGS(mPostBilateralRootSignature.GetAddressOf())) >> Check;
}

void Graphic::BuildShadersAndInputLayout()
{
    const D3D_SHADER_MACRO defines[] = {"FOG", "1", NULL, NULL};
    const D3D_SHADER_MACRO alphaTestDefines[] = {"FOG", "1", "ALPHA_TEST", "1", NULL, NULL};
    const D3D_SHADER_MACRO waveDefines[] = {"DISPLACEMENT_MAP", "1", NULL, NULL};

    mShaders["standardVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Default.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["opaquePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Default.hlsl", defines, "PS", "ps_5_1");
    mShaders["alphaTestedPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Default.hlsl", alphaTestDefines, "PS", "ps_5_1");

    mShaders["treeSpriteVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\ThreeSprite.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["treeSpriteGS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\ThreeSprite.hlsl", nullptr, "GS", "gs_5_1");
    mShaders["treeSpritePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\ThreeSprite.hlsl", alphaTestDefines, "PS", "ps_5_1");

    mShaders["geometrySubdivideVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\GeometrySubdivide.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["geometrySubdivideGS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\GeometrySubdivide.hlsl", nullptr, "GS", "gs_5_1");
    mShaders["geometrySubdividePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\GeometrySubdivide.hlsl", defines, "PS", "ps_5_1");

    mShaders["horzBlurCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Blur.hlsl", nullptr, "HorzBlurCS", "cs_5_1");
    mShaders["vertBlurCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Blur.hlsl", nullptr, "VertBlurCS", "cs_5_1");

    mShaders["horzBilateralCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\BilateralFilter.hlsl", nullptr, "HorzBilateralCS", "cs_5_1");
    mShaders["vertBilateralCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\BilateralFilter.hlsl", nullptr, "VertBilateralCS", "cs_5_1");

    mShaders["wavesVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Default.hlsl", waveDefines, "VS", "vs_5_1");
    mShaders["wavesUpdateCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\WaveSim.hlsl", nullptr, "UpdateWavesCS", "cs_5_1");
    mShaders["wavesDisturbCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\WaveSim.hlsl", nullptr, "DisturbWavesCS", "cs_5_1");

    mShaders["compositeVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Composite.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["compositePS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Composite.hlsl", nullptr, "PS", "ps_5_1");
    mShaders["sobelCS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Sobel.hlsl", nullptr, "SobelCS", "cs_5_1");

    mShaders["tessVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "HS", "hs_5_1");
    mShaders["tessDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "DS", "ds_5_1");
    mShaders["tessPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTri.hlsl", nullptr, "PS", "ps_5_1");

    mShaders["tessFracOddVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessFracOddHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "HS", "hs_5_1");
    mShaders["tessFracOddDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "DS", "ds_5_1");
    mShaders["tessFracOddPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracOdd.hlsl", nullptr, "PS", "ps_5_1");

    mShaders["tessFracEvenVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessFracEvenHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "HS", "hs_5_1");
    mShaders["tessFracEvenDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "DS", "ds_5_1");
    mShaders["tessFracEvenPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriFracEven.hlsl", nullptr, "PS", "ps_5_1");

    mShaders["tessPowVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["tessPowHS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "HS", "hs_5_1");
    mShaders["tessPowDS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "DS", "ds_5_1");
    mShaders["tessPowPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\TessellationTriPow.hlsl", nullptr, "PS", "ps_5_1");

    mShaders["skyVS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Sky.hlsl", nullptr, "VS", "vs_5_1");
    mShaders["skyPS"] = D3D12Utils::CompileShader(L"..\\Source\\Shaders\\Sky.hlsl", nullptr, "PS", "ps_5_1");

    mStdInputLayout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };

    mTreeSpriteInputLayout = {
        {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
        {"SIZE", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
    };
}

void Graphic::BuildPSOs()
{
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaquePsoDesc;

    //
    // PSO for opaque objects.
    //
    ZeroMemory(&opaquePsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    opaquePsoDesc.InputLayout = {mStdInputLayout.data(), (UINT)mStdInputLayout.size()};
    opaquePsoDesc.pRootSignature = mRootSignature.Get();
    opaquePsoDesc.VS = {reinterpret_cast<BYTE*>(mShaders["standardVS"]->GetBufferPointer()), mShaders["standardVS"]->GetBufferSize()};
    opaquePsoDesc.PS = {reinterpret_cast<BYTE*>(mShaders["opaquePS"]->GetBufferPointer()), mShaders["opaquePS"]->GetBufferSize()};
    opaquePsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    opaquePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
    opaquePsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
    opaquePsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    opaquePsoDesc.SampleMask = UINT_MAX;
    opaquePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    opaquePsoDesc.NumRenderTargets = 1;
    opaquePsoDesc.RTVFormats[0] = mBackBufferFormat;
    opaquePsoDesc.SampleDesc.Count = 1;
    opaquePsoDesc.SampleDesc.Quality = 0;
    opaquePsoDesc.DSVFormat = mDepthStencilFormat;
    mDevice->CreateGraphicsPipelineState(&opaquePsoDesc, IID_PPV_ARGS(&mPSOs["opaque"])) >> Check;

    //
    // PSO for opaque wireframe objects.
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframePsoDesc = opaquePsoDesc;
    opaqueWireframePsoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    mDevice->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["opaque_wireframe"])) >> Check;

    //
    // PSO for transparent objects
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC transparentPsoDesc = opaquePsoDesc;

    D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc;
    transparencyBlendDesc.BlendEnable = true;
    transparencyBlendDesc.LogicOpEnable = false;
    transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
    transparencyBlendDesc.DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
    transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ZERO;
    transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    transparencyBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
    transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    transparentPsoDesc.BlendState.RenderTarget[0] = transparencyBlendDesc;
    mDevice->CreateGraphicsPipelineState(&transparentPsoDesc, IID_PPV_ARGS(&mPSOs["transparent"])) >> Check;

    //
    // PSO for alpha tested objects
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC alphaTestedPsoDesc = opaquePsoDesc;
    alphaTestedPsoDesc.PS = {reinterpret_cast<BYTE*>(mShaders["alphaTestedPS"]->GetBufferPointer()), mShaders["alphaTestedPS"]->GetBufferSize()};
    alphaTestedPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    mDevice->CreateGraphicsPipelineState(&alphaTestedPsoDesc, IID_PPV_ARGS(&mPSOs["alphaTested"])) >> Check;

    //
    // PSO for marking stencil mirrors.
    //
    CD3DX12_BLEND_DESC mirrorBlendState(D3D12_DEFAULT);
    mirrorBlendState.RenderTarget[0].RenderTargetWriteMask = 0;

    D3D12_DEPTH_STENCIL_DESC mirrorDSS;
    mirrorDSS.DepthEnable = true;
    mirrorDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    mirrorDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    mirrorDSS.StencilEnable = true;
    mirrorDSS.StencilReadMask = 0xff;
    mirrorDSS.StencilWriteMask = 0xff;

    mirrorDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    mirrorDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    mirrorDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
    mirrorDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    // We are not rendering backfacing polygons, so these settings do not matter.
    mirrorDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    mirrorDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    mirrorDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
    mirrorDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC markMirrorsPsoDesc = opaquePsoDesc;
    markMirrorsPsoDesc.BlendState = mirrorBlendState;
    markMirrorsPsoDesc.DepthStencilState = mirrorDSS;
    mDevice->CreateGraphicsPipelineState(&markMirrorsPsoDesc, IID_PPV_ARGS(&mPSOs["markStencilMirrors"])) >> Check;

    //
    // PSO for stencil reflections.
    //
    D3D12_DEPTH_STENCIL_DESC reflectionsDSS;
    reflectionsDSS.DepthEnable = true;
    reflectionsDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    reflectionsDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    reflectionsDSS.StencilEnable = true;
    reflectionsDSS.StencilReadMask = 0xff;
    reflectionsDSS.StencilWriteMask = 0xff;

    reflectionsDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

    // We are not rendering backfacing polygons, so these settings do not matter.
    reflectionsDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    reflectionsDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC drawReflectionsPsoDesc = opaquePsoDesc;
    drawReflectionsPsoDesc.DepthStencilState = reflectionsDSS;
    drawReflectionsPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    drawReflectionsPsoDesc.RasterizerState.FrontCounterClockwise = true;
    mDevice->CreateGraphicsPipelineState(&drawReflectionsPsoDesc, IID_PPV_ARGS(&mPSOs["drawStencilReflections"])) >> Check;

    //
    // PSO for shadow objects
    //
    // We are going to draw shadows with transparency, so base it off the transparency description.
    D3D12_DEPTH_STENCIL_DESC shadowDSS;
    shadowDSS.DepthEnable = true;
    shadowDSS.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
    shadowDSS.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
    shadowDSS.StencilEnable = true;
    shadowDSS.StencilReadMask = 0xff;
    shadowDSS.StencilWriteMask = 0xff;

    shadowDSS.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    shadowDSS.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    shadowDSS.FrontFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
    shadowDSS.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

    // We are not rendering backfacing polygons, so these settings do not matter.
    shadowDSS.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    shadowDSS.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    shadowDSS.BackFace.StencilPassOp = D3D12_STENCIL_OP_INCR;
    shadowDSS.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC shadowPsoDesc = transparentPsoDesc;
    shadowPsoDesc.DepthStencilState = shadowDSS;
    mDevice->CreateGraphicsPipelineState(&shadowPsoDesc, IID_PPV_ARGS(&mPSOs["shadow"])) >> Check;

    //
    // PSO for tree sprites
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC treeSpritePsoDesc = opaquePsoDesc;
    treeSpritePsoDesc.VS = {reinterpret_cast<BYTE*>(mShaders["treeSpriteVS"]->GetBufferPointer()), mShaders["treeSpriteVS"]->GetBufferSize()};
    treeSpritePsoDesc.GS = {reinterpret_cast<BYTE*>(mShaders["treeSpriteGS"]->GetBufferPointer()), mShaders["treeSpriteGS"]->GetBufferSize()};
    treeSpritePsoDesc.PS = {reinterpret_cast<BYTE*>(mShaders["treeSpritePS"]->GetBufferPointer()), mShaders["treeSpritePS"]->GetBufferSize()};
    treeSpritePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT;
    treeSpritePsoDesc.InputLayout = {mStdInputLayout.data(), (UINT)mStdInputLayout.size()};
    treeSpritePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    mDevice->CreateGraphicsPipelineState(&treeSpritePsoDesc, IID_PPV_ARGS(&mPSOs["treeSprites"])) >> Check;

    //
    // PSO for subdivide shape
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GeometrySubdividePsoDesc = opaquePsoDesc;
    GeometrySubdividePsoDesc.VS = {reinterpret_cast<BYTE*>(mShaders["geometrySubdivideVS"]->GetBufferPointer()), mShaders["geometrySubdivideVS"]->GetBufferSize()};
    GeometrySubdividePsoDesc.GS = {reinterpret_cast<BYTE*>(mShaders["geometrySubdivideGS"]->GetBufferPointer()), mShaders["geometrySubdivideGS"]->GetBufferSize()};
    GeometrySubdividePsoDesc.PS = {reinterpret_cast<BYTE*>(mShaders["geometrySubdividePS"]->GetBufferPointer()), mShaders["geometrySubdividePS"]->GetBufferSize()};
    GeometrySubdividePsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    GeometrySubdividePsoDesc.InputLayout = {mStdInputLayout.data(), (UINT)mStdInputLayout.size()};
    GeometrySubdividePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    mDevice->CreateGraphicsPipelineState(&GeometrySubdividePsoDesc, IID_PPV_ARGS(&mPSOs["geometrySubdivide"])) >> Check;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC GeometrySubdividePsoDescWireFrame = GeometrySubdividePsoDesc;
    GeometrySubdividePsoDescWireFrame.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    mDevice->CreateGraphicsPipelineState(&opaqueWireframePsoDesc, IID_PPV_ARGS(&mPSOs["geometrySubdivideWireFrame"])) >> Check;

    //
    // PSO for horizontal blur
    //
    D3D12_COMPUTE_PIPELINE_STATE_DESC horzBlurPSO = {};
    horzBlurPSO.pRootSignature = mPostBilateralRootSignature.Get();
    horzBlurPSO.CS = {reinterpret_cast<BYTE*>(mShaders["horzBilateralCS"]->GetBufferPointer()), mShaders["horzBilateralCS"]->GetBufferSize()};
    horzBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    mDevice->CreateComputePipelineState(&horzBlurPSO, IID_PPV_ARGS(&mPSOs["horzBilateral"])) >> Check;

    //
    // PSO for vertical blur
    //
    D3D12_COMPUTE_PIPELINE_STATE_DESC vertBlurPSO = {};
    vertBlurPSO.pRootSignature = mPostBilateralRootSignature.Get();
    vertBlurPSO.CS = {reinterpret_cast<BYTE*>(mShaders["vertBilateralCS"]->GetBufferPointer()), mShaders["vertBilateralCS"]->GetBufferSize()};
    vertBlurPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    mDevice->CreateComputePipelineState(&vertBlurPSO, IID_PPV_ARGS(&mPSOs["vertBilateral"])) >> Check;

    //
    // PSO for drawing waves
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC wavesRenderPSO = transparentPsoDesc;
    wavesRenderPSO.VS = {reinterpret_cast<BYTE*>(mShaders["wavesVS"]->GetBufferPointer()), mShaders["wavesVS"]->GetBufferSize()};
    mDevice->CreateGraphicsPipelineState(&wavesRenderPSO, IID_PPV_ARGS(&mPSOs["wavesRender"])) >> Check;

    //
    // PSO for compositing post process
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC compositePSO = opaquePsoDesc;
    compositePSO.pRootSignature = mPostProcessRootSignature.Get();

    // Disable depth test.
    compositePSO.DepthStencilState.DepthEnable = false;
    compositePSO.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    compositePSO.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
    compositePSO.InputLayout = {nullptr, 0};
    compositePSO.VS = {reinterpret_cast<BYTE*>(mShaders["compositeVS"]->GetBufferPointer()), mShaders["compositeVS"]->GetBufferSize()};
    compositePSO.PS = {reinterpret_cast<BYTE*>(mShaders["compositePS"]->GetBufferPointer()), mShaders["compositePS"]->GetBufferSize()};
    mDevice->CreateGraphicsPipelineState(&compositePSO, IID_PPV_ARGS(&mPSOs["composite"])) >> Check;

    //
    // PSO for sobel
    //
    D3D12_COMPUTE_PIPELINE_STATE_DESC sobelPSO = {};
    sobelPSO.pRootSignature = mPostProcessRootSignature.Get();
    sobelPSO.CS = {reinterpret_cast<BYTE*>(mShaders["sobelCS"]->GetBufferPointer()), mShaders["sobelCS"]->GetBufferSize()};
    sobelPSO.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
    mDevice->CreateComputePipelineState(&sobelPSO, IID_PPV_ARGS(&mPSOs["sobel"])) >> Check;

    //
    // PSO for Tesselation
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationPSO = opaquePsoDesc;
    tesselationPSO.VS = {reinterpret_cast<BYTE*>(mShaders["tessVS"]->GetBufferPointer()), mShaders["tessVS"]->GetBufferSize()};
    tesselationPSO.HS = {reinterpret_cast<BYTE*>(mShaders["tessHS"]->GetBufferPointer()), mShaders["tessHS"]->GetBufferSize()};
    tesselationPSO.DS = {reinterpret_cast<BYTE*>(mShaders["tessDS"]->GetBufferPointer()), mShaders["tessDS"]->GetBufferSize()};
    tesselationPSO.PS = {reinterpret_cast<BYTE*>(mShaders["tessPS"]->GetBufferPointer()), mShaders["tessPS"]->GetBufferSize()};
    tesselationPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    mDevice->CreateGraphicsPipelineState(&tesselationPSO, IID_PPV_ARGS(&mPSOs["tess"])) >> Check;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationWireFramePSO = tesselationPSO;
    tesselationWireFramePSO.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    mDevice->CreateGraphicsPipelineState(&tesselationWireFramePSO, IID_PPV_ARGS(&mPSOs["tessWireframe"])) >> Check;

    // Tesselation Fractional ODD
    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationPSOFracOdd = tesselationPSO;
    tesselationPSOFracOdd.VS = {reinterpret_cast<BYTE*>(mShaders["tessFracOddVS"]->GetBufferPointer()), mShaders["tessFracOddVS"]->GetBufferSize()};
    tesselationPSOFracOdd.HS = {reinterpret_cast<BYTE*>(mShaders["tessFracOddHS"]->GetBufferPointer()), mShaders["tessFracOddHS"]->GetBufferSize()};
    tesselationPSOFracOdd.DS = {reinterpret_cast<BYTE*>(mShaders["tessFracOddDS"]->GetBufferPointer()), mShaders["tessFracOddDS"]->GetBufferSize()};
    tesselationPSOFracOdd.PS = {reinterpret_cast<BYTE*>(mShaders["tessFracOddPS"]->GetBufferPointer()), mShaders["tessFracOddPS"]->GetBufferSize()};
    tesselationPSOFracOdd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    mDevice->CreateGraphicsPipelineState(&tesselationPSOFracOdd, IID_PPV_ARGS(&mPSOs["tessFracOdd"])) >> Check;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationPSOFracOddWireFrame = tesselationPSOFracOdd;
    tesselationPSOFracOddWireFrame.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    mDevice->CreateGraphicsPipelineState(&tesselationPSOFracOddWireFrame, IID_PPV_ARGS(&mPSOs["tessFracOddWireframe"])) >> Check;

    // Tesselation Fractional Even
    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationPSOFracEven = tesselationPSO;
    tesselationPSOFracEven.VS = {reinterpret_cast<BYTE*>(mShaders["tessFracEvenVS"]->GetBufferPointer()), mShaders["tessFracEvenVS"]->GetBufferSize()};
    tesselationPSOFracEven.HS = {reinterpret_cast<BYTE*>(mShaders["tessFracEvenHS"]->GetBufferPointer()), mShaders["tessFracEvenHS"]->GetBufferSize()};
    tesselationPSOFracEven.DS = {reinterpret_cast<BYTE*>(mShaders["tessFracEvenDS"]->GetBufferPointer()), mShaders["tessFracEvenDS"]->GetBufferSize()};
    tesselationPSOFracEven.PS = {reinterpret_cast<BYTE*>(mShaders["tessFracEvenPS"]->GetBufferPointer()), mShaders["tessFracEvenPS"]->GetBufferSize()};
    tesselationPSOFracEven.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    mDevice->CreateGraphicsPipelineState(&tesselationPSOFracEven, IID_PPV_ARGS(&mPSOs["tessFracEven"])) >> Check;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationPSOFracEvenWireFrame = tesselationPSOFracEven;
    tesselationPSOFracEvenWireFrame.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    mDevice->CreateGraphicsPipelineState(&tesselationPSOFracEvenWireFrame, IID_PPV_ARGS(&mPSOs["tessFracEvenWireframe"])) >> Check;

    // Tesselation Pow2
    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationPSOPow = tesselationPSO;
    tesselationPSOPow.VS = {reinterpret_cast<BYTE*>(mShaders["tessPowVS"]->GetBufferPointer()), mShaders["tessPowVS"]->GetBufferSize()};
    tesselationPSOPow.HS = {reinterpret_cast<BYTE*>(mShaders["tessPowHS"]->GetBufferPointer()), mShaders["tessPowHS"]->GetBufferSize()};
    tesselationPSOPow.DS = {reinterpret_cast<BYTE*>(mShaders["tessPowDS"]->GetBufferPointer()), mShaders["tessPowDS"]->GetBufferSize()};
    tesselationPSOPow.PS = {reinterpret_cast<BYTE*>(mShaders["tessPowPS"]->GetBufferPointer()), mShaders["tessPowPS"]->GetBufferSize()};
    tesselationPSOPow.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_PATCH;
    mDevice->CreateGraphicsPipelineState(&tesselationPSOPow, IID_PPV_ARGS(&mPSOs["tessPow"])) >> Check;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC tesselationPSOPowWireFrame = tesselationPSOPow;
    tesselationPSOPowWireFrame.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    mDevice->CreateGraphicsPipelineState(&tesselationPSOPowWireFrame, IID_PPV_ARGS(&mPSOs["tessPowWireframe"])) >> Check;

    //
    // PSO for sky.
    //
    D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = opaquePsoDesc;

    // The camera is inside the sky sphere, so just turn off culling.
    skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

    // Make sure the depth function is LESS_EQUAL and not just LESS.
    // Otherwise, the normalized depth values at z = 1 (NDC) will
    // fail the depth test if the depth buffer was cleared to 1.
    skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    skyPsoDesc.pRootSignature = mRootSignature.Get();
    skyPsoDesc.VS = {reinterpret_cast<BYTE*>(mShaders["skyVS"]->GetBufferPointer()), mShaders["skyVS"]->GetBufferSize()};
    skyPsoDesc.PS = {reinterpret_cast<BYTE*>(mShaders["skyPS"]->GetBufferPointer()), mShaders["skyPS"]->GetBufferSize()};
    mDevice->CreateGraphicsPipelineState(&skyPsoDesc, IID_PPV_ARGS(&mPSOs["sky"])) >> Check;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDescwireframe = skyPsoDesc;
    skyPsoDescwireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    mDevice->CreateGraphicsPipelineState(&skyPsoDescwireframe, IID_PPV_ARGS(&mPSOs["skyWireframe"])) >> Check;
}

void Graphic::BuildFrameResources()
{
    for (int i = 0; i < gNumFrameResources; ++i)
    {
        mFrameResources.push_back(std::make_unique<FrameResource>(mDevice.Get(), 2, (UINT)mSceneObjectCount, (UINT)mWavesVerticesCount, (UINT)mMaterialCount));
    }
}

void Graphic::DrawRenderItems(const std::vector<BaseSceneObject*>& sceneObjects, bool isReflectedObjects)
{
    UINT objCBByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));
    UINT matCBByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(MaterialConstants));

    auto objectCB = mCurrFrameResource->ObjectCB->Resource();
    auto matCB = mCurrFrameResource->MaterialCB->Resource();

    for (size_t i = 0; i < sceneObjects.size(); ++i)
    {
        auto ri = sceneObjects[i]->GetRenderItem();

        auto VBView = ri->Geo->VertexBufferView();
        auto IBView = ri->Geo->IndexBufferView();

        mCommandList->IASetVertexBuffers(0, 1, &VBView);
        mCommandList->IASetIndexBuffer(&IBView);
        mCommandList->IASetPrimitiveTopology(ri->PrimitiveType);

        CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        tex.Offset(ri->Mat->Tex->DiffuseSrvHeapIndex, mCbvSrvUavDescriptorSize);

        auto objCBIndex = isReflectedObjects ? sceneObjects[i]->GetReflectedObjCBIndex() * objCBByteSize : ri->ObjCBIndex * objCBByteSize;

        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + objCBIndex;
        D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + ri->Mat->MatCBIndex * matCBByteSize;

        mCommandList->SetGraphicsRootDescriptorTable(4, tex);
        mCommandList->SetGraphicsRootConstantBufferView(1, objCBAddress);
        mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);
        mCommandList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
    }
}

void Graphic::DrawShadows(const std::vector<BaseSceneObject*>& sceneObjects)
{
    UINT objCBByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(ObjectConstants));
    UINT matCBByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(MaterialConstants));

    auto objectCB = mCurrFrameResource->ObjectCB->Resource();
    auto matCB = mCurrFrameResource->MaterialCB->Resource();

    for (size_t i = 0; i < sceneObjects.size(); ++i)
    {
        auto ri = sceneObjects[i]->GetRenderItem();

        auto VBView = ri->Geo->VertexBufferView();
        auto IBView = ri->Geo->IndexBufferView();

        mCommandList->IASetVertexBuffers(0, 1, &VBView);
        mCommandList->IASetIndexBuffer(&IBView);
        mCommandList->IASetPrimitiveTopology(ri->PrimitiveType);

        CD3DX12_GPU_DESCRIPTOR_HANDLE tex(mCbvSrvUavDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
        tex.Offset(ri->Mat->Tex->DiffuseSrvHeapIndex, mCbvSrvUavDescriptorSize);

        auto objCBIndex = sceneObjects[i]->GetObjCBIndexShadow() * objCBByteSize;

        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = objectCB->GetGPUVirtualAddress() + objCBIndex;
        D3D12_GPU_VIRTUAL_ADDRESS matCBAddress = matCB->GetGPUVirtualAddress() + sceneObjects[i]->GetShadowMaterial()->MatCBIndex * matCBByteSize;

        mCommandList->SetGraphicsRootDescriptorTable(4, tex);
        mCommandList->SetGraphicsRootConstantBufferView(1, objCBAddress);
        mCommandList->SetGraphicsRootConstantBufferView(3, matCBAddress);
        mCommandList->DrawIndexedInstanced(ri->IndexCount, 1, ri->StartIndexLocation, ri->BaseVertexLocation, 0);
    }
}

void Graphic::UpdateWavesMesh(const GameTimerW* gt, WavesSceneObject* waveObject)
{
    assert(gt);
    assert(waveObject);
    assert(waveObject->GetWaves());
    // Every quarter second, generate a random wave.
    static float t_base = 0.0f;
    if ((gt->GetTotalTime() - t_base) >= 0.25f)
    {
        t_base += 0.25f;

        int i = MathHelper::Rand(4, waveObject->GetWaves()->RowCount() - 5);
        int j = MathHelper::Rand(4, waveObject->GetWaves()->ColumnCount() - 5);

        float r = MathHelper::RandF(0.2f, 0.5f);

        waveObject->GetWaves()->Disturb(i, j, r);
    }

    // Update the wave simulation.
    waveObject->GetWaves()->Update(gt->GetDeltaTime());

    // Update the wave vertex buffer with the new solution.
    auto currWavesVB = mCurrFrameResource->WavesVB.get();
    for (int i = 0; i < waveObject->GetWaves()->VertexCount(); ++i)
    {
        Vertex v;
        v.Pos = waveObject->GetWaves()->Position(i);
        v.Normal = waveObject->GetWaves()->Normal(i);

        // Derive tex-coords from position by
        // mapping [-w/2,w/2] --> [0,1]
        v.TexC.x = 0.5f + v.Pos.x / waveObject->GetWaves()->Width();
        v.TexC.y = 0.5f - v.Pos.z / waveObject->GetWaves()->Depth();

        currWavesVB->CopyData(i, v);
    }

    // Set the dynamic VB of the wave renderitem to the current frame VB.
    waveObject->GetRenderItem()->Geo->VertexBufferGPU = currWavesVB->Resource();
}

void Graphic::DrawFullscreenQuad(ID3D12GraphicsCommandList* cmdList)
{
    // Null-out IA stage since we build the vertex off the SV_VertexID in the shader.
    cmdList->IASetVertexBuffers(0, 1, nullptr);
    cmdList->IASetIndexBuffer(nullptr);
    cmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

    cmdList->DrawInstanced(6, 1, 0, 0);
}
