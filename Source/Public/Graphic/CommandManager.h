#pragma once
#include "GraphicCommonHeaders.h"
#include <array>
#include <mutex>
#include <queue>
#include "CommandList.h"

class CommandQueue;
class Pso;
class CommandAllocator;

using VectorCommandListPtr = std::vector<CommandList*>;
auto CommandListComparator = [](CommandList* l1, CommandList* l2) { return l1->GetFenceValue() > l2->GetFenceValue(); };

class CommandManager
{
    static inline std::unique_ptr<CommandQueue> mCommandQueueDirect;

    static constexpr UINT mNumCommandList = 10;

    static inline std::array<std::unique_ptr<CommandList>, mNumCommandList> mCommandListStorage;  // ownership of resources

    static inline std::priority_queue<CommandList*, VectorCommandListPtr, decltype(CommandListComparator)> mFreeCommandListPtrPool; // Pool of ready to distribute Command Lists 

    static inline std::mutex mGetCommandListMutex;

public:
    CommandManager(ID3D12Device* device);
    ~CommandManager();
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    CommandManager(CommandManager&& rhs) noexcept = delete;
    CommandManager& operator=(CommandManager&& rhs) noexcept = delete;

    static CommandList* GetFreeCommandList();
    static void ReturnAndExecuteCommandList(CommandList* commandList);

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