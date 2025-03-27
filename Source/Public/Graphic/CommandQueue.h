#pragma once
#include "GraphicCommonHeaders.h"
#include <mutex>

class CommandQueue
{
    const D3D12_COMMAND_LIST_TYPE mType;
    UINT64 mCurrentFenceValue = 0;
    UINT64 mLastCompletedFenceValue = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence1> mFence = nullptr;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;

    std::mutex mFenceMutex;
    std::mutex mEventMutex;

    HANDLE mEventHandle = nullptr;

public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE Type, ID3D12Device* device);
    ~CommandQueue();

    
    UINT64 ExecuteCommandList(ID3D12GraphicsCommandList* list);
    void WaitForFence(UINT64 fenceValue);
    void FlushCommandQueue();
    bool IsFenceComplete(UINT64 FenceValue);
    [[nodiscard]] ID3D12CommandQueue* GetCommandQueue() const noexcept { return mCommandQueue.Get(); }
    [[nodiscard]] bool IsFenceComplete(UINT64 fenceValue) noexcept { return mFence->GetCompletedValue() >= fenceValue; }
    [[nodiscard]] D3D12_COMMAND_LIST_TYPE GetType() const noexcept { return mType; }
    [[nodiscard]] UINT64 GetCurrentFenceValue() const noexcept { return mCurrentFenceValue; }
    [[nodiscard]] UINT64 GetLastCompletedFenceValue() const noexcept { return mLastCompletedFenceValue; }


private:
    void Initialize(ID3D12Device* device);
};