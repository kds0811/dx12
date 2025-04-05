#include "RenderTarget.h"
#include "Device.h"
#include "DescriptorHeapManager.h"

bool RenderTarget::Initialize(std::wstring name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension)
{
    assert(mWidth > 0 && mWidth < 1000000 && mHeight > 0 && mHeight < 1000000);
    if (mWidth == 0 || mHeight == 0)
    {
        LOG_ERROR("Width and height must be greater than zero.");
        return false;
    }

    mWidth = width;
    mHeight = height;
    mFormat = format;
    mClearValue = clearValue;
    mResourceDimension = resourceDimension;
    mName = std::move(name);
    mCurrentState = D3D12_RESOURCE_STATE_RENDER_TARGET;

    if (!BuildResource())
    {
        LOG_ERROR("RenderTarget: ", mName, " Build Resource is failed!");
        return false;
    }

    if (!BuildDescriptors())
    {
        LOG_ERROR("RenderTarget: ", mName, " BuildDescriptors is failed!");
        return false;
    }

    return true;
}

bool RenderTarget::InitializeAsBackBuffer(ID3D12Resource* existingResource, std::wstring name, DXGI_FORMAT format)
{
    assert(existingResource);
    if (!existingResource)
    {
        LOG_ERROR("Existing resource is null.");
        return false;
    }

    mResource = existingResource;
    mName = std::move(name);
    mFormat = format;
    mCurrentState = D3D12_RESOURCE_STATE_PRESENT;

    const D3D12_RESOURCE_DESC& desc = mResource->GetDesc();
    mWidth = static_cast<UINT>(desc.Width);
    mHeight = desc.Height;
    mResourceDimension = desc.Dimension;

    mResource->SetName(mName.c_str());

    if (!BuildDescriptors())
    {
        LOG_ERROR("Failed to build descriptors for RenderTarget.");
        return false;
    }

    return true;
}

RenderTarget::~RenderTarget()
{
    Destroy();
}

void RenderTarget::OnResize(UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        if (mResource)
        {
            Destroy();
        }

        mWidth = newWidth;
        mHeight = newHeight;

        if (!BuildResource())
        {
            LOG_ERROR("Failed BuildResource on resize RenderTarget.");
            assert(0);
            return;
        }

        if (!BuildDescriptors())
        {
            LOG_ERROR("Failed BuildDescriptors on resize RenderTarget.");
            assert(0);
            return;
        }
    }
}

bool RenderTarget::BuildResource()
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

    

    Device::GetDevice()->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, mCurrentState, &mClearValue, IID_PPV_ARGS(&mResource)) >>
        Kds::App::Check;

    if (mResource)
    {
        mResource->SetName((mName + std::to_wstring(mVersionID)).c_str());
        return true;
    }
    else
    {
        LOG_ERROR("Failed to create resource.");
        assert(0);
        return false;
    }
}

bool RenderTarget::BuildDescriptors()
{
    mSrvDescriptorHandle = DescriptorHeapManager::AllocateCbvSrvUav();
    if (mSrvDescriptorHandle.IsNull())
    {
        LOG_ERROR("Failed to allocate SRV descriptor.");
        return false;
    }

    mRtvDescriptorHandle = DescriptorHeapManager::AllocateRtv();
    if (mRtvDescriptorHandle.IsNull())
    {
        LOG_ERROR("Failed to allocate RTV descriptor.");
        return false;
    }

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    Device::GetDevice()->CreateShaderResourceView(mResource.Get(), &srvDesc, mSrvDescriptorHandle.GetCpuHandle());
    Device::GetDevice()->CreateRenderTargetView(mResource.Get(), nullptr, mRtvDescriptorHandle.GetCpuHandle());
    return true;
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
