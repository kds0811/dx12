#pragma once
#include "GraphicCommonHeaders.h"

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
class DescriptorAllocator;
class DescriptorHandle;

class DescriptorHeapManager
{
    static std::unique_ptr<DescriptorAllocator<D3D12_DESCRIPTOR_HEAP_FLAG_NONE>> mRtvAllocator;
    static std::unique_ptr<DescriptorAllocator<D3D12_DESCRIPTOR_HEAP_FLAG_NONE>> mDsvAllocator;
    static std::unique_ptr<DescriptorAllocator<D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE>> mCbvSrvUavAllocator;

public:
    DescriptorHeapManager();
    ~DescriptorHeapManager();
    DescriptorHeapManager(const DescriptorHeapManager&) = delete;
    DescriptorHeapManager& operator=(const DescriptorHeapManager&) = delete;
    DescriptorHeapManager(DescriptorHeapManager&& other) noexcept = delete;
    DescriptorHeapManager& operator=(DescriptorHeapManager&& other) noexcept = delete;

    [[nodiscard]] static DescriptorHandle AllocateRtv(UINT32 count = 1);
    [[nodiscard]] static DescriptorHandle AllocateDsv(UINT32 count = 1);
    [[nodiscard]] static DescriptorHandle AllocateCbvSrvUav(UINT32 count = 1);

    static void DeallocateRtv(DescriptorHandle descriptorHandle);
    static void DeallocateDsv(DescriptorHandle descriptorHandle);
    static void DeallocateCbvSrvUav(DescriptorHandle descriptorHandle);

};
 