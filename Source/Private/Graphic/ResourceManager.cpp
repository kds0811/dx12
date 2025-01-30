#include "ResourceManager.h"
#include <stdexcept>

ResourceManager::ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue) : mDevice(device), mCommandQueue(commandQueue)
{
    if (mDevice == nullptr || mCommandQueue == nullptr)
    {
        throw std::runtime_error("Device or Command Queue is not initialized.");
    }  
    
    mDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())) >> Kds::App::Check;

    mDevice->CreateCommandList(
        0u, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >>
        Kds::App::Check;

    mCommandList->Close();

    mDevice->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())) >> Kds::App::Check;

}