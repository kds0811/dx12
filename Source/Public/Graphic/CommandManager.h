#pragma once
#include "GraphicCommonHeaders.h"
#include <array>
#include <mutex>
#include <queue>


class CommandQueue;
class Pso;
class CommandAllocator;
class CommandList;

using VectorCommandListPtr = std::vector<CommandList*>;
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
    static inline std::unique_ptr<CommandQueue> mCommandQueueDirect;

    static constexpr UINT mNumCommandList = 10;

    static inline std::array<std::unique_ptr<CommandList>, mNumCommandList> mCommandListStorage;  // ownership of resources

    static inline std::priority_queue<CommandList*, VectorCommandListPtr, decltype(CommandListComparator)> mPoolFreeCommandListPtr;  // Pool of Free Command Lists 

    static inline std::mutex mGetCommandListMutex;

public:
    CommandManager(ID3D12Device* device);
    ~CommandManager();
    CommandManager(const CommandManager&) = delete;
    CommandManager& operator=(const CommandManager&) = delete;
    CommandManager(CommandManager&& rhs) noexcept = delete;
    CommandManager& operator=(CommandManager&& rhs) noexcept = delete;

    /// \brief Retrieves a free CommandList from the pool, resets it, and prepares it for reuse.
    /// \param pso Pointer to the Pipeline State Object (PSO) used to reset the CommandList.
    /// \return A pointer to the ready-to-use CommandList, or nullptr if no free CommandList is available.
    static CommandList* GetFreeCommandListAndResetIt(Pso* pso);

    /// \brief Executes the given CommandList and returns it to the pool of free CommandLists.
    /// \param commandList Pointer to the CommandList to execute and return to the pool.
    static void ReturnAndExecuteCommandList(CommandList* commandList);

    /// \brief Flushes the command queue to ensure all pending commands are executed.
    /// 
    /// This function checks if the CommandQueue is valid and then calls its FlushCommandQueue method.
    /// Flushing the queue ensures that all previously submitted commands are completed before proceeding.
    static void FlushCommandQueue();

private:
    void Initialize(ID3D12Device* device);
};