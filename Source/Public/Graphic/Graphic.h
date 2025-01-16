#pragma once
#include "D3D12Utils.h"
#include <memory>
#include "UploadBuffer.h"
#include "MathHelper.h"

using namespace Microsoft::WRL;


struct Vertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT4 Color;
};

struct ObjectConstants
{
    DirectX::XMFLOAT4X4 WorldViewProj = MathHelper::Identity4x4();
};



class Graphic
{
public:
    Graphic(UINT Width, UINT Height, HWND hwnd);
    ~Graphic();

    float GetAspectRatio() const;
    void OnResize(UINT nWidth, UINT nHeight);
    void Draw();
    void Update(DirectX::FXMMATRIX ViewMat);


private:
    static constexpr UINT SwapChainBufferCount = 2;
    DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    int CurrBackBuffer = 0;
    UINT RtvDescriptorSize = 0u;
    UINT DsvDescriptorSize = 0u;
    UINT CbvSrvDescriptorSize = 0u;
    UINT ClientWidth = 0u;
    UINT ClientHeight = 0u;
    UINT MsaaQuality4x = 0u;
    bool MsaaState4x = false;
    HWND WindowHandle = nullptr;
    D3D_DRIVER_TYPE D3dDriverType = D3D_DRIVER_TYPE_HARDWARE;
    DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    D3D12_RECT ScissorRect;
    D3D12_VIEWPORT ScreenViewport; 

    ComPtr<ID3D12Debug3> DebugController;
    ComPtr<IDXGIFactory7> Factory;
    ComPtr<ID3D12Device8> Device;

    ComPtr<ID3D12Fence1> Fence;
    UINT64 CurrentFence = 0;

    ComPtr<ID3D12CommandQueue> CommandQueue;
    ComPtr<ID3D12CommandAllocator> CommandAlloc;
    ComPtr<ID3D12GraphicsCommandList6> CommandList;
    ComPtr<IDXGISwapChain4> SwapChain;
    ComPtr<ID3D12DescriptorHeap> RtvHeap;
    ComPtr<ID3D12DescriptorHeap> DsvHeap;
    ComPtr<ID3D12DescriptorHeap> CbvHeap;
    ComPtr<ID3D12Resource> SwapChainBuffer[SwapChainBufferCount];
    ComPtr<ID3D12Resource> DepthStencilBuffer;


    // New
    ComPtr<ID3D12RootSignature> mRootSignature = nullptr;

    std::unique_ptr<UploadBuffer<ObjectConstants>> mObjectCB = nullptr;

    std::unique_ptr<MeshGeometry> mBoxGeo = nullptr;

    ComPtr<ID3DBlob> mvsByteCode = nullptr;
    ComPtr<ID3DBlob> mpsByteCode = nullptr;

    std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout;

    ComPtr<ID3D12PipelineState> mPSO = nullptr;

    DirectX::XMFLOAT4X4 mWorld = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mView = MathHelper::Identity4x4();
    DirectX::XMFLOAT4X4 mProj = MathHelper::Identity4x4();


private:
    inline D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentBackBufferView() const noexcept;
    inline D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView() const noexcept;
    inline ID3D12Resource* CurrentBackBuffer() const noexcept;

    void InitPipeline();
    void EnableDebugLayer();
    void CreateFactory();
    void CreateDevice();
    void CreateFence();
    void SetDescriptorSizes();
    void CheckMSAASupport();
    void CreateCommandObjects();
    void CreateSwapChain();
    void CreateRtvAndDsvDescriptorHeaps();
    void CreateRtvforSwapChain();
    void CreateDsvForSwapChain();
    void CreateAndSetViewport();
    void CreateScissorRect();
    void FlushCommandQueue();


    // new
    void BuildDescriptorHeaps();
    void BuildConstantBuffers();
    void BuildRootSignature();
    void BuildShadersAndInputLayout();
    void BuildBoxGeometry();
    void BuildPSO();
    
};
