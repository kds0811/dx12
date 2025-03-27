#pragma once
#include "GraphicCommonHeaders.h"

class Pso;

class CommandList
{
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAlloc;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    bool bIsClossed = false;

public:
    CommandList(ID3D12Device* device);

    [[nodiscard]] inline ID3D12GraphicsCommandList* GetCommandList() const noexcept { return mCommandList.Get(); }

    [[nodiscard]] inline bool IsClosed() const noexcept { return bIsClossed; }
    void Close();

    void ResetWithOwnAlloc(Pso* pso);

    void ResetWithAnotherAlloc(ID3D12CommandAllocator* alloc, Pso* pso);

private:
    void Initialize(ID3D12Device* device);
};
