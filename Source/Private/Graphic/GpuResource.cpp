#include "GpuResource.h"
#include "CommandList.h"

GpuResource::GpuResource() = default;

GpuResource::GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState) : mResource(pResource), mCurrenState(CurrentState) {}

GpuResource::~GpuResource() = default;

void GpuResource::Destroy()
{
    mResource = nullptr;
    mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    mCurrenState = D3D12_RESOURCE_STATE_COMMON;
    ++mVersionID;
}

void GpuResource::CreateResource(ID3D12Device* device, const D3D12_RESOURCE_DESC& desc, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState)
{
    device->CreateCommittedResource(&CD3DX12_HEAP_PROPERTIES(heapType), D3D12_HEAP_FLAG_NONE, &desc, initialState, nullptr, IID_PPV_ARGS(&mResource)) >> Kds::App::Check;
    mCurrenState = initialState;
    mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
}

bool GpuResource::ChangeState(CommandList* cmdList, D3D12_RESOURCE_STATES newState)
{
    if (newState == mCurrenState)
    {
        LOG_WARNING("Current D3D12_RESOURCE_STATES is same new State");
        return false;
    }

    auto ResBar = CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), mCurrenState, newState);
    cmdList->ResourceBarrier(1, &ResBar);
    mCurrenState = newState;

    return true;
}
