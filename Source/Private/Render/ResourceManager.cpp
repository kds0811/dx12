#include "ResourceManager.h"

ResourceManager::ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue) : pDevice(device), pCommandQueue(commandQueue)
{
    assert(pDevice);
    assert(pCommandQueue);

    CreateCommandList();

    mTextureManager = std::make_unique<TextureManager>();
    mGeometryManager = std::make_unique<GeometryManager>(pDevice, mCommandList.Get());
    mMaterialManager = std::make_unique<MaterialManager>();

    if (pDevice && pCommandQueue && mCommandAllocator && mCommandList && mFence)
    {
        BuildResources();
    }
    AddTexturesToStandartMaterial();
}


const Material* ResourceManager::GetMaterial(const std::string& name) const
{
    return mMaterialManager->GetMaterial(name);
}

void ResourceManager::BuildResources()
{
    // open command list
    mCommandList->Reset(mCommandAllocator.Get(), nullptr) >> Kds::App::Check;

    mTextureManager->CreateBaseTextures(pDevice, mCommandList.Get());
    mGeometryManager->CreateBaseGeometries(pDevice, mCommandList.Get());
    mGeometryManager->CreatePrimitiveGeoRenderData();
    
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

void ResourceManager::AddTexturesToStandartMaterial()
{
   // add base color textures ptr to material
    mMaterialManager->SetMaterialBaseColor("wood_crate1", mTextureManager->GetTexture("wood_crate1"));
    mMaterialManager->SetMaterialBaseColor("tile", mTextureManager->GetTexture("tile"));
    mMaterialManager->SetMaterialBaseColor("grass", mTextureManager->GetTexture("grass"));
    mMaterialManager->SetMaterialBaseColor("bricks", mTextureManager->GetTexture("bricks"));
    mMaterialManager->SetMaterialBaseColor("bricks2", mTextureManager->GetTexture("bricks2"));
    mMaterialManager->SetMaterialBaseColor("default", mTextureManager->GetTexture("default"));
    mMaterialManager->SetMaterialBaseColor("stone", mTextureManager->GetTexture("stone"));
    mMaterialManager->SetMaterialBaseColor("grassx", mTextureManager->GetTexture("grassx"));
    mMaterialManager->SetMaterialBaseColor("grassy", mTextureManager->GetTexture("grassy"));
    mMaterialManager->SetMaterialBaseColor("firebal", mTextureManager->GetTexture("firebal"));
    mMaterialManager->SetMaterialBaseColor("coldfire", mTextureManager->GetTexture("coldfire"));
    mMaterialManager->SetMaterialBaseColor("metall", mTextureManager->GetTexture("metall"));
    mMaterialManager->SetMaterialBaseColor("droneBaseColor", mTextureManager->GetTexture("droneBaseColor"));
    mMaterialManager->SetMaterialBaseColor("treeArray2", mTextureManager->GetTexture("treeArray2"));
    mMaterialManager->SetMaterialBaseColor("wireFence", mTextureManager->GetTexture("wireFence"));
    mMaterialManager->SetMaterialBaseColor("water", mTextureManager->GetTexture("water"));
    mMaterialManager->SetMaterialBaseColor("grasscube1024", mTextureManager->GetTexture("grasscube1024"));

    // Add normal maps textures ptr
    mMaterialManager->SetMaterialNormalMap("default", mTextureManager->GetTexture("default_nmap"));
    mMaterialManager->SetMaterialNormalMap("bricks", mTextureManager->GetTexture("bricks_nmap"));
    mMaterialManager->SetMaterialNormalMap("bricks2", mTextureManager->GetTexture("bricks2_nmap"));
    mMaterialManager->SetMaterialNormalMap("tile", mTextureManager->GetTexture("tile_nmap"));
}
