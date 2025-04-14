#pragma once
#include "GraphicCommonHeaders.h"
#include "Buffer.h"
#include "GpuResource.h"
#include "CommandList.h"


template <typename T>
class Buffer
{
    std::unique_ptr<GpuResource> mBufferResource = nullptr;
    std::unique_ptr<GpuResource> mBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mBufferBlob = nullptr;

    UINT mBufferSize = 0;
    std::wstring mName{};

public:
    Buffer(const std::wstring& name, CommandList* cmdList, const std::vector<T>& resource);

    ~Buffer() = default;

    [[nodiscard]] inline UINT GetBufferSize() const noexcept { return mBufferSize; }
    [[nodiscard]] inline D3D12_GPU_VIRTUAL_ADDRESS GetResourceGpuVirtualAddress() const noexcept { return mBufferResource->GetGpuVirtualAddress(); }

private:
    GpuResource CreateDefaultBuffer(CommandList* cmdList, const void* initData, UINT64 byteSize, GpuResource* uploadBuffer);
};

template <typename T>
inline Buffer<T>::Buffer(const std::wstring& name, CommandList* cmdList, const std::vector<T>& resource)
{
    mName = name;
    mBufferSize = (UINT)resource.size() * sizeof(T);

    D3DCreateBlob(mBufferSize, &mBufferBlob) >> Kds::App::Check;
    CopyMemory(mBufferBlob->GetBufferPointer(), resource.data(), mBufferSize);

    mBufferUploader = std::make_unique<GpuResource>();
    mBufferResource = std::make_unique<GpuResource>(CreateDefaultBuffer(cmdList, resource.data(), mBufferSize, mBufferUploader));
}


template <typename T>
inline GpuResource Buffer<T>::CreateDefaultBuffer(CommandList* cmdList, const void* initData, UINT64 byteSize, GpuResource* uploadBuffer)
{
    assert(cmdList && uploadBuffer && initData);

    GpuResource buffer;
    const CD3DX12_RESOURCE_DESC ResDescBuf = CD3DX12_RESOURCE_DESC::Buffer(byteSize);
    const auto HeapPropDefault = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    buffer.CreateResource(mName, &HeapPropDefault, D3D12_HEAP_FLAG_NONE, &ResDescBuf, D3D12_RESOURCE_STATE_COMMON);

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

