#pragma once
#include "GraphicCommonHeaders.h"

class CommandAllocator
{
    UINT64 mFence = 0;
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAlloc = nullptr;

public:
    CommandAllocator(ID3D12Device* device);

    inline UINT64 GetFenceValue() noexcept { return mFence; }
    inline void SetFenceValue(UINT64 value) noexcept { mFence = value; }
    ID3D12CommandAllocator* GetCommandListAllocator();
    ID3D12CommandAllocator* GetCommandListAllocator() const;

    bool ResetCommandAllocatorIfFenceComplited(UINT64 fence);

private:
    void Initialize(ID3D12Device* device);
};