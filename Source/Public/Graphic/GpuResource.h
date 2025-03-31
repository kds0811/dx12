#pragma once
#include "GraphicCommonHeaders.h"

class CommandList;


class GpuResource
{
protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource = nullptr;
    D3D12_RESOURCE_STATES mCurrenState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
    D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    uint32_t mVersionID = 0;

public:
    GpuResource();
    GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState);
    ~GpuResource();

    virtual void Destroy();
    virtual void CreateResource(ID3D12Device* device, const D3D12_RESOURCE_DESC& desc, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState);

    [[nodiscard]] inline ID3D12Resource* GetResource()  { return mResource.Get(); }
    inline const ID3D12Resource* GetResource() const { return mResource.Get(); }
    inline ID3D12Resource** GetAddressOf() { return mResource.GetAddressOf(); }
    inline D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const noexcept { return mGpuVirtualAddress; }
    inline uint32_t GetVersionID() const noexcept { return mVersionID; }
    inline void IncrementVersion() { ++mVersionID; }

    [[nodiscard]] bool ChangeState(CommandList* cmdList, D3D12_RESOURCE_STATES newState);
    [[nodiscard]] inline D3D12_RESOURCE_STATES GetCurrentState() const noexcept { return mCurrenState; }
};
