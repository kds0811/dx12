#include "CommandManager.h"
#include "CommandQueue.h"
#include "Pso.h"
#include "CommandAllocator.h"
#include "SwapChain.h"

using namespace Kds::App;

CommandManager::CommandManager(ID3D12Device* device)
{
    Initialize(device);
}

CommandManager::~CommandManager() = default;

CommandList* CommandManager::GetFreeCommandListAndResetIt(Pso* pso)
{
    std::lock_guard<std::mutex> lockGuard(mCommandListMutex);

    if (!mPoolFreeCommandListPtr.empty())
    {
        auto cmdListPtr = mPoolFreeCommandListPtr.top();
        if (mCommandQueueDirect->GetLastCompletedFenceValue() < cmdListPtr->GetFenceValue())
        {
            mCommandQueueDirect->WaitForFence(cmdListPtr->GetFenceValue());
        }

        if (!cmdListPtr->ResetAllocatorAndCommandList(pso, mCommandQueueDirect->GetLastCompletedFenceValue()))
        {
            LOG_ERROR("Error ResetAllocatorAndCommandList");
            return nullptr;
        }

        mPoolFreeCommandListPtr.pop();

        return cmdListPtr;
    }
    else
    {
        // TODO need implement waitng free command list
        return nullptr;
    }
}

UINT64 CommandManager::ReturnAndExecuteCommandList(CommandList* commandList)
{    
    std::lock_guard<std::mutex> lockGuard(mCommandListMutex);
    UINT64 fenceValue = mCommandQueueDirect->ExecuteCommandList(commandList);
    mPoolFreeCommandListPtr.push(commandList);
    return fenceValue;
}

bool CommandManager::FlushCommandQueue()
{
    assert(mCommandQueueDirect);
    if (!mCommandQueueDirect) return false;

    return mCommandQueueDirect->FlushCommandQueue();
}

bool CommandManager::WaitForFence(UINT64 fenceValue)
{
    return mCommandQueueDirect->WaitForFence(fenceValue);
}

void CommandManager::Initialize(ID3D12Device* device)
{
    mCommandQueueDirect = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT, device);

    for (size_t i = 0; i < mNumCommandList; ++i)
    {
        mCommandListStorage[i] = std::make_unique<CommandList>(device, i);
        if (mCommandListStorage[i])
        {
            mPoolFreeCommandListPtr.push(mCommandListStorage[i].get());
        }
        else
        {
            LOG_ERROR("CommandList with ID: ", i, " was not created");
        }
    }
}

ID3D12CommandQueue* CommandManager::GetCommandQueue()
{
    assert(mCommandQueueDirect);
    if (!mCommandQueueDirect) return nullptr;

    return mCommandQueueDirect->GetCommandQueue();
}
