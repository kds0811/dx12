#pragma once
#include <string>
#include "GpuResource.h"
#include <memory>
#include <cstdint>

enum class ETextureType : std::uint8_t
{
    BaseColor,
    NormalMap,
    RoughnessMap,
    MetallicMap,
    Default
};

class Texture
{
    ETextureType mType = ETextureType::Default;
    std::string mName;
    std::wstring mFilename;
    int mSrvHeapIndex = -1;

    std::unique_ptr<GpuResource> mResource = nullptr;
    std::unique_ptr<GpuResource> mUploadHeap = nullptr;

public:
    Texture(ETextureType type, std::string name, std::wstring fileName, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    inline ETextureType GetTextureType() noexcept { return mType; }
    inline std::string GetName() { return mName.empty() ? " Unnamed Object " : mName; }
    inline int GetSrvIndex() noexcept { return mSrvHeapIndex; }

    

private:
    void LoadFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};
