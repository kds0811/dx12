#pragma once
#include "GpuResource.h"

enum class ETextureType : std::uint8_t
{
    BaseColor,
    NormalMap,
    RoughnessMap,
    MetallicMap,
    Default
};

class Texture : public GpuResource
{
    ETextureType mType = ETextureType::Default;
    std::string mName;
    std::wstring mFilename;

    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadHeap = nullptr;

public:
    Texture(ETextureType type, std::string name, std::wstring fileName, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    inline ETextureType GetTextureType() noexcept { return mType; }
    inline std::string GetName() { return mName.empty() ? " Unnamed Object " : mName; }

private:
    void LoadFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};
