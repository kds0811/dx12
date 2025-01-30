#pragma once
#include "D3D12Utils.h"

class ResourceManager
{
    // pointer to Graphic fields
    ID3D12Device* mDevice = nullptr;
    ID3D12CommandQueue* mCommandQueue = nullptr;
    // own fields
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator = nullptr;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence = nullptr;
    UINT64 m_fenceValue = 0;

public:
    explicit ResourceManager(ID3D12Device* device, ID3D12CommandQueue* commandQueue);
};