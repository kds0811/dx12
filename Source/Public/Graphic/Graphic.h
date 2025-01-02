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
    Graphic();

private:
    ComPtr<ID3D12Debug> m_DebugController;
    ComPtr<IDXGIFactory6> m_Factory;
    ComPtr<ID3D12Device> m_Device;
    ComPtr<ID3D12Fence> m_Fence;


    UINT m_RtvDescriptorSize = 0;
    UINT m_DsvDescriptorSize = 0;
    UINT m_CbvSrvDescriptorSize = 0;


private:
    void InitPipeline();
    void EnableDebugLayer();
    void CreateFactory();
    void CreateDevice();
    void CreateFence();
    void SetDescriptorSizes();
};
