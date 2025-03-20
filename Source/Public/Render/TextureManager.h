#pragma once
#include "D3D12Utils.h"
#include "Texture.h"
#include <cassert>
#include "Logger.h"

class TextureManager
{
    using iterator = std::unordered_map<std::string, std::unique_ptr<Texture>>::iterator;
    using const_iterator = std::unordered_map<std::string, std::unique_ptr<Texture>>::const_iterator;

    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

public:
    TextureManager();

    void CreateBaseTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    void CreateTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, ETextureType type, const std::string& name, const std::wstring& path);

    [[nodiscard]] inline const Texture* GetTexture(const std::string& name) const
    {
        if (!mTextures.contains(name))
        {
            Log::LogWarning("texture " + name + " is not in the container");
            assert(0);
            return nullptr;
        }
        return mTextures.at(name).get();
    }

    [[nodiscard]] inline std::pair<iterator, iterator> GetIterators() noexcept
    {
        if (mTextures.empty())
        {
            Log::LogWarning("Texture container is empty.");
            assert(0);
            return {};
        }
        return std::make_pair(mTextures.begin(), mTextures.end());
    }

    [[nodiscard]] inline const std::pair<const_iterator, const_iterator> GetConstIterators() const noexcept
    {
        if (mTextures.empty())
        {
            Log::LogWarning("Texture container is empty.");
            assert(0);
            return {};
        }
        return std::make_pair(mTextures.cbegin(), mTextures.cend());
    }

    [[nodiscard]] inline size_t GetTextureSize() const noexcept { return mTextures.size();}

};