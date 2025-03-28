#pragma once
#include "GraphicCommonHeaders.h"
#include <memory>

class Pso;
class CommandAllocator;

class CommandList
{
    std::unique_ptr<CommandAllocator> mCommandAllocator;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mCommandList;
    bool bIsClossed = false;

public:
    CommandList(ID3D12Device* device);

    [[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const;
    [[nodiscard]] ID3D12GraphicsCommandList* GetCommandList();

    [[nodiscard]] inline bool IsClosed() const noexcept { return bIsClossed; }
    void Close();

    bool ResetWithOwnAlloc(Pso* pso, UINT64 queueLastCompletedFenceValue);
    bool ResetWithAnotherAlloc(CommandAllocator* commandAllocator, Pso* pso, UINT64 queueLastCompletedFenceValue);

private:
    void Initialize(ID3D12Device* device);
};
