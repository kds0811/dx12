#pragma once
#include "GpuResource.h"


class Buffer : public GpuResource
{
protected:
    UINT mBufferSize = 0; 

public:
    Buffer() = default;
    Buffer(ID3D12Device* device, std::wstring name, UINT bufferSize, D3D12_HEAP_TYPE heapType, D3D12_RESOURCE_STATES initialState);
    virtual ~Buffer() = default;

    [[nodiscard]] inline UINT GetBufferSize() const noexcept { return mBufferSize; }
};
