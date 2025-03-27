#include "DescriptorManager.h"
#include <cassert>

DescriptorManager::DescriptorManager(ID3D12Device* device)
{
    Initialize(device);
}

void DescriptorManager::Initialize(ID3D12Device* device) 
{
    assert(device);


    mRtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    mDsvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    mCbvSrvUavDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


}
