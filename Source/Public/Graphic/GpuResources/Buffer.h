#pragma once
#include "GraphicCommonHeaders.h"

class GpuResource;
class CommandList;

class Buffer
{
    std::unique_ptr<GpuResource> mVertexBufferResource = nullptr;
    std::unique_ptr<GpuResource> mIndexBufferResource = nullptr;
    std::unique_ptr<GpuResource> mVertexBufferUploader = nullptr;
    std::unique_ptr<GpuResource> mIndexBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mVertexBufferBlob = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mIndexBufferBlob = nullptr;



    UINT mBufferSize = 0;

public:
    Buffer();
    ~Buffer();
    Buffer(ID3D12Device* device, std::wstring name, UINT bufferSize, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState);

    [[nodiscard]] inline UINT GetBufferSize() const noexcept { return mBufferSize; }

private:
    GpuResource CreateDefaultBuffer(const std::wstring& name, CommandList* cmdList, const void* initData, UINT64 byteSize, GpuResource* uploadBuffer);
};
