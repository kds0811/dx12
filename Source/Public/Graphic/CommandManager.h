#pragma once
#include "GraphicCommonHeaders.h"
#include <array>
#include <mutex>
#include <queue>
#include "CommandList.h"

class CommandQueue;
class Pso;
class CommandAllocator;
class SwapChain;

// Comparator for priority queue of CommandLists. Compares FenceValues.
auto CommandListComparator = [](CommandList* l1, CommandList* l2) { return l1->GetFenceValue() > l2->GetFenceValue(); };

/// \brief Manages a pool of CommandLists and provides an interface to retrieve, reset, and execute them.
///
/// This class is responsible for maintaining a pool of reusable CommandLists. It ensures efficient resource usage by recycling CommandLists after they have been executed. The
/// class also provides thread-safe access to the pool through a mutex.
///
/// Key responsibilities:
/// - Maintaining ownership of all CommandLists (`mCommandListStorage`).
/// - Providing free CommandLists via `GetFreeCommandListAndResetIt`.
/// - Returning executed CommandLists to the pool via `ReturnAndExecuteCommandList`.
/// - Managing the CommandQueue (`mCommandQueueDirect`) for executing commands.
class CommandManager
{
    friend SwapChain;  // friend to access ID3D12CommandQueue* GetCommandQueue()

    static inline std::unique_ptr<CommandQueue> mCommandQueueDirect = nullptr;
    static constexpr UINT mNumCommandList = 10;
    static inline std::array<std::unique_ptr<CommandList>, mNumCommandList> mCommandListStorage;                                          // ownership of resources
    static inline std::priority_queue<CommandList*, std::vector<CommandList*>, decltype(CommandListComparator)> mPoolFreeCommandListPtr;  // Pool of Free Command Lists
    static inline std::mutex mCommandListMutex;

public:
    CommandManager(ID3D12Device* device);
    ~CommandManager();
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    CommandManager(CommandManager&&) noexcept = delete;
    CommandManager& operator=(CommandManager&&) noexcept = delete;

    /// \brief Retrieves a free CommandList from the pool, resets it, and prepares it for reuse.
    /// \param pso Pointer to the Pipeline State Object (PSO) used to reset the CommandList.
    /// \return A pointer to the ready-to-use CommandList, or nullptr if no free CommandList is available.
    [[nodiscard]] static CommandList* GetFreeCommandListAndResetIt(Pso* pso);

    /// \brief Executes the given CommandList and returns it to the pool of free CommandLists.
    /// \param commandList Pointer to the CommandList to execute and return to the pool.
    /// \return The fence value associated with the executed CommandList.
    [[nodiscard]] static UINT64 ReturnAndExecuteCommandList(CommandList* commandList);

    /// \brief Flushes the command queue to ensure all pending commands are executed.
    /// This function checks if the CommandQueue is valid and then calls its FlushCommandQueue method.
    /// Flushing the queue ensures that all previously submitted commands are completed before proceeding.
    [[nodiscard]] static bool FlushCommandQueue();

    /// \brief Waits for a specific fence value to be signaled by the GPU.
    /// \param fenceValue The fence value to wait for.
    /// \return True if the fence value is successfully waited for, false otherwise.
    [[nodiscard]] static bool WaitForFence(UINT64 fenceValue);

private:
    void Initialize(ID3D12Device* device);
    static ID3D12CommandQueue* GetCommandQueue();
};