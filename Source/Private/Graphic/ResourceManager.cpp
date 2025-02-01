#include "ResourceManager.h"

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

    assert(mCommandAllocator);
    assert(mCommandList);
    assert(mFence);

    if (pDevice && pCommandQueue && mCommandAllocator && mCommandList && mFence)
    {
        CreateStandartShapeGeometry();
    }

    mMaterials = mMaterialBuilder.CreateMaterials();
}

void ResourceManager::CreateStandartShapeGeometry() 
{
    mCommandList->Reset(mCommandAllocator.Get(), nullptr) >> Kds::App::Check;

    mGeometries["shapeGeo"] = mShapeGeometryBuilder.BuildShapeGeometry(pDevice, mCommandList.Get());
    mGeometries["waterGeo"] = mShapeGeometryBuilder.BuildWavesGeometry(pDevice, mCommandList.Get());

    // add on queue and execute commands
    mCommandList->Close() >> Kds::App::Check;
    ID3D12CommandList* cmdsLists[] = {mCommandList.Get()};
    pCommandQueue->ExecuteCommandLists(_countof(cmdsLists), cmdsLists);
    FlushCommandQueue();

    // check create  shape geometry buufers
    assert(mGeometries["shapeGeo"]->IndexBufferCPU);
    assert(mGeometries["shapeGeo"]->IndexBufferGPU);
    assert(mGeometries["shapeGeo"]->VertexBufferCPU);
    assert(mGeometries["shapeGeo"]->VertexBufferGPU);

    // check create water geo index buffer, Vecrtex buffer will create dynamiclly
    assert(mGeometries["waterGeo"]->IndexBufferCPU);
    assert(mGeometries["waterGeo"]->IndexBufferGPU);
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
