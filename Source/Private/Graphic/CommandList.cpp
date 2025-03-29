#include "CommandList.h"
#include "Pso.h"
#include "CommandAllocator.h"

CommandList::CommandList(ID3D12Device* device)
{
    assert(device);
    Initialize(device);
}

CommandList::~CommandList() = default;

CommandList::CommandList(CommandList&& other) noexcept
    :
    mCommandAllocator(std::move(other.mCommandAllocator)),
    mCommandList(std::move(other.mCommandList)),
    bIsClossed(other.bIsClossed)
{
    other.bIsClossed = true;
}

CommandList& CommandList::operator=(CommandList&& other) noexcept
{
    if (this != &other)
    {
        mCommandAllocator = std::move(other.mCommandAllocator);
        mCommandList = std::move(other.mCommandList);
        bIsClossed = other.bIsClossed;
        other.bIsClossed = true;
    }   
    return *this;
}

ID3D12GraphicsCommandList* CommandList::GetCommandList() const
{
    assert(mCommandList);
    if (mCommandList)
    {
        return mCommandList.Get();
    }
    LOG_ERROR("try to get ID3D12GraphicsCommandList* when it is nullptr");
    return nullptr;
}

ID3D12GraphicsCommandList* CommandList::GetCommandList()
{
    assert(mCommandList);
    if (mCommandList)
    {
        return mCommandList.Get();
    }
    LOG_ERROR("try to get ID3D12GraphicsCommandList* when it is nullptr");
    return nullptr;
}

void CommandList::Close()
{
    if (!bIsClossed)
    {
        mCommandList->Close();
        bIsClossed = true;
    }
    else
    {
        LOG_ERROR("attempt to close an already closed Command List");
    }
}

bool CommandList::ResetWithOwnAlloc(Pso* pso, UINT64 queueLastCompletedFenceValue)
{
    if (!bIsClossed)
    {
        LOG_ERROR("Command list allready is open");
        return false;
    }

    if (mCommandAllocator->ResetCommandAllocatorIfFenceComplited(queueLastCompletedFenceValue))
    {
        mCommandList->Reset(mCommandAllocator->GetCommandListAllocator(), pso->GetPso()) >> Kds::App::Check;
        bIsClossed = false;
        return true;
    }
    else
    {
        return false;
    }
}

bool CommandList::ResetWithAnotherAlloc(Pso* pso, UINT64 queueLastCompletedFenceValue, CommandAllocator* commandAllocator)
{
    if (!bIsClossed)
    {
        LOG_ERROR("Command list allready is open");
        return false;
    }

    if (commandAllocator->ResetCommandAllocatorIfFenceComplited(queueLastCompletedFenceValue))
    {
        mCommandList->Reset(commandAllocator->GetCommandListAllocator(), pso->GetPso()) >> Kds::App::Check;
        bIsClossed = false;
        return true;
    }
    else
    {
        return false;
    }
}

void CommandList::Initialize(ID3D12Device* device)
{
    if (mCommandAllocator && mCommandList)
    {
        LOG_WARNING("CommandList is already initialized.");
        return;
    }

    mCommandAllocator = std::make_unique<CommandAllocator>(device);

    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator->GetCommandListAllocator(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >>
        Kds::App::Check;
    mCommandList->SetName(L"Command List");
    LOG_MESSAGE("Command List has been created");

    mCommandList->Close();
    bIsClossed = true;
}