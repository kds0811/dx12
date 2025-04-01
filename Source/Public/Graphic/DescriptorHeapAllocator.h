#pragma once
#include "GraphicCommonHeaders.h"
#include <mutex>


class DescriptorAllocator
{
    static constexpr UINT16 mNumDescriptorsPerHeap = 1024;
    std::mutex mAllocationMutex;
    std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mDescriptorHeapPool;
 
     D3D12_DESCRIPTOR_HEAP_TYPE mType;
     ID3D12DescriptorHeap* mCurrentHeap = nullptr;
     D3D12_CPU_DESCRIPTOR_HANDLE mCurrentHandle;
     UINT32 mDescriptorSize = 0;
     UINT32 mRemainingFreeHandles = 0;

public:
    DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type);
    ~DescriptorAllocator();
    DescriptorAllocator(const DescriptorAllocator&) = delete;
    DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;
    DescriptorAllocator(DescriptorAllocator&& other) noexcept;
    DescriptorAllocator& operator=(DescriptorAllocator&& other) noexcept;

    D3D12_CPU_DESCRIPTOR_HANDLE Allocate(UINT32 count = 1);
    ID3D12DescriptorHeap* RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type);

};