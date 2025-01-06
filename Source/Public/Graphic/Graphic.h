#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <directx/d3dx12.h>
#include "GraphicError.h"

using namespace Microsoft::WRL;
using namespace kds::app;

class Graphic
{
public:
    Graphic(UINT Width, UINT Height, HWND hwnd);
    ~Graphic();

    float GetAspectRatio() const;
    void OnResize(UINT nWidth, UINT nHeight);
    void Draw();


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
    ComPtr<ID3D12Resource> SwapChainBuffer[SwapChainBufferCount];
    ComPtr<ID3D12Resource> DepthStencilBuffer;

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
    
};
