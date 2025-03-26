#pragma once
#include "GraphicCommonHeaders.h"

class Pso;

class CommandList
{
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    bool bIsClossed = false;

public:
    CommandList();

    [[nodiscard]] inline ID3D12GraphicsCommandList* GetCommandList() const noexcept { return mCommandList.Get(); }
    void Initialize(ID3D12Device* device);

    void Close();
    void ResetWithOwnAlloc(Pso* pso);
    void ResetWithAnotherAlloc(ID3D12CommandAllocator* alloc, Pso* pso);

};
