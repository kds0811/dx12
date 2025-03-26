#include "CommandList.h"
#include "Pso.h"


CommandList::CommandList() {}

void CommandList::Initialize(ID3D12Device* device) 
{
    assert(device);
    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAlloc.GetAddressOf())) >> Kds::App::Check;
    mCommandAlloc->SetName(L"Command List Allocator");
    LOG_MESSAGE("Command List Allocator has been created");

    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAlloc.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >> Kds::App::Check;
    mCommandList->SetName(L"Command List");
    LOG_MESSAGE("Command List has been created");

    mCommandList->Close();
    bIsClossed = true;
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

void CommandList::ResetWithOwnAlloc(Pso* pso)
{
    mCommandAlloc->Reset() >> Kds::App::Check;
    mCommandList->Reset(mCommandAlloc.Get(), pso->GetPso()) >> Kds::App::Check;
    bIsClossed = false;
}

void CommandList::ResetWithAnotherAlloc(ID3D12CommandAllocator* alloc, Pso* pso) 
{
    alloc->Reset() >> Kds::App::Check;
    mCommandList->Reset(alloc, pso->GetPso()) >> Kds::App::Check;
    bIsClossed = false;
}
