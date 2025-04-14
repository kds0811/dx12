#include "RenderTarget.h"
#include "Device.h"
#include "DescriptorHeapManager.h"

RenderTarget::RenderTarget(
    const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension, D3D12_RESOURCE_STATES state)
{
    Initialize(name, width, height, format, clearValue, resourceDimension, state);
}

RenderTarget::RenderTarget(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_DIMENSION resourceDimension, ID3D12Resource* existingResource,
    D3D12_RESOURCE_STATES state)
{
    InitializeAsBackBuffer(name, width, height, format, resourceDimension, existingResource, state);
}

void RenderTarget::Initialize(
    const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension, D3D12_RESOURCE_STATES state)
{
    if (IsInitialized()) return;

    assert(mWidth < 1000000 && mHeight < 1000000);

    mWidth = width;
    mHeight = height;
    mFormat = format;
    mClearValue = clearValue;
    mResourceDimension = resourceDimension;
    BuildResource(name, state);
    BuildDescriptors();
}

void RenderTarget::InitializeAsBackBuffer(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_DIMENSION resourceDimension,
    ID3D12Resource* existingResource, D3D12_RESOURCE_STATES state)
{
    if (IsInitialized()) return;

    assert(existingResource);
    assert(mWidth < 1000000 && mHeight < 1000000);

    if (!existingResource)
    {
        LOG_ERROR("Existing resource is null.");
        return;
    }

    SetResource(name, existingResource, state);
    mWidth = width;
    mHeight = height;
    mFormat = format;
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
        BuildResource(GetName(), GetCurrentState());
        BuildDescriptors();
    }
}

void RenderTarget::BuildResource(const std::wstring& name, D3D12_RESOURCE_STATES state)
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

    CreateResource(name, &HeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, state, &mClearValue);
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
