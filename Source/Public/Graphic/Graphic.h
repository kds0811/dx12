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
    ComPtr<ID3D12Debug3> m_DebugController;
    ComPtr<IDXGIFactory7> m_Factory;
    ComPtr<ID3D12Device8> m_Device;
    ComPtr<ID3D12Fence1> m_Fence;
    ComPtr<ID3D12CommandQueue> m_CommandQueue;
    ComPtr<ID3D12CommandAllocator> m_CommandAlloc;
    ComPtr<ID3D12GraphicsCommandList6> m_CommandList;
    ComPtr<IDXGISwapChain4> m_SwapChain;
    ComPtr<ID3D12DescriptorHeap> m_RtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_DsvHeap;


    DXGI_FORMAT m_BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    static constexpr int m_SwapChainBufferCount = 2;
    int m_CurrBackBuffer = 0;
    UINT m_RtvDescriptorSize = 0;
    UINT m_DsvDescriptorSize = 0;
    UINT m_CbvSrvDescriptorSize = 0;
    UINT m_ClientWidth = 0;
    UINT m_ClientHeight = 0;
    UINT m_4xMsaaQuality = 0;
    bool m_4xMsaaState = false;
    HWND m_hMainWnd = nullptr;

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
