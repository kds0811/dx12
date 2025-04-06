#include "DepthStencil.h"
#include "DescriptorHeapManager.h"
#include "Device.h"

DepthStencil::~DepthStencil() = default;

bool DepthStencil::Initialize(std::wstring name, UINT width, UINT height, DXGI_FORMAT format)
{
    mName = name;
    mWidth = width;
    mHeight = height;
    mFormat = format;
    mCurrentState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

    if (!BuildResource()) return false;

    if (!BuildDescriptors())
    {
        Destroy();
        return false;
    }

    return true;
}

bool DepthStencil::InitializeFromExistingResource(ID3D12Resource* existingResource, std::wstring name, DXGI_FORMAT format)
{
    return false;
}

void DepthStencil::OnResize(UINT newWidth, UINT newHeight)
{
    if (mWidth == newWidth && mHeight == newHeight) return;

    mWidth = newWidth;
    mHeight = newHeight;

    Destroy();
    BuildResource();
    BuildDescriptors();
}

bool DepthStencil::BuildResource()
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

    Device::GetDevice()->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, mCurrentState, &optClear, IID_PPV_ARGS(&mResource)) >>
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

bool DepthStencil::BuildDescriptors()
{
    mDsvDescriptorHandle = DescriptorHeapManager::AllocateDsv();
    if (mDsvDescriptorHandle.IsNull())
    {
        LOG_ERROR("Failed to allocate DSV descriptor");
        return false;
    }

    Device::GetDevice()->CreateDepthStencilView(mResource.Get(), nullptr, mDsvDescriptorHandle.GetCpuHandle());
    return true;
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
