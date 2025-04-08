#include "Buffer.h"

Buffer::Buffer(ID3D12Device* device, std::wstring name, UINT bufferSize, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState) 
{
    auto HeapProperties = CD3DX12_HEAP_PROPERTIES(heapType);
    auto BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(mBufferSize);

    device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &BufferDesc, initialState, nullptr, IID_PPV_ARGS(&mResource)) >> Kds::App::Check;

    mCurrentState = initialState;

    mName = std::move(name);
    mResource->SetName(mName.c_str());
}
