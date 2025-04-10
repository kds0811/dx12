#pragma once
#include "GraphicCommonHeaders.h"
#include <mutex>

class CommandList;
class CommandManager;

/// \brief Represents a Direct3D 12 command queue and manages GPU synchronization using fences.
///
/// This class encapsulates the functionality of a Direct3D 12 command queue, including executing CommandLists, managing fences for synchronization, and ensuring that GPU resources
/// are properly synchronized with the CPU.
///
/// Key responsibilities:
/// - Executing CommandLists submitted by the application.
/// - Managing GPU/CPU synchronization using fences.
/// - Flushing the command queue to ensure all pending commands are completed.
/// - Waiting for specific fence values to be signaled by the GPU.
class CommandQueue
{
    friend CommandManager;

    UINT64 mCurrentFenceValue = 1;
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

    /// \brief Executes the given CommandList and signals the fence.
    /// \param list Pointer to the CommandList to execute.
    /// \return The fence value associated with the execution.
    [[nodiscard]] UINT64 ExecuteCommandList(CommandList* list);

    /// \brief Waits for a specific fence value to be signaled by the GPU.
    /// \param fenceValue The fence value to wait for.
    /// \return True if the fence value is successfully waited for, false otherwise.
    [[nodiscard]] bool WaitForFence(UINT64 fenceValue);

    /// \brief Flushes the command queue to ensure all pending commands are completed.
    /// \return True if the flush is successful, false otherwise.
    [[nodiscard]] bool FlushCommandQueue();

    /// \brief Checks if a specific fence value has been completed by the GPU.
    /// \param FenceValue The fence value to check.
    /// \return True if the fence value is complete, false otherwise
    [[nodiscard]] bool IsFenceComplete(UINT64 FenceValue);


    [[nodiscard]] inline D3D12_COMMAND_LIST_TYPE GetType() const noexcept { return mType; }
    [[nodiscard]] inline UINT64 GetCurrentFenceValue() const noexcept { return mCurrentFenceValue; }
    [[nodiscard]] inline UINT64 GetLastCompletedFenceValue() const noexcept { return mLastCompletedFenceValue; }

private:
    void Initialize(ID3D12Device* device);
    [[nodiscard]] inline ID3D12CommandQueue* GetCommandQueue() const { return mCommandQueue.Get(); }
    [[nodiscard]] bool IsValidState();
};