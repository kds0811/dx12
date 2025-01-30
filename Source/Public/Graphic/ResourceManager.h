#pragma once
#include "D3D12Utils.h"

class ResourceManager
{
    // pointer to Graphic fields
    ID3D12Device8* mDevice = nullptr;
    ID3D12CommandQueue* mCommandQueue = nullptr;
    // own fields
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator = nullptr;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> mCommandList = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence = nullptr;
    UINT64 mFenceValue = 0;

public:
    ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue);
};