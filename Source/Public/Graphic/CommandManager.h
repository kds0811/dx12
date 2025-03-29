#pragma once
#include "GraphicCommonHeaders.h"

class CommandQueue;
class CommandList;
class Pso;
class CommandAllocator;

class CommandManager
{
    std::unique_ptr<CommandQueue> mCommandQueueDirect;
    std::unique_ptr<CommandList> mCommandList;

public:
    CommandManager(ID3D12Device* device);
    ~CommandManager();
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    CommandManager(CommandManager&& rhs) noexcept;
    CommandManager& operator=(CommandManager&& rhs) noexcept;

    [[nodiscard]] bool ExecuteCommandList();
    void FlushCommandQueue();
    void WaitForExternalFence(UINT64 fenceValue);
    void WaitForInternalFence();
    [[nodiscard]] bool IsExternalFenceComplete(UINT64 fenceValue);
    [[nodiscard]] bool IsInternalFenceComplete();
    [[nodiscard]] bool CloseCommandList();
    [[nodiscard]] bool ResetCommandListWithOwnAlloc(Pso* pso);
    [[nodiscard]] bool ResetCommandListWithAnotherAlloc(Pso* pso, CommandAllocator* commandAllocator);
    [[nodiscard]] UINT64 GetCurrentQueueFenceValue() const;
    [[nodiscard]] UINT64 GetLastCompletedQueueFenceValue() const;
    [[nodiscard]] bool IsCommandListClosed() const;

private:
    void Initialize(ID3D12Device* device);
};