#pragma once
#include "GraphicCommonHeaders.h"
#include <mutex>

class CommandQueue
{
private:
    const D3D12_COMMAND_LIST_TYPE mType;
    UINT64 mCurrentFenceValue = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence1> mFence = nullptr;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;

    std::mutex mFenceMutex;
    std::mutex mEventMutex;

public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE Type);
    ~CommandQueue();

    void Initialize(ID3D12Device* device);
    uint64_t ExecuteCommandList(ID3D12GraphicsCommandList* list);
    void FlushCommandQueue();

private:

};