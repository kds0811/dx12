#pragma once
#include "D3D12Utils.h"
#include "Texture.h"

class TextureManager
{
    using iterator = std::unordered_map<std::string, std::unique_ptr<Texture>>::iterator;
    using const_iterator = std::unordered_map<std::string, std::unique_ptr<Texture>>::const_iterator;

    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

public:
    TextureManager();

    void CreateBaseTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    void CreateTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, ETextureType type, std::string name, std::string path);

    Texture* GetTexture(std::string name);

    [[nodiscard]] inline std::pair<iterator, iterator> GetIterators() noexcept
    {
        if (mTextures.empty())
        {
            Log::LogWarning("Texture container is empty.");
        }
        return std::make_pair(mTextures.begin(), mTextures.end());
    }

    [[nodiscard]] inline const std::pair<const_iterator, const_iterator> GetConstIterators() const noexcept
    {
        if (mTextures.empty())
        {
            Log::LogWarning("Texture container is empty.");
        }
        return std::make_pair(mTextures.cbegin(), mTextures.cend());
    }

};