#pragma once
#include "GraphicCommonHeaders.h"
#include "DescriptorHeapAllocator.h"

class CommandList;

class GpuResource
{
    Microsoft::WRL::ComPtr<ID3D12Resource> mResource = nullptr;
    D3D12_RESOURCE_STATES mCurrentState = D3D12_RESOURCE_STATES::D3D12_RESOURCE_STATE_COMMON;
    D3D12_GPU_VIRTUAL_ADDRESS mGpuVirtualAddress = D3D12_GPU_VIRTUAL_ADDRESS_NULL;
    uint32_t mVersionID = 0;
    std::wstring mName{};

public:
    GpuResource();
    GpuResource(ID3D12Resource* pResource, D3D12_RESOURCE_STATES CurrentState);
    GpuResource(const GpuResource& rhs);
    GpuResource& operator=(const GpuResource& rhs);
    GpuResource(const GpuResource&& rhs) noexcept;
    GpuResource& operator=(const GpuResource&& rhs) noexcept;
    virtual ~GpuResource();

    void SetName(const std::wstring& name);
    void CreateResource(const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES InitialResourceState,
        const D3D12_CLEAR_VALUE* pOptimizedClearValue);
    void CreateResource(const D3D12_HEAP_PROPERTIES* pHeapProperties, D3D12_HEAP_FLAGS HeapFlags, const D3D12_RESOURCE_DESC* pDesc, D3D12_RESOURCE_STATES InitialResourceState);

    void DestroyResource();
    [[nodiscard]] inline ID3D12Resource* GetResource() { return mResource.Get(); }
    [[nodiscard]] inline const ID3D12Resource* GetResource() const { return mResource.Get(); }
    [[nodiscard]] inline ID3D12Resource** GetAddressOf() { return mResource.GetAddressOf(); }
    [[nodiscard]] inline D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const noexcept { return mGpuVirtualAddress; }
    [[nodiscard]] inline uint32_t GetVersionID() const noexcept { return mVersionID; }
    [[nodiscard]] inline bool ResourceIsInitialized() const noexcept { return mResource; }
    inline void IncrementVersion() { ++mVersionID; }

    void ChangeState(CommandList* cmdList, D3D12_RESOURCE_STATES newState);
    [[nodiscard]] inline D3D12_RESOURCE_STATES GetCurrentState() const noexcept { return mCurrentState; }
    Microsoft::WRL::ComPtr<ID3D12Resource>& GetComPtr() { return mResource; }

protected:
    void SetResource(ID3D12Resource* resource, D3D12_RESOURCE_STATES newState);
};
