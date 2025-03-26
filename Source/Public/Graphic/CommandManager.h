#pragma once
#include "GraphicCommonHeaders.h"

class CommandManager
{

    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;

public:
    CommandManager();

    void Initialize(ID3D12Device* device);
};