#include "GpuResource.h"
#include "CommandList.h"

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

GpuResource::~GpuResource() = default;

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
