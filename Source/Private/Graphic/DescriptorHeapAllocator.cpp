#include "DescriptorHeapAllocator.h"
#include "Device.h"

DescriptorAllocator::DescriptorAllocator(D3D12_DESCRIPTOR_HEAP_TYPE type) : mType(type)
{
    mCurrentHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
    auto device = Device::GetDevice();
    assert(device);
    if (device)
    {
        mDescriptorSize = Device::GetDevice()->GetDescriptorHandleIncrementSize(mType);
    }
}

DescriptorAllocator::~DescriptorAllocator() = default;

DescriptorAllocator::DescriptorAllocator(DescriptorAllocator&& other) noexcept
    :  mDescriptorHeapPool(std::move(other.mDescriptorHeapPool)), mType(other.mType), mCurrentHeap(other.mCurrentHeap),
      mCurrentHandle(other.mCurrentHandle), mDescriptorSize(other.mDescriptorSize), mRemainingFreeHandles(other.mRemainingFreeHandles)
{
    other.mCurrentHeap = nullptr;
    other.mCurrentHandle.ptr = D3D12_GPU_VIRTUAL_ADDRESS_UNKNOWN;
    other.mDescriptorSize = 0;
    other.mRemainingFreeHandles = 0;
}

DescriptorAllocator& DescriptorAllocator::operator=(DescriptorAllocator&& other) noexcept
{
    if (this != &other)
    {
        mDescriptorHeapPool = std::move(other.mDescriptorHeapPool);
        mType = other.mType;
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

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorAllocator::Allocate(UINT32 count)
{
    std::lock_guard<std::mutex> lockGuard(mAllocationMutex);

    if (mCurrentHeap == nullptr || mRemainingFreeHandles < count)
    {
        mCurrentHeap = RequestNewHeap(mType);
        mCurrentHandle = mCurrentHeap->GetCPUDescriptorHandleForHeapStart();
        mRemainingFreeHandles = mNumDescriptorsPerHeap;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE ret = mCurrentHandle;
    mCurrentHandle.ptr += count * mDescriptorSize;
    mRemainingFreeHandles -= count;
    return ret;
}



ID3D12DescriptorHeap* DescriptorAllocator::RequestNewHeap(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
    D3D12_DESCRIPTOR_HEAP_DESC Desc{};
    Desc.Type = type;
    Desc.NumDescriptors = mNumDescriptorsPerHeap;
    Desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
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
