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
    std::wstring mFilename;
    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    Microsoft::WRL::ComPtr<ID3D12Resource> mUploadHeap = nullptr;
    ETextureType mType = ETextureType::Default;

public:
    Texture(ETextureType type, std::wstring name, std::wstring fileName, ID3D12GraphicsCommandList* cmdList);

    inline ETextureType GetTextureType() noexcept { return mType; }

private:
    void LoadFromFile(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};
