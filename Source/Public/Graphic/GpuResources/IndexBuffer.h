#pragma once
#include "Buffer.h"

class IndexBuffer : public Buffer
{
public:
    IndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT indexCount, DXGI_FORMAT format);
    ~IndexBuffer() override = default;

    [[nodiscard]] inline UINT GetIndexCount() const noexcept { return mIndexCount; }
    [[nodiscard]] inline DXGI_FORMAT GetIndexFormat() const noexcept { return mIndexFormat; }

    [[nodiscard]] D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;

private:
    UINT mIndexCount = 0;
    DXGI_FORMAT mIndexFormat;
};
