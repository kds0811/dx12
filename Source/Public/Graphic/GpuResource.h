#pragma once
#include "GraphicCommonHeaders.h"
#include "DescriptorHeapAllocator.h"

class CommandList;

class GpuResource
{
protected:
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource = nullptr;
    D3D12_RESOURCE_STATES mCurrenState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
    D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    uint32_t mVersionID = 0;
    std::wstring mName{};

public:
    GpuResource();
    GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState);
    ~GpuResource();

    void DestroyResource();
    [[nodiscard]] inline ID3D12Resource* GetResource()  { return mResource.Get(); }
    [[nodiscard]] inline const ID3D12Resource* GetResource() const { return mResource.Get(); }
    [[nodiscard]] inline ID3D12Resource** GetAddressOf() { return mResource.GetAddressOf(); }
    [[nodiscard]] inline D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const noexcept { return mGpuVirtualAddress; }
    [[nodiscard]] inline uint32_t GetVersionID() const noexcept { return mVersionID; }
    inline void IncrementVersion() { ++mVersionID; }

    [[nodiscard]] bool ChangeState(CommandList* cmdList, D3D12_RESOURCE_STATES newState);
    [[nodiscard]] inline D3D12_RESOURCE_STATES GetCurrentState() const noexcept { return mCurrenState; }
    Microsoft::WRL::ComPtr<ID3D12Resource>& GetComPtr() { return mResource; }

};
