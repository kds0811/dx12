#include "CommandManager.h"
#include "CommandQueue.h"

#include "Pso.h"
#include "CommandAllocator.h"

using namespace Kds::App;

CommandManager::CommandManager(ID3D12Device* device)
{
    Initialize(device);
}

CommandManager::~CommandManager() = default;

CommandList* CommandManager::GetFreeCommandList()
{
    std::lock_guard<std::mutex> LockGuard(mGetCommandListMutex);

    if (!mFreeCommandListPtrPool.empty())
    {
        auto cmdListPtr = mFreeCommandListPtrPool.top();
        mFreeCommandListPtrPool.pop();
        return cmdListPtr;
    }
    else
    {
        // TODO need implement waitng free command list
        return nullptr;
    }
}

void CommandManager::ReturnAndExecuteCommandList(CommandList* commandList)
{
    mCommandQueueDirect->ExecuteCommandList(commandList);
    mFreeCommandListPtrPool.push(commandList);
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

    for (size_t i = 0; i < mNumCommandList; ++i)
    {
        mCommandListStorage[i] = std::make_unique<CommandList>(device);
        if (mCommandListStorage[i])
        {
            mCommandListStorage[i]->SetID(i);
            mFreeCommandListPtrPool.push(mCommandListStorage[i].get());
        }
        else
        {
            LOG_ERROR("CommandList with Index: ", i, " was not created");
        }
    }
}
