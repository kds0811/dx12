#include "CommandQueue.h"
#include "Logger.h"
#include "GraphicError.h"
#include <cassert>

CommandQueue::CommandQueue(D3D12_COMMAND_LIST_TYPE type) : mType(type) {}

CommandQueue::~CommandQueue() {}

void CommandQueue::Initialize(ID3D12Device* device)
{
    assert(device);
    device->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&mFence)) >> Kds::App::Check;
 

    // Create Command Objects
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    queueDesc.Type = mType;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    queueDesc.NodeMask = 0;

    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&mCommandQueue)) >> Kds::App::Check;
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
    ++mCurrentFenceValue;
    mCommandQueue->Signal(mFence.Get(), mCurrentFenceValue) >> Kds::App::Check;

    if (mFence->GetCompletedValue() < mCurrentFenceValue)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(mCurrentFenceValue, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
    LOG_MESSAGE("Ñommand queue has been Flushed");
}
