#include "Buffer.h"
#include "GpuResource.h"

using namespace Microsoft::WRL;

Buffer::Buffer() 
{
    mVertexBufferResource = std::make_unique<GpuResource>();
    mIndexBufferResource = std::make_unique<GpuResource>();
    mVertexBufferUploader = std::make_unique<GpuResource>();
    mIndexBufferUploader = std::make_unique<GpuResource>();
}

Buffer::~Buffer() = default;

Buffer::Buffer(ID3D12Device* device, std::wstring name, UINT bufferSize, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState)
{
    auto HeapProperties = CD3DX12_HEAP_PROPERTIES(heapType);
    auto BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(mBufferSize);

    device->CreateCommittedResource(&HeapProperties, D3D12_HEAP_FLAG_NONE, &BufferDesc, initialState, nullptr, IID_PPV_ARGS(&mResource)) >> Kds::App::Check;

    mCurrentState = initialState;

    mName = std::move(name);
    mResource->SetName(mName.c_str());
}

GpuResource Buffer::CreateDefaultBuffer(ID3D12Device* device, CommandList* cmdList, const void* initData, UINT64 byteSize, GpuResource* uploadBuffer)
{
    GpuResource buffer;
    const CD3DX12_RESOURCE_DESC ResDescBuf = CD3DX12_RESOURCE_DESC::Buffer(byteSize);

    // Create the actual default buffer resource.
    const auto HeapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    device->CreateCommittedResource(&HeapPropDefault, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_COMMON, nullptr, IID_PPV_ARGS(buffer.GetAddressOf())) >> Kds::App::Check;

    // In order to copy CPU memory data into our default buffer, we need to create
    // an intermediate upload heap.
    const auto HeapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    device->CreateCommittedResource(&HeapPropUpload, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(uploadBuffer.GetAddressOf())) >>
        Check;

    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    const auto ResBarStateToCopy = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);

    cmdList->ResourceBarrier(1, &ResBarStateToCopy);
    UpdateSubresources<1>(cmdList, defaultBuffer.Get(), uploadBuffer.Get(), 0, 0, 1, &subResourceData);

    const auto ResBarCopyToState = CD3DX12_RESOURCE_BARRIER::Transition(defaultBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_GENERIC_READ);
    cmdList->ResourceBarrier(1, &ResBarCopyToState);

    return buffer;
}
