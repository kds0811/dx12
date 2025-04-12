#include "Buffer.h"
#include "GpuResource.h"
#include "CommandList.h"

using namespace Microsoft::WRL;

Buffer::~Buffer() = default;

GpuResource Buffer::CreateDefaultBuffer( CommandList* cmdList, const void* initData, UINT64 byteSize, GpuResource* uploadBuffer)
{
    GpuResource buffer;
    const CD3DX12_RESOURCE_DESC ResDescBuf = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    const auto HeapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    buffer.CreateResource(mName, & HeapPropDefault, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_COMMON);

    const auto HeapPropUpload = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
    uploadBuffer->CreateResource(L"Upload Buffer", &HeapPropUpload, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_GENERIC_READ);

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
