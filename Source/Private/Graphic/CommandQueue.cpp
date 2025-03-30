#include "CommandQueue.h"
#include <MathHelper.h>
#include "CommandList.h"
#include "CommandManager.h"

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type, ID3D12Device* device) : mType(type)
{
    assert(device);
    Initialize(device);
}

CommandQueue::~CommandQueue()
{
    if (mEventHandle)
    {
        CloseHandle(mEventHandle);
        mEventHandle = nullptr;
    }
}

CommandQueue::CommandQueue(CommandQueue&& other) noexcept
    : mCurrentFenceValue(other.mCurrentFenceValue), mLastCompletedFenceValue(other.mLastCompletedFenceValue), mFence(std::move(other.mFence)),
      mCommandQueue(std::move(other.mCommandQueue)), mEventHandle(other.mEventHandle), mType(other.mType)
{
}

CommandQueue& CommandQueue::operator=(CommandQueue&& other) noexcept
{
    if (this != &other)
    {
        if (mEventHandle)
        {
            CloseHandle(mEventHandle);
            mEventHandle = nullptr;
        }
        mCurrentFenceValue = other.mCurrentFenceValue;
        mLastCompletedFenceValue = other.mLastCompletedFenceValue;
        mFence = std::move(other.mFence);
        mCommandQueue = std::move(other.mCommandQueue);
        mEventHandle = other.mEventHandle;
        mType = other.mType;
    }
    return *this;
}

bool CommandQueue::ExecuteCommandList(CommandList* list)
{
    if (!IsValidState()) return false;

    if (!list)
    {
        LOG_ERROR("CommandQueue::ExecuteCommandList: list pointer is nullptr");
        return false;
    }

    if (!list->Close())
    {
        LOG_MESSAGE("Command list allready is closed");
    }

    std::lock_guard<std::mutex> LockGuard(mFenceMutex);

    ID3D12CommandList* cmdsLists[] = {list->GetCommandList()};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    mCommandQueue->Signal(mFence.Get(), ++mCurrentFenceValue);

    list->SetFenceValue(mCurrentFenceValue);

    return true;
}

void CommandQueue::WaitForFence(UINT64 fenceValue)
{
    if (IsFenceComplete(fenceValue))
    {
        return;
    }
    else
    {
        if (!IsValidState()) return;

        std::lock_guard<std::mutex> LockGuard(mEventMutex);

        mFence->SetEventOnCompletion(fenceValue, mEventHandle);
        WaitForSingleObject(mEventHandle, INFINITE);
        mLastCompletedFenceValue = fenceValue;
    }
}

void CommandQueue::FlushCommandQueue()
{
    if (!IsValidState()) return;

    std::lock_guard<std::mutex> LockGuard(mEventMutex);

    mCommandQueue->Signal(mFence.Get(), ++mCurrentFenceValue) >> Kds::App::Check;

    if (mFence->GetCompletedValue() < mCurrentFenceValue)
    {
        mFence->SetEventOnCompletion(mCurrentFenceValue, mEventHandle);
        WaitForSingleObject(mEventHandle, INFINITE);
    }
    LOG_MESSAGE("Command queue has been flushed.");
    mLastCompletedFenceValue = mCurrentFenceValue;
}

bool CommandQueue::IsFenceComplete(UINT64 FenceValue)
{
    if (FenceValue > mLastCompletedFenceValue)
    {
        mLastCompletedFenceValue = MathHelper::Max(mLastCompletedFenceValue, mFence->GetCompletedValue());
    }
    return FenceValue <= mLastCompletedFenceValue;
}

void CommandQueue::Initialize(ID3D12Device* device)
{
    if (mFence && mCommandQueue)
    {
        LOG_WARNING("CommandQueue is already initialized.");
        return;
    }

    // Create Event
    mEventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);
    if (!mEventHandle)
    {
        LOG_ERROR("Failed to create event handle.");
        assert(false);
    }

    // Create Command Objects
    device->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)) >> Kds::App::Check;

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = mType;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.NodeMask = 0;

    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)) >> Kds::App::Check;
    mCommandQueue->SetName(L"Command Queue");
}

bool CommandQueue::IsValidState()
{
    assert(mFence && mCommandQueue && mEventHandle);

    if (!mFence)
    {
        LOG_ERROR("CommandQueue: Attempt to access a nullptr mFence");
        return false;
    }

    if (!mCommandQueue)
    {
        LOG_ERROR("CommandQueue: Attempt to access a nullptr mCommandQueue");
        return false;
    }

    if (!mEventHandle)
    {
        LOG_ERROR("CommandQueue: Attempt to access a nullptr mEventHandle");
        return false;
    }

    return true;
}
