#pragma once
#include "GpuResource.h"

class CommandList;

enum class ETextureType : std::uint8_t
{
    BaseColor,
    NormalMap,
    RoughnessMap,
    MetallicMap,
    CubeMap,
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
    DescriptorHandle mSrvDescriptorHandle{};
    D3D12_SRV_DIMENSION mSrvDimesion{};

public:
    Texture(ETextureType type, const std::wstring& name, std::wstring fileName, CommandList* cmdList);

    inline ETextureType GetTextureType() noexcept { return mType; }

private:
    void LoadFromFile(const std::wstring& name, CommandList* cmdList);

    void BuildDescriptors();
};
