#include "RenderTarget.h"
#include "Device.h"
#include "DescriptorHeapManager.h"

RenderTarget::RenderTarget(std::wstring name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension)
    :
    mWidth(width),
    mHeight(height),
    mFormat(format),
    mClearValue(clearValue),
    mResourceDimension(resourceDimension)
{
    mName = std::move(name);
    BuildResource();
    BuildDescriptors();
}

void RenderTarget::OnResize(UINT newWidth, UINT newHeight)
{
    if ((mWidth != newWidth) || (mHeight != newHeight))
    {
        mWidth = newWidth;
        mHeight = newHeight;

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

    Device::GetDevice()->CreateCommittedResource(&HeapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COMMON, &mClearValue, IID_PPV_ARGS(&mResource)) >> Kds::App::Check;

    mResource->SetName((mName + std::to_wstring(mVersionID)).c_str());
}

void RenderTarget::BuildDescriptors()
{
    mSrvDescriptorHandle = DescriptorHeapManager::AllocateCbvSrvUav();
    mRtvDescriptorHandle = DescriptorHeapManager::AllocateRtv();

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = mFormat;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;

    Device::GetDevice()->CreateShaderResourceView(mResource.Get(), &srvDesc, mSrvDescriptorHandle.GetCpuHandle());

    Device::GetDevice()->CreateRenderTargetView(mResource.Get(), nullptr, mRtvDescriptorHandle.GetCpuHandle());
}
