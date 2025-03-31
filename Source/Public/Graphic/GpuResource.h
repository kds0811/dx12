#pragma once
#include "GraphicCommonHeaders.h"

class GpuResource
{
protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource = nullptr;
    D3D12_RESOURCE_STATES mCurrenState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
    D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    uint32_t mVersionID = 0;

public:
    GpuResource() = default;

    GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState)
        :  mResource(pResource), mCurrenState(CurrentState)
    {  }

    ~GpuResource() { Destroy(); }

    virtual void Destroy()
    {
        mResource = nullptr;
        mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
        ++mVersionID;
    }

    [[nodiscard]] inline ID3D12Resource* GetResource() { return mResource.Get(); }
    const ID3D12Resource* GetResource() const  { return mResource.Get(); }

    ID3D12Resource** GetAddressOf() { return mResource.GetAddressOf(); }
    D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const noexcept { return mGpuVirtualAddress; }
    uint32_t GetVersionID() const noexcept { return mVersionID; }

    [[nodiscard]] inline bool ChangeState(ID3D12GraphicsCommandList* cmdList, D3D12_RESOURCE_STATES newState)
    {
        if (newState == mCurrenState)
        {
            LOG_ERROR("Current D3D12_RESOURCE_STATES is same new State");
            return false;
        }
        auto ResBar = CD3DX12_RESOURCE_BARRIER::Transition(mResource.Get(), mCurrenState, newState);
        cmdList->ResourceBarrier(1, &ResBar);
        mCurrenState = newState;
        return true;
    }
    
    [[nodiscard]] inline D3D12_RESOURCE_STATES GetCurrentState() const noexcept { return mCurrenState; }

};
