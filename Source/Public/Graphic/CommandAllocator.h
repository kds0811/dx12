#pragma once
#include "GraphicCommonHeaders.h"


/// \brief Represents a Direct3D 12 command allocator and manages its lifecycle.
///
/// This class encapsulates a Direct3D 12 command allocator, which is used to allocate memory for recording command lists. It ensures proper synchronization with the GPU using
/// fences and provides methods for resetting the allocator when it is safe to do so.
///
/// Key responsibilities:
/// - Allocating memory for command lists.
/// - Managing GPU/CPU synchronization using fences (`mFence`).
/// - Resetting the allocator when it is no longer in use by the GPU.
/// - Providing access to the underlying ID3D12CommandAllocator object.
class CommandAllocator
{
    UINT64 mFence = 0;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAlloc = nullptr;
    int mID = -1;

public:
    CommandAllocator(ID3D12Device* device, UINT id);
    ~CommandAllocator();
    CommandAllocator(const CommandAllocator&) = delete;
    CommandAllocator& operator=(const CommandAllocator&) = delete;
    CommandAllocator(CommandAllocator&& other) noexcept;
    CommandAllocator& operator=(CommandAllocator&& other) noexcept;

    inline UINT64 GetFenceValue() noexcept { return mFence; }
    inline void SetFenceValue(UINT64 value) noexcept { mFence = value; }
    ID3D12CommandAllocator* GetCommandListAllocator();
    ID3D12CommandAllocator* GetCommandListAllocator() const;

    bool ResetCommandAllocatorIfFenceComplited(UINT64 fence);

private:
    void Initialize(ID3D12Device* device, UINT id);
};