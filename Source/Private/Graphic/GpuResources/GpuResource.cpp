#include "GpuResource.h"
#include "CommandList.h"
#include <utility>
#include "Device.h"

GpuResource::GpuResource() = default;

GpuResource::GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) : mResource(pResource), mCurrentState(CurrentState)
{
    assert(pResource);
    if (!pResource)
    {
        LOG_ERROR("Invalid resource pointer passed to GpuResource constructor.");
    }
    mGpuVirtualAddress = pResource->GetGPUVirtualAddress();
}

GpuResource::GpuResource(const GpuResource& rhs)
    :
    mResource(rhs.mResource),
    mCurrentState(rhs.mCurrentState),
    mGpuVirtualAddress(rhs.mGpuVirtualAddress),
    mVersionID(rhs.mVersionID),
    mName(rhs.mName)
{}

GpuResource& GpuResource::operator=(const GpuResource& rhs)
{
    if (this != &rhs)
    {
        mResource = rhs.mResource;
        mCurrentState = rhs.mCurrentState;
        mGpuVirtualAddress = rhs.mGpuVirtualAddress;
        mVersionID = rhs.mVersionID;
        mName = rhs.mName;
    }
    return *this;
}

GpuResource::GpuResource(const GpuResource&& rhs) noexcept
    :
    mResource(std::exchange(rhs.mResource, nullptr)),
    mCurrentState(std::exchange(rhs.mCurrentState, D3D12_RESOURCE_STATE_COMMON)),
    mGpuVirtualAddress(std::exchange(rhs.mGpuVirtualAddress, D3D12_GPU_VIRTUAL_ADDRESS_NULL)),
    mVersionID(std::exchange(rhs.mVersionID, 0)),
    mName(std::exchange(rhs.mName, {}))
{}

GpuResource& GpuResource::operator=(const GpuResource&& rhs) noexcept
{
    if (this != &rhs)
    {
        mResource = std::exchange(rhs.mResource, nullptr);
        mCurrentState = std::exchange(rhs.mCurrentState, D3D12_RESOURCE_STATE_COMMON);
        mGpuVirtualAddress = std::exchange(rhs.mGpuVirtualAddress, D3D12_GPU_VIRTUAL_ADDRESS_NULL);
        mVersionID = std::exchange(rhs.mVersionID, 0);
        mName = std::exchange(rhs.mName, {});
    }
    return *this;
}

GpuResource::~GpuResource() = default;

void GpuResource::SetName(const std::wstring& name)
{
    mName = name;
}

void GpuResource::CreateResource(const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC* pDesc,
    D3D12_RESOURCE_STATES InitialResourceState, const D3D12_CLEAR_VALUE* pOptimizedClearValue)
{
    Device::GetDevice()->CreateCommittedResource(pHeapProperties, HeapFlags, pDesc, InitialResourceState, pOptimizedClearValue, IID_PPV_ARGS(&mResource)) >> Kds::App::Check;
    mCurrentState = InitialResourceState;
    mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
    mResource->SetName((mName + std::to_wstring(mVersionID)).c_str());
}

void GpuResource::DestroyResource()
{
    mResource = nullptr;
    mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    mCurrentState = D3D12_RESOURCE_STATE_COMMON;
    ++mVersionID;
}


bool GpuResource::ChangeState(CommandList* cmdList, D3D12_RESOURCE_STATES newState)
{
    if (newState == mCurrentState)
    {
        LOG_WARNING("Current D3D12_RESOURCE_STATES is same new State");
        return false;
    }

    auto ResBar = CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), mCurrentState, newState);
    cmdList->ResourceBarrier(1, &ResBar);
    mCurrentState = newState;

    return true;
}

void GpuResource::SetResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES newState)
{
    assert(resource);
    if (!resource) return;

    mResource = resource;
    mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
    mCurrentState = newState;
}

