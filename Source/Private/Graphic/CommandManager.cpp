#include "CommandManager.h"

using namespace Kds::App;

CommandManager::CommandManager() {}

void CommandManager::Initialize(ID3D12Device* device)
{  // Create Fence


    device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAlloc.GetAddressOf())) >> Check;

    device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAlloc.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >> Check;

    mCommandList->Close();

    assert(mCommandQueue);
    assert(mCommandAlloc);
    assert(mCommandList);


}
