#pragma once
#include "Buffer.h"

class VertexBuffer : public Buffer
{
public:
    VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT vertexCount, UINT stride);
    ~VertexBuffer() override = default;

    [[nodiscard]] inline UINT GetVertexCount() const noexcept { return mVertexCount; }
    [[nodiscard]] inline UINT GetVertexStride() const noexcept { return mVertexStride; }

    [[nodiscard]] D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;

private:
    UINT mVertexCount = 0;   
    UINT mVertexStride = 0; 
};
