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

private:
    ComPtr<ID3D12Debug3> DebugController;
    ComPtr<IDXGIFactory7> Factory;
    ComPtr<ID3D12Device8> Device;
    ComPtr<ID3D12Fence1> Fence;
    ComPtr<ID3D12CommandQueue> CommandQueue;
    ComPtr<ID3D12CommandAllocator> CommandAlloc;
    ComPtr<ID3D12GraphicsCommandList6> CommandList;
    ComPtr<IDXGISwapChain4> SwapChain;
    ComPtr<ID3D12DescriptorHeap> RtvHeap;
    ComPtr<ID3D12DescriptorHeap> DsvHeap;


    DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    static constexpr int SwapChainBufferCount = 2;
    int CurrBackBuffer = 0;
    UINT RtvDescriptorSize = 0;
    UINT DsvDescriptorSize = 0;
    UINT CbvSrvDescriptorSize = 0;
    UINT ClientWidth = 0;
    UINT ClientHeight = 0;
    UINT MsaaQuality4x = 0;
    bool MsaaState4x = false;
    HWND WindowHandle = nullptr;

private:
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
};
