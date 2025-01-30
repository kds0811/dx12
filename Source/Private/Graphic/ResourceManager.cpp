#include "ResourceManager.h"
#include <stdexcept>

ResourceManager::ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue)
{
    assert(device);
    assert(commandQueue);
    pDevice = device;
    pCommandQueue = commandQueue;
    
    pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())) >> Kds::App::Check;

    pDevice->CreateCommandList(
        0u, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >>
        Kds::App::Check;

    mCommandList->Close();

    pDevice->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())) >> Kds::App::Check;

}