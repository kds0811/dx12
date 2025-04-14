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
    UploadBuffer(const std::wstring& name, UINT elementCount, bool isConstantBuffer) : mIsConstantBuffer(isConstantBuffer)
    {
        isConstantBuffer ? mElementByteSize = D3D12Utils::CalcConstantBufferByteSize(sizeof(T)) : mElementByteSize = sizeof(T);
        auto HeapUploadProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
        auto BufferDesc = CD3DX12_RESOURCE_DESC::Buffer(mElementByteSize * elementCount);
        CreateResource(name , &HeapUploadProp, D3D12_HEAP_FLAG_NONE, &BufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ);
        GetResource()->Map(0, nullptr, reinterpret_cast<void**>(&mMappedData)) >> Kds::App::Check;
    }

    UploadBuffer(const UploadBuffer& rhs) = delete;
    UploadBuffer& operator=(const UploadBuffer& rhs) = delete;

    ~UploadBuffer()
    {
        if (ResourceIsInitialized())
        {
            GetResource()->Unmap(0, nullptr);
            DestroyResource();
        }
        mMappedData = nullptr;
    }

    void CopyData(int elementIndex, const T& data)
    {
        if (elementIndex < 0)
        {
            LOG_ERROR("Invalid element index in CopyData.");
            assert(false);
        }
        memcpy(&mMappedData[elementIndex * mElementByteSize], &data, sizeof(T));
    }
};