#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include "GraphicError.h"
//#include "D3D12Utils.h"

template <typename T>
class UploadBuffer
{
public:
    UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) : mIsConstantBuffer(isConstantBuffer)
    {
        isConstantBuffer ? mElementByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(T)) : mElementByteSize = sizeof(T);

        auto HeapUploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        auto BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);

        device->CreateCommittedResource(
            &HeapUploadProp, D3D12_HEAP_FLAG_NONE, &BufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mUploadBuffer)) >>
            Kds::App::Check;

        mUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)) >> Kds::App::Check;

        // We do not need to unmap until we are done with the resource.  However, we must not write to
        // the resource while it is in use by the GPU (so we must use synchronization techniques).
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
    ~UploadBuffer()
    {
        if (mUploadBuffer != nullptr) mUploadBuffer->Unmap(0, nullptr);

        mMappedData = nullptr;
    }

    ID3D12Resource* Resource() const { return mUploadBuffer.Get(); }

    void CopyData(int elementIndex, const T& data) { memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T)); }

private:
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadBuffer;
    BYTE* mMappedData = nullptr;

    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;
};