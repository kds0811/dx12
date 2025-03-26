#pragma once
#include "GraphicCommonHeaders.h"
#include <mutex>

class CommandQueue
{
    const D3D12_COMMAND_LIST_TYPE mType;
    UINT64 mCurrentFenceValue = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence1> mFence = nullptr;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;

    std::mutex mFenceMutex;

    HANDLE mEventHandle = nullptr;

public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE Type, ID3D12Device* device);
    ~CommandQueue();

    
    uint64_t ExecuteCommandList(ID3D12GraphicsCommandList* list);
    void FlushCommandQueue();

    [[nodiscard]] bool IsFenceComplete(uint64_t fenceValue) const noexcept { return mFence->GetCompletedValue() >= fenceValue; }

private:
    void Initialize(ID3D12Device* device);
};