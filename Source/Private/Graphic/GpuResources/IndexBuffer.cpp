#include "IndexBuffer.h"
#include "D3D12Utils.h"

IndexBuffer::IndexBuffer(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const void* initData, UINT indexCount, DXGI_FORMAT format)
{
    Microsoft::WRL::ComPtr<ID3D12Resource> uploadBuffer;
    mResource = D3D12Utils::CreateDefaultBuffer(device, cmdList, initData, mBufferSize, uploadBuffer);

    // Устанавливаем GPU-адрес
    mGpuVirtualAddress = mResource->GetGPUVirtualAddress();
}

D3D12_INDEX_BUFFER_VIEW IndexBuffer::GetIndexBufferView() const
{
    D3D12_INDEX_BUFFER_VIEW ibv{};
    ibv.BufferLocation = mGpuVirtualAddress;
    ibv.Format = mIndexFormat;
    ibv.SizeInBytes = mBufferSize;

    return ibv;
}
