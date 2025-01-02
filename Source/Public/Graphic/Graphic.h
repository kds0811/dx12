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

private:
    void InitPipeline();
    void EnableDebugLayer();
    void CreateFactory();
    void CreateDevice();
};
