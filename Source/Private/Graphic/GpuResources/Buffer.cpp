#include "Buffer.h"
#include "GpuResource.h"
#include "CommandList.h"

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

GpuResource Buffer::CreateDefaultBuffer(const std::wstring& name, CommandList* cmdList, const void* initData, UINT64 byteSize, GpuResource* uploadBuffer)
{
    GpuResource buffer;
    buffer.SetName(name);
    const CD3DX12_RESOURCE_DESC ResDescBuf = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    const auto HeapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    buffer.CreateResource(&HeapPropDefault, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_COMMON);

    const auto HeapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    uploadBuffer->SetName(L"Upload Buffer");
    uploadBuffer->CreateResource(&HeapPropUpload, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_GENERIC_READ);

    // Describe the data we want to copy into the default buffer.
    D3D12_SUBRESOURCE_DATA subResourceData = {};
    subResourceData.pData = initData;
    subResourceData.RowPitch = byteSize;
    subResourceData.SlicePitch = subResourceData.RowPitch;

    buffer.ChangeState(cmdList, D3D12_RESOURCE_STATE_COPY_DEST);

    UpdateSubresources<1>(cmdList->GetCommandList(), buffer.GetResource(), uploadBuffer->GetResource(), 0, 0, 1, &subResourceData);

    buffer.ChangeState(cmdList, D3D12_RESOURCE_STATE_GENERIC_READ);

    return buffer;
}
