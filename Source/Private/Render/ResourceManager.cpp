#include "ResourceManager.h"

ResourceManager::ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue)
    :
    pDevice(device), 
    pCommandQueue(commandQueue)
{
    assert(pDevice);
    assert(pCommandQueue);
    
    CreateCommandList();

    mTextureManager = std::make_unique<TextureManager>();
    mGeometryManager = std::make_unique<GeometryManager>(pDevice, mCommandList.Get());

    if (pDevice && pCommandQueue && mCommandAllocator && mCommandList && mFence)
    {
        BuildResources();
    }

    mMaterials = mMaterialBuilder.CreateMaterials(mTextures);

}

void ResourceManager::BuildResources() 
{
   // open command list 
    mCommandList->Reset(mCommandAllocator.Get(), nullptr) >> Kds::App::Check;

    mTextureManager->CreateBaseTextures(pDevice, mCommandList.Get());
    mGeometryManager->CreateBaseGeometries(pDevice, mCommandList.Get());

    // add on queue and execute commands
    mCommandList->Close() >> Kds::App::Check;
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    FlushCommandQueue();
}

void ResourceManager::CreateCommandList()
{
    pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(mCommandAllocator.GetAddressOf())) >> Kds::App::Check;

    pDevice->CreateCommandList(0u, D3D12_COMMAND_LIST_TYPE_DIRECT, mCommandAllocator.Get(), nullptr, IID_PPV_ARGS(mCommandList.GetAddressOf())) >> Kds::App::Check;

    mCommandList->Close();

    pDevice->CreateFence(mCurrentFenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(mFence.GetAddressOf())) >> Kds::App::Check;

    assert(mCommandAllocator);
    assert(mCommandList);
    assert(mFence);
}


void ResourceManager::FlushCommandQueue()
{
    ++mCurrentFenceValue;
    pCommandQueue->Signal(mFence.Get(), mCurrentFenceValue) >> Kds::App::Check;

    if (mFence->GetCompletedValue() < mCurrentFenceValue)
    {
        HANDLE eventHandle = CreateEventEx(nullptr, nullptr, false, EVENT_ALL_ACCESS);

        mFence->SetEventOnCompletion(mCurrentFenceValue, eventHandle);

        WaitForSingleObject(eventHandle, INFINITE);
        CloseHandle(eventHandle);
    }
}

