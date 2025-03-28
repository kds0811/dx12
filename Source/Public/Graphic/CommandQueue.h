#pragma once
#include "GraphicCommonHeaders.h"
#include <mutex>

class CommandQueue
{
    UINT64 mCurrentFenceValue = 0;
    UINT64 mLastCompletedFenceValue = 0;
    Microsoft::WRL::ComPtr<ID3D12Fence1> mFence = nullptr;
    Microsoft::WRL::ComPtr<ID3D12CommandQueue> mCommandQueue = nullptr;
    HANDLE mEventHandle = nullptr;
    D3D12_COMMAND_LIST_TYPE mType;
    std::mutex mFenceMutex;
    std::mutex mEventMutex;

public:
    CommandQueue(D3D12_COMMAND_LIST_TYPE Type, ID3D12Device* device);
    ~CommandQueue();
    CommandQueue(const CommandQueue&) = delete;
    CommandQueue& operator=(const CommandQueue&) = delete;
    CommandQueue(CommandQueue&& other) noexcept;
    CommandQueue& operator=(CommandQueue&& other) noexcept;

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