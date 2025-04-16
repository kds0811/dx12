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

public:
    Texture(ETextureType type, const std::wstring& name, std::wstring fileName, CommandList* cmdList);
    ~Texture();
    Texture(const Texture& rhs) = delete;
    Texture& operator=(const Texture& rhs) = delete;
    Texture(const Texture&& rhs) noexcept = delete;
    Texture& operator=(const Texture&& rhs) noexcept = delete;

    [[nodiscard]] inline ETextureType GetTextureType() noexcept { return mType; }
    [[nodiscard]] inline DescriptorHandle& GetSrvDescriptorHandle() noexcept { return mSrvDescriptorHandle; }
    [[nodiscard]] bool IsInitialized() const noexcept { return ResourceIsInitialized() && !mSrvDescriptorHandle.IsNull(); }

private:
    void LoadFromFile(const std::wstring& name, CommandList* cmdList);

    void BuildDescriptors();
};
