#pragma once
#include "GraphicCommonHeaders.h"
#include <mutex>
#include <unordered_map>

class DescriptorHandle
{
    D3D12_CPU_DESCRIPTOR_HANDLE mCpuHandle;
    D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle;
    UINT32 mDescriptorSize = 0;

public:
    DescriptorHandle()
    {
        mCpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
        mGpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
    }
    DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, UINT32 descriptorSize)
        :
        mCpuHandle(CpuHandle),
        mDescriptorSize(descriptorSize)
    {
        assert(mDescriptorSize > 0);
    }

    DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE CpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE GpuHandle, UINT32 descriptorSize)
        :
        mCpuHandle(CpuHandle),
        mGpuHandle(GpuHandle),
        mDescriptorSize(descriptorSize)
    {
        assert(mDescriptorSize > 0);
    }

    void operator+=(INT countSecriptors)
    {
        if (mCpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN) mCpuHandle.ptr += countSecriptors * mDescriptorSize;
        if (mGpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN) mGpuHandle.ptr += countSecriptors * mDescriptorSize;
    }
    bool operator==(const DescriptorHandle& other) const { return mCpuHandle.ptr == other.mCpuHandle.ptr && mGpuHandle.ptr == other.mGpuHandle.ptr; }

    bool operator!=(const DescriptorHandle& other) const { return !(*this == other); }

    [[nodiscard]] inline D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle() const noexcept { return mCpuHandle; }
    [[nodiscard]] inline D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle() const noexcept { return mGpuHandle; }
    [[nodiscard]] inline size_t GetCpuPtr() const noexcept { return mCpuHandle.ptr; }
    [[nodiscard]] inline uint64_t GetGpuPtr() const noexcept { return mGpuHandle.ptr; }
    [[nodiscard]] inline bool IsNull() const noexcept { return mCpuHandle.ptr == D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
    [[nodiscard]] inline bool IsShaderVisible() const noexcept { return mGpuHandle.ptr != D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN; }
    [[nodiscard]] inline UINT32 GetDescriptorSize() const noexcept { return mDescriptorSize; }

    inline void Reset() noexcept
    {
        mCpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
        mGpuHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
        mDescriptorSize = 0;
    }
};

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
class DescriptorAllocator
{
    UINT16 mNumDescriptorsPerHeap = 32;
    std::mutex mAllocationMutex{};
    std::vector<Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>> mDescriptorHeapPool{};
    std::unordered_multimap<UINT32, DescriptorHandle> mReleasedDescriptorHandle{};

    D3D12_DESCRIPTOR_HEAP_TYPE mType{};
    D3D12_DESCRIPTOR_HEAP_FLAGS mHeapFlag{};
    ID3D12DescriptorHeap* mCurrentHeap = nullptr;
    DescriptorHandle mCurrentHandle{};
    UINT32 mDescriptorSize = 0;
    UINT32 mRemainingFreeHandles = 0;

    bool bIsGpuVisible = false;

public:
    DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type);
    ~DescriptorAllocator();
    DescriptorAllocator(const DescriptorAllocator&) = delete;
    DescriptorAllocator& operator=(const DescriptorAllocator&) = delete;
    DescriptorAllocator(DescriptorAllocator&& other) noexcept;
    DescriptorAllocator& operator=(DescriptorAllocator&& other) noexcept;

    [[nodiscard]] DescriptorHandle Allocate(UINT32 count = 1);
    void Deallocate(DescriptorHandle handle);

private:
    ID3D12DescriptorHeap* RequestNewHeap();
    void CreateNewHandle();
    void CreateNewHeapAndHandle();
};
