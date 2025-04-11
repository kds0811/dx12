#include "RenderTarget.h"
#include "Device.h"
#include "DescriptorHeapManager.h"

RenderTarget::RenderTarget(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension)
{
    Initialize(name, width, height, format, clearValue, resourceDimension);
}

void RenderTarget::Initialize(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension)
{
    if (IsInitialized()) return;
    assert(mWidth > 0 && mWidth < 1000000 && mHeight > 0 && mHeight < 1000000);

    SetName(name);
    mWidth = width;
    mHeight = height;
    mFormat = format;
    mClearValue = clearValue;
    mResourceDimension = resourceDimension;
    BuildResource();
    BuildDescriptors();
}

void RenderTarget::InitializeAsBackBuffer(const std::wstring& name, ID3D12Resource* existingResource, DXGI_FORMAT format, UINT width, UINT height)
{
    assert(existingResource);
    if (!existingResource)
    {
        LOG_ERROR("Existing resource is null.");
        return;
    }
    SetResource(existingResource, D3D12_RESOURCE_STATE_PRESENT);
    SetName(name);
    mWidth = width;
    mHeight = height;
    mFormat = format;
    const D3D12_RESOURCE_DESC& desc = GetResource()->GetDesc();
    mResourceDimension = desc.Dimension;
    BuildDescriptors();
}

RenderTarget::~RenderTarget()
{
    Destroy();
}

void RenderTarget::OnResize(UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;
        Destroy();
        BuildResource();
        BuildDescriptors();
    }
}

void RenderTarget::BuildResource()
{
    CD3DX12_HEAP_PROPERTIES HeapProps(D3D12_HEAP_TYPE_DEFAULT);

    D3D12_RESOURCE_DESC resourceDesc;
    ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
    resourceDesc.Dimension = mResourceDimension;
    resourceDesc.Alignment = 0;
    resourceDesc.Width = mWidth;
    resourceDesc.Height = mHeight;
    resourceDesc.DepthOrArraySize = 1;
    resourceDesc.MipLevels = 1;
    resourceDesc.Format = mFormat;
    resourceDesc.SampleDesc.Count = 1;
    resourceDesc.SampleDesc.Quality = 0;
    resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

    CreateResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_RENDER_TARGET, &mClearValue);
}

void RenderTarget::BuildDescriptors()
{
    mSrvDescriptorHandle = DescriptorHeapManager::AllocateCbvSrvUav();
    if (mSrvDescriptorHandle.IsNull())
    {
        LOG_ERROR("Failed to allocate SRV descriptor.");
        return;
    }
    mRtvDescriptorHandle = DescriptorHeapManager::AllocateRtv();
    if (mRtvDescriptorHandle.IsNull())
    {
        LOG_ERROR("Failed to allocate RTV descriptor.");
        return;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    Device::GetDevice()->CreateShaderResourceView(GetResource(), &srvDesc, mSrvDescriptorHandle.GetCpuHandle());
    Device::GetDevice()->CreateRenderTargetView(GetResource(), nullptr, mRtvDescriptorHandle.GetCpuHandle());

}

void RenderTarget::Destroy()
{
    DestroyResource();
    DestroyDescriptorHandles();
}

void RenderTarget::DestroyDescriptorHandles()
{
    DescriptorHeapManager::DeallocateRtv(mRtvDescriptorHandle);
    DescriptorHeapManager::DeallocateCbvSrvUav(mSrvDescriptorHandle);
}
