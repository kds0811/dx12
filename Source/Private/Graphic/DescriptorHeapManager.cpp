#include "DescriptorHeapManager.h"
#include "DescriptorHeapAllocator.h"

DescriptorHeapManager::DescriptorHeapManager()
{
    mRtvAllocator = std::make_unique<DescriptorAllocator<D3D12_DESCRIPTOR_HEAP_FLAG_NONE>>(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvAllocator = std::make_unique<DescriptorAllocator<D3D12_DESCRIPTOR_HEAP_FLAG_NONE>>(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvUavAllocator = std::make_unique<DescriptorAllocator<D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE>>(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

}

DescriptorHeapManager::~DescriptorHeapManager() = default;

DescriptorHandle DescriptorHeapManager::AllocateRtv(UINT32 count)
{
    if (!mRtvAllocator)
    {
        LOG_ERROR("mRtvAllocator is nullptr");
        return DescriptorHandle{};
    }
    return mRtvAllocator->Allocate(count);
}

DescriptorHandle DescriptorHeapManager::AllocateDsv(UINT32 count)
{
    if (!mDsvAllocator)
    {
        LOG_ERROR("mDsvAllocator is nullptr");
        return DescriptorHandle{};
    }
    return mDsvAllocator->Allocate(count);
}

DescriptorHandle DescriptorHeapManager::AllocateCbvSrvUav(UINT32 count)
{
    if (!mCbvSrvUavAllocator)
    {
        LOG_ERROR("mRtvAllocator is nullptr");
        return DescriptorHandle{};
    }
    return mCbvSrvUavAllocator->Allocate(count);
}

void DescriptorHeapManager::DeallocateRtv(DescriptorHandle descriptorHandle)
{
    if (!mRtvAllocator)
    {
        LOG_ERROR("mRtvAllocator is nullptr");
        return;
    }
    mRtvAllocator->Deallocate(descriptorHandle);
}

void DescriptorHeapManager::DeallocateDsv(DescriptorHandle descriptorHandle) 
{
    if (!mDsvAllocator)
    {
        LOG_ERROR("mDsvAllocator is nullptr");
        return;
    }
    mDsvAllocator->Deallocate(descriptorHandle);
}

void DescriptorHeapManager::DeallocateCbvSrvUav(DescriptorHandle descriptorHandle) 
{
    if (!mCbvSrvUavAllocator)
    {
        LOG_ERROR("mRtvAllocator is nullptr");
        return;
    }
    mCbvSrvUavAllocator->Deallocate(descriptorHandle);
}
