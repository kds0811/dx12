#pragma once
#include "D3D12Utils.h"
#include <memory>
#include "UploadBuffer.h"
#include "MathHelper.h"

using namespace Microsoft::WRL;

struct VertexPos
{
    DirectX::XMFLOAT3 Pos;
};

struct VertexCol
{
    DirectX::XMFLOAT4 Color;
};

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
    float time = 0.0f;
};

class Graphic
{
private:
    // Main Fields
    ComPtr<ID3D12Debug3> mDebugController;
    ComPtr<IDXGIFactory7> mFactory;
    ComPtr<ID3D12Device8> mDevice;

    // Command Stuff
    ComPtr<ID3D12Fence1> mFence;
    UINT64 mCurrentFence = 0;
    ComPtr<ID3D12CommandQueue> mCommandQueue;
    ComPtr<ID3D12CommandAllocator> mCommandAlloc;
    ComPtr<ID3D12GraphicsCommandList6> mCommandList;

    // SwapChain and Backbuffer Fields
    static constexpr UINT mSwapChainBufferCount = 2;
    ComPtr<IDXGISwapChain4> mSwapChain;
    ComPtr<ID3D12Resource> mSwapChainBuffer[mSwapChainBufferCount];
    DXGI_FORMAT mBackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    int mCurrBackBuffer = 0;

    // Depth Stencil Fields
    ComPtr<ID3D12Resource> mDepthStencilBuffer;
    DXGI_FORMAT mDepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    // Decriptor Heaps
    ComPtr<ID3D12DescriptorHeap> mRtvHeap;
    ComPtr<ID3D12DescriptorHeap> mDsvHeap;
    ComPtr<ID3D12DescriptorHeap> mCbvHeap;

    // Decriptor Sizes
    UINT mRtvDescriptorSize = 0u;
    UINT mDsvDescriptorSize = 0u;
    UINT mCbvSrvDescriptorSize = 0u;

    // Viewport and window fields
    D3D12_VIEWPORT mScreenViewport;
    UINT mClientWidth = 0u;
    UINT mClientHeight = 0u;
    D3D12_RECT mScissorRect;
    HWND mWindowHandle = nullptr;

    //Driver Type
    D3D_DRIVER_TYPE mD3dDriverType = D3D_DRIVER_TYPE_HARDWARE;

    

public:
    Graphic(UINT Width, UINT Height, HWND hwnd);
    ~Graphic();

    float GetAspectRatio() const;
    void OnResize(UINT nWidth, UINT nHeight);
    void Draw();
    void Update(DirectX::FXMMATRIX ViewMat, float TotalTime);

private:
    // New
    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    DirectX::XMFLOAT4X4 mWorldBox = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mWorldPyramid = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();

private:
    D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const noexcept;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const noexcept;
    ID3D12Resource* CurrentBackBuffer() const noexcept;

    void InitPipeline();
    void InitResources();
    void FlushCommandQueue();

    // new
    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildBoxGeometry();
    void BuildPSO();
};
