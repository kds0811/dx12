#include "DescriptorHeapAllocator.h"
#include "Device.h"

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
DescriptorAllocator<heapFlag>::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) : mType(type), mHeapFlag(heapFlag)
{
    static_assert(heapFlag == D3D12_DESCRIPTOR_HEAP_FLAG_NONE || heapFlag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE, "Invalid heap flag for DescriptorAllocator!");

    auto device = Device::GetDevice();
    assert(device);
    if (device)
    {
        mDescriptorSize = Device::GetDevice()->GetDescriptorHandleIncrementSize(mType);
    }

    if (mType == D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV)
    {
        mNumDescriptorsPerHeap = 128;
    }

    if (mHeapFlag == D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
    {
        bIsGpuVisible = true;
        mNumDescriptorsPerHeap = 1024;
    }

    CreateNewHeapAndHandle();
}

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
DescriptorAllocator<heapFlag>::~DescriptorAllocator() = default;

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
DescriptorAllocator<heapFlag>::DescriptorAllocator(DescriptorAllocator&& other) noexcept
    : mDescriptorHeapPool(std::move(other.mDescriptorHeapPool)), mType(other.mType), mHeapFlag(heapFlag), mCurrentHeap(other.mCurrentHeap), mCurrentHandle(other.mCurrentHandle),
      mDescriptorSize(other.mDescriptorSize), mRemainingFreeHandles(other.mRemainingFreeHandles)
{
    other.mCurrentHeap = nullptr;
    other.mCurrentHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
    other.mDescriptorSize = 0;
    other.mRemainingFreeHandles = 0;
}

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
DescriptorAllocator<heapFlag>& DescriptorAllocator<heapFlag>::operator=(DescriptorAllocator<heapFlag>&& other) noexcept
{
    if (this != &other)
    {
        mDescriptorHeapPool = std::move(other.mDescriptorHeapPool);
        mType = other.mType;
        mHeapFlag = other.mHeapFlag;
        mCurrentHeap = other.mCurrentHeap;
        mCurrentHandle = other.mCurrentHandle;
        mDescriptorSize = other.mDescriptorSize;
        mRemainingFreeHandles = other.mRemainingFreeHandles;
        other.mCurrentHeap = nullptr;
        other.mCurrentHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
        other.mDescriptorSize = 0;
        other.mRemainingFreeHandles = 0;
    }
    return *this;
}

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
DescriptorHandle DescriptorAllocator<heapFlag>::Allocate(UINT32 count)
{
    assert(count > 0 && count < mNumDescriptorsPerHeap);

    std::lock_guard<std::mutex> lockGuard(mAllocationMutex);

    if (!mReleasedDescriptorHandle.empty())
    {
        if (mReleasedDescriptorHandle.contains(count))
        {
            auto it = mReleasedDescriptorHandle.find(count);
            if (it != mReleasedDescriptorHandle.end())
            {
                DescriptorHandle retH = it->second;
                mReleasedDescriptorHandle.erase(it);
                return retH;
            }
        }
    }

    if (mCurrentHeap == nullptr || mRemainingFreeHandles < count)
    {
        CreateNewHeapAndHandle();
    }

    DescriptorHandle ret = mCurrentHandle;
    mCurrentHandle += count;
    mRemainingFreeHandles -= count;
    return ret;
}

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
void DescriptorAllocator<heapFlag>::Deallocate(DescriptorHandle handle)
{
    if (!handle.IsNull())
    {
        mReleasedDescriptorHandle.emplace(handle.GetDescriptorSize(), handle);
    }
}

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
ID3D12DescriptorHeap* DescriptorAllocator<heapFlag>::RequestNewHeap()
{
    D3D12_DESCRIPTOR_HEAP_DESC Desc{};
    Desc.Type = mType;
    Desc.NumDescriptors = mNumDescriptorsPerHeap;
    Desc.Flags = mHeapFlag;
    Desc.NodeMask = 1;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> pHeap;

    auto device = Device::GetDevice();
    assert(device);
    if (device)
    {
        device->CreateDescriptorHeap(&Desc, IID_PPV_ARGS(&pHeap)) >> Kds::App::Check;
    }
    mDescriptorHeapPool.emplace_back(pHeap);
    LOG_MESSAGE("Create new descriptor heap");
    return pHeap.Get();
}

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
void DescriptorAllocator<heapFlag>::CreateNewHandle()
{
    if (bIsGpuVisible)
    {
        mCurrentHandle = DescriptorHandle{mCurrentHeap->GetCPUDescriptorHandleForHeapStart(), mCurrentHeap->GetGPUDescriptorHandleForHeapStart(), mDescriptorSize};
    }
    else
    {
        mCurrentHandle = DescriptorHandle{mCurrentHeap->GetCPUDescriptorHandleForHeapStart(), mDescriptorSize};
    }
}

template <D3D12_DESCRIPTOR_HEAP_FLAGS heapFlag>
void DescriptorAllocator<heapFlag>::CreateNewHeapAndHandle()
{
    mCurrentHeap = RequestNewHeap();
    assert(mCurrentHeap && "Failed to create a new descriptor heap!");

    CreateNewHandle();
    mRemainingFreeHandles = mNumDescriptorsPerHeap;
}
