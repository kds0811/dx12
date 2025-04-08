#include "VertexBuffer.h"
#include "D3D12Utils.h"

VertexBuffer::VertexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT vertexCount, UINT stride)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
    D3D12Utils::CreateDefaultBuffer(device, cmdList, initData, mBufferSize, uploadBuffer);

    mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
}

D3D12_VERTEX_BUFFER_VIEW VertexBuffer::GetVertexBufferView() const
{
    D3D12_VERTEX_BUFFER_VIEW vbv{};
    vbv.BufferLocation = mGpuVirtualAddress;
    vbv.StrideInBytes = mVertexStride;
    vbv.SizeInBytes = mBufferSize;

    return vbv;
}
