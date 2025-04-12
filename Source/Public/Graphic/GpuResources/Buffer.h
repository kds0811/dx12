#pragma once
#include "GraphicCommonHeaders.h"

class GpuResource;
class CommandList;

class Buffer
{
    std::unique_ptr<GpuResource> mBufferResource = nullptr;
    std::unique_ptr<GpuResource> mBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> mBufferBlob = nullptr;

    UINT mBufferSize = 0;
    std::wstring mName{};

public:
    ~Buffer();

    template <typename T>
    Buffer(const std::wstring& name, CommandList* cmdList, const std::vector<T>& resource);

    [[nodiscard]] inline UINT GetBufferSize() const noexcept { return mBufferSize; }

private:
    GpuResource CreateDefaultBuffer(CommandList* cmdList, const void* initData, UINT64 byteSize, GpuResource* uploadBuffer);
};

template <typename T>
inline Buffer::Buffer(const std::wstring& name, CommandList* cmdList, const std::vector<T>& resource)
{
    mName = name;
    const UINT bufferByteSize = (UINT)resource.size() * sizeof(T);
    
    D3DCreateBlob(bufferByteSize, &mBufferBlob) >> Kds::App::Check;
    CopyMemory(mBufferBlob->GetBufferPointer(), resource.data(), bufferByteSize);

    mBufferUploader = std::make_unique<GpuResource>();
    mBufferResource = std::make_unique<GpuResource>(CreateDefaultBuffer(cmdList, resource.data(), bufferByteSize, mBufferUploader));
}
