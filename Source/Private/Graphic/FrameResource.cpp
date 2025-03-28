#include "FrameResource.h"
#include "cassert"
#include "Logger.h"
#include "CommandAllocator.h"

FrameResource::FrameResource(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    Initialize(device,  passCount, objectCount, materialCount);
}

FrameResource::~FrameResource() = default;

UINT64 FrameResource::GetFenceValue() noexcept
{
    assert(mCommandAllocator);
    if (!mCommandAllocator)
    {
        LOG_ERROR("Command Allocator pointer is nullptr");
        return 0;
    }
    return mCommandAllocator->GetFenceValue();
}

void FrameResource::SetFenceValue(UINT64 value) noexcept
{
    assert(mCommandAllocator);
    if (!mCommandAllocator)
    {
        LOG_ERROR("Command Allocator pointer is nullptr");
        return;
    }
    mCommandAllocator->SetFenceValue(value);
}

ID3D12CommandAllocator* FrameResource::GetCommandListAllocator() noexcept
{
    assert(mCommandAllocator);
    if (!mCommandAllocator)
    {
        LOG_ERROR("Command Allocator pointer is nullptr");
        return nullptr;
    }
    return mCommandAllocator->GetCommandListAllocator();
}

ID3D12CommandAllocator* FrameResource::GetCommandListAllocator() const noexcept
{
    assert(mCommandAllocator);
    if (!mCommandAllocator)
    {
        LOG_ERROR("Command Allocator pointer is nullptr");
        return nullptr;
    }
    return mCommandAllocator->GetCommandListAllocator();
}

bool FrameResource::ResetCommandAllocatorIfFenceComplited(UINT64 fence)
{
    assert(mCommandAllocator);
    if (!mCommandAllocator)
    {
        LOG_ERROR("Command Allocator pointer is nullptr");
        return false;
    }
    return mCommandAllocator->ResetCommandAllocatorIfFenceComplited(fence);
}

void FrameResource::Initialize(ID3D12Device* device, UINT passCount, UINT objectCount, UINT materialCount)
{
    assert(device);

    mCommandAllocator = std::make_unique<CommandAllocator>(device);

    PassCB = std::make_unique<UploadBuffer<PassConstants>>(device, passCount, true);

    MaterialCB = std::make_unique<UploadBuffer<MaterialConstants>>(device, materialCount, true);

    ObjectCB = std::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, true);

    assert(mCommandAllocator);
    assert(PassCB);
    assert(MaterialCB);
    assert(ObjectCB);

    LOG_MESSAGE("Frame Resource has been created");
}
