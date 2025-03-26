#include "CommandQueue.h"

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
    }
}

uint64_t CommandQueue::ExecuteCommandList(ID3D12GraphicsCommandList* list)
{
    std::lock_guard<std::mutex> LockGuard(mFenceMutex);

    list->Close() >> Kds::App::Check;
    ID3D12CommandList* cmdsLists[] = {list};
    mCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);

    ++mCurrentFenceValue;

    mCommandQueue->Signal(mFence.Get(), mCurrentFenceValue);

    return mCurrentFenceValue;
}

void CommandQueue::FlushCommandQueue()
{
    std::lock_guard<std::mutex> LockGuard(mFenceMutex);

    ++mCurrentFenceValue;
    mCommandQueue->Signal(mFence.Get(), mCurrentFenceValue) >> Kds::App::Check;

    ++mCurrentFenceValue;
    mCommandQueue->Signal(mFence.Get(), mCurrentFenceValue) >> Kds::App::Check;

    if (mFence->GetCompletedValue() < mCurrentFenceValue)
    {
        mFence->SetEventOnCompletion(mCurrentFenceValue, mEventHandle);
        WaitForSingleObject(mEventHandle, INFINITE);
    }
    LOG_MESSAGE("Command queue has been flushed.");

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
}