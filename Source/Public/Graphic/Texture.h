#pragma once
#include <string>
#include "GpuResource.h"
#include <memory>
#include <cstdint>

enum class ETexture2DType : std::uint8_t
{
    BaseColor,
    NormalMap,
    RoughnessMap,
    MetallicMap
};

class Texture
{
    ETexture2DType mType;
    std::string mName;
    std::wstring mFilename;
    int mSrvHeapIndex = -1;

    std::unique_ptr<GpuResource> mResource = nullptr;
    std::unique_ptr<GpuResource> mUploadHeap = nullptr;

public:
    Texture(ETexture2DType type, std::string name, std::wstring fileName);
    
    void LoadFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);


};
