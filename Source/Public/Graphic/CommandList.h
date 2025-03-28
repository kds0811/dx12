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
    ~CommandList();
    CommandList(const CommandList&) = delete;
    CommandList& operator=(const CommandList&) = delete;

    CommandList(CommandList&& other) noexcept;
    CommandList& operator=(CommandList&& other) noexcept;


    [[nodiscard]] ID3D12GraphicsCommandList* GetCommandList() const;
    [[nodiscard]] ID3D12GraphicsCommandList* GetCommandList();

    [[nodiscard]] inline bool IsClosed() const noexcept { return bIsClossed; }
    [[nodiscard]] inline bool IsInitialized() const noexcept { return mCommandAllocator && mCommandList; }
    void Close();

    bool ResetWithOwnAlloc(Pso* pso, UINT64 queueLastCompletedFenceValue);
    bool ResetWithAnotherAlloc(CommandAllocator* commandAllocator, Pso* pso, UINT64 queueLastCompletedFenceValue);

private:
    void Initialize(ID3D12Device* device);
};
