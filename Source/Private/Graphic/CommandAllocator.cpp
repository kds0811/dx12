#include "CommandAllocator.h"


CommandAllocator::CommandAllocator(ID3D12Device* device)
{
    Initialize(device);
}

ID3D12CommandAllocator* CommandAllocator::GetCommandListAllocator()
{
    if (!mCommandAlloc)
    {
        LOG_ERROR("Command list allocator is nullptr");
        return nullptr;
    }
    return mCommandAlloc.Get();
}

ID3D12CommandAllocator* CommandAllocator::GetCommandListAllocator() const
{
    if (!mCommandAlloc)
    {
        LOG_ERROR("Command list allocator is nullptr");
        return nullptr;
    }
    return mCommandAlloc.Get();
}

bool CommandAllocator::ResetCommandAllocatorIfFenceComplited(UINT64 fence)
{
    if (mFence > fence)
    {
        LOG_MESSAGE("Ñurrent Fence value of this allocator is not fulfilled. It is necessary to wait for execution.");
        return false;
    }
    mCommandAlloc->Reset();
    return true;
}

void CommandAllocator::Initialize(ID3D12Device* device)
{
    assert(device);

    if (device)
    {
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAlloc.GetAddressOf())) >> Kds::App::Check;
        mCommandAlloc->SetName(L"Command List Allocator");
    }

    if (mCommandAlloc)
    {
        LOG_MESSAGE("Command List Allocator has been created");
    }
    else
    {
        LOG_ERROR("Error when creating a Command List Allocator");
    }
}
 