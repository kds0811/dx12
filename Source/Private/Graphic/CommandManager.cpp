#include "CommandManager.h"
#include "CommandQueue.h"
#include "CommandList.h"
#include "Pso.h"
#include "CommandAllocator.h"

using namespace Kds::App;

CommandManager::CommandManager(ID3D12Device* device)
{
    Initialize(device);
}

CommandManager::~CommandManager() = default;

CommandManager::CommandManager(CommandManager&& rhs) noexcept : mCommandQueueDirect(std::move(rhs.mCommandQueueDirect)), mCommandList(std::move(rhs.mCommandList)) {}

CommandManager& CommandManager::operator=(CommandManager&& rhs) noexcept
{
    if (this != &rhs)
    {
        mCommandQueueDirect.reset();
        mCommandList.reset();

        mCommandQueueDirect = std::move(rhs.mCommandQueueDirect);
        mCommandList = std::move(rhs.mCommandList);
    }
    return *this;
}

bool CommandManager::ExecuteCommandList()
{
    assert(mCommandList && mCommandQueueDirect);
    if (!mCommandList || !mCommandQueueDirect) return false;

    return mCommandQueueDirect->ExecuteCommandList(mCommandList.get());
}

void CommandManager::FlushCommandQueue()
{
    assert(mCommandQueueDirect);
    if (!mCommandQueueDirect) return;

    mCommandQueueDirect->FlushCommandQueue();
}

void CommandManager::WaitForExternalFence(UINT64 fenceValue)
{
    assert(mCommandQueueDirect);
    if (!mCommandQueueDirect) return;

    mCommandQueueDirect->WaitForFence(fenceValue);
}

bool CommandManager::IsExternalFenceComplete(UINT64 fenceValue)
{
    assert(mCommandQueueDirect);
    if (!mCommandQueueDirect) return false;

    return mCommandQueueDirect->IsFenceComplete(fenceValue);
}

void CommandManager::WaitForInternalFence()
{
    assert(mCommandList && mCommandQueueDirect);
    if (!mCommandList || !mCommandQueueDirect) return;

    UINT64 fence = mCommandList->GetFenceValue();
    if (fence == 0)
    {
        LOG_WARNING("CommandManager: Fence value is not set.");
        return;
    }

    mCommandQueueDirect->WaitForFence(fence);
}

bool CommandManager::IsInternalFenceComplete()
{
    assert(mCommandList && mCommandQueueDirect);
    if (!mCommandList || !mCommandQueueDirect) return false;

    UINT64 fence = mCommandList->GetFenceValue();
    if (fence == 0)
    {
        LOG_WARNING("CommandManager: Fence value is not set.");
        return;
    }
    return mCommandQueueDirect->IsFenceComplete(fence);
}

bool CommandManager::CloseCommandList()
{
    assert(mCommandList);
    if (!mCommandList) return false;

    if (mCommandList->IsClosed())
    {
        LOG_MESSAGE("CommandManager: Command list is already closed.");
        return false;
    }

    return mCommandList->Close();
}

bool CommandManager::ResetCommandListWithOwnAlloc(Pso* pso)
{
    assert(mCommandList && mCommandQueueDirect);
    if (!mCommandList || !mCommandQueueDirect) return false;

    if (!mCommandQueueDirect->IsFenceComplete(mCommandList->GetFenceValue()))
    {
        LOG_MESSAGE("CommandManager: Waiting for fence completion...");
        mCommandQueueDirect->WaitForFence(mCommandList->GetFenceValue());
    }

    return mCommandList->ResetWithOwnAlloc(pso, mCommandQueueDirect->GetLastCompletedFenceValue());
}

bool CommandManager::ResetCommandListWithAnotherAlloc(Pso* pso, CommandAllocator* commandAllocator)
{
    assert(mCommandList && mCommandQueueDirect && commandAllocator);
    if (!mCommandList || !mCommandQueueDirect || !commandAllocator) return false;

    if (!mCommandQueueDirect->IsFenceComplete(commandAllocator->GetFenceValue()))
    {
        LOG_MESSAGE("CommandManager: Waiting for fence completion...");
        mCommandQueueDirect->WaitForFence(commandAllocator->GetFenceValue());
    }

    return mCommandList->ResetWithAnotherAlloc(pso, mCommandQueueDirect->GetLastCompletedFenceValue(), commandAllocator);
}

UINT64 CommandManager::GetCurrentQueueFenceValue() const
{
    assert(mCommandQueueDirect);
    return mCommandQueueDirect->GetCurrentFenceValue();
}

UINT64 CommandManager::GetLastCompletedQueueFenceValue() const
{
    assert(mCommandQueueDirect);
    return mCommandQueueDirect->GetLastCompletedFenceValue();
}

bool CommandManager::IsCommandListClosed() const
{
    assert(mCommandList);
    if (!mCommandList) return false;

    return mCommandList->IsClosed();
}

void CommandManager::Initialize(ID3D12Device* device)
{
    mCommandQueueDirect = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT, device);
    mCommandList = std::make_unique<CommandList>(device);
}
