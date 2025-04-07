#pragma once
#include "GraphicCommonHeaders.h"
#include "GpuResource.h"

template <typename T>
class UploadBuffer : public GpuResource
{
    BYTE* mMappedData = nullptr;
    UINT mElementByteSize = 0;
    bool mIsConstantBuffer = false;

public:
    UploadBuffer(ID3D12Device* device, UINT elementCount, bool isConstantBuffer) : mIsConstantBuffer(isConstantBuffer)
    {
        isConstantBuffer ? mElementByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(T)) : mElementByteSize = sizeof(T);

        auto HeapUploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);

        auto BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);

        device->CreateCommittedResource(&HeapUploadProp, D3D12_HEAP_FLAG_NONE, &BufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&mResource)) >>
            Kds::App::Check;

        mCurrentState = D3D12_RESOURCE_STATE_GENERIC_READ;

        mResource->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)) >> Kds::App::Check;

        if (mResource)
        {
            mResource->SetName(L"UploadBuffer");
        }
        else
        {
            LOG_ERROR("UploadBuffer creation failed");
        }
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

    ~UploadBuffer()
    {
        if (mResource)
        {
            mResource->Unmap(0, nullptr);
            DestroyResource();
        }
        mMappedData = nullptr;
    }
    void CopyData(int elementIndex, const T& data) { memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T)); }
};