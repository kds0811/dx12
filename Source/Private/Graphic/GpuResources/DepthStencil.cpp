#include "DepthStencil.h"
#include "DescriptorHeapManager.h"
#include "Device.h"

DepthStencil::DepthStencil(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_STATES state)
{
    Initialize(name, width, height, format, state);
}

DepthStencil::~DepthStencil()
{
    Destroy();
}

void DepthStencil::Initialize(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_STATES state)
{
    if (IsInitialized()) return;

    mWidth = width;
    mHeight = height;
    mFormat = format;
    BuildResource(name, state);
    BuildDescriptors();
}

void DepthStencil::OnResize(UINT newWidth, UINT newHeight)
{
    if (mWidth == newWidth && mHeight == newHeight) return;

    mWidth = newWidth;
    mHeight = newHeight;
    Destroy();
    BuildResource(GetName(), GetCurrentState());
    BuildDescriptors();
}

void DepthStencil::BuildResource(const std::wstring& name, D3D12_RESOURCE_STATES state)
{
    D3D12_RESOURCE_DESC resourceDesc = {};
    resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = mWidth;
    resourceDesc.Height = mHeight;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = mFormat;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear{};
    optClear.Format = mFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0;

    D3D12_HEAP_PROPERTIES heapProps = {};
    heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;

    CreateResource(name, &heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, state, &optClear);
}

void DepthStencil::BuildDescriptors()
{
    mDsvDescriptorHandle = DescriptorHeapManager::AllocateDsv();
    if (mDsvDescriptorHandle.IsNull())
    {
        LOG_ERROR("Failed to allocate DSV descriptor");
        assert(false);
        return;
    }
    Device::GetDevice()->CreateDepthStencilView(GetResource(), nullptr, mDsvDescriptorHandle.GetCpuHandle());
}

void DepthStencil::Destroy()
{
    DestroyResource();
    DestroyDescriptorHandles();
}

void DepthStencil::DestroyDescriptorHandles()
{
    DescriptorHeapManager::DeallocateDsv(mDsvDescriptorHandle);
}
