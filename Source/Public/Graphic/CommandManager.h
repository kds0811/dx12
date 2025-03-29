#pragma once
#include "GraphicCommonHeaders.h"

class CommandQueue;
class CommandList;

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

    CommandQueue* GetCommandQueueDirect() const;
    CommandList* GetCommandList() const;

private:
    void Initialize(ID3D12Device* device);
};