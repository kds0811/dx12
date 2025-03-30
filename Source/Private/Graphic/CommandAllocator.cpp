#include "CommandAllocator.h"
#include <string>


CommandAllocator::CommandAllocator(ID3D12Device* device, UINT id)
{
    Initialize(device);
}

CommandAllocator::~CommandAllocator() = default;

CommandAllocator::CommandAllocator(CommandAllocator&& other) noexcept
    :
    mFence(other.mFence),
    mCommandAlloc(std::move(other.mCommandAlloc))
{}

CommandAllocator& CommandAllocator::operator=(CommandAllocator&& other) noexcept
{
    mFence = other.mFence;
    mCommandAlloc = std::move(other.mCommandAlloc);
    return *this;
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

void CommandAllocator::Initialize(ID3D12Device* device, UINT id)
{
    assert(device);
    mID = static_cast<int>(id);
    if (device)
    {
        device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAlloc.GetAddressOf())) >> Kds::App::Check;

        std::wstring name = L"Command List Allocator ID: " + std::to_wstring(mID);
        mCommandAlloc->SetName(name.c_str());
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
 