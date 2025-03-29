#include "CommandManager.h"
#include "CommandQueue.h"
#include "CommandList.h"

using namespace Kds::App;

CommandManager::CommandManager(ID3D12Device* device)
{
    Initialize(device);
}

CommandManager::~CommandManager() = default;

CommandManager::CommandManager(CommandManager&& rhs) noexcept
    :
    mCommandQueueDirect(std::move(rhs.mCommandQueueDirect)),
    mCommandList(std::move(rhs.mCommandList))
{}

CommandManager& CommandManager::operator=(CommandManager&& rhs) noexcept
{
    if (this != &rhs)
    {
        mCommandQueueDirect.reset();
        mCommandList.reset();

        mCommandQueueDirect = std::move(rhs.mCommandQueueDirect);
        mCommandList = std::move(rhs.mCommandList);
    }
    return *this;
}

CommandQueue* CommandManager::GetCommandQueueDirect() const
{
    assert(mCommandQueueDirect);
    if (mCommandQueueDirect)
    {
        return mCommandQueueDirect.get();
    }
    LOG_ERROR("try to get CommandQueue* when it is nullptr");
    return nullptr;
}

CommandList* CommandManager::GetCommandList() const
{
    assert(mCommandList);
    if (mCommandList)
    {
        return mCommandList.get();
    }
    LOG_ERROR("try to get CommandList* when it is nullptr");
    return nullptr;
}

void CommandManager::Initialize(ID3D12Device* device)
{
    mCommandQueueDirect = std::make_unique<CommandQueue>(D3D12_COMMAND_LIST_TYPE_DIRECT, device);
    mCommandList = std::make_unique<CommandList>(device);
}
