#pragma once
#include <unordered_map>
#include <string>
#include "Texture.h"

class CommandList;

class TextureManager
{

    std::unordered_map<std::wstring, std::unique_ptr<Texture>> mTextures;

public:
    TextureManager();
    ~TextureManager();

    void CreateBaseTextures(CommandList* cmdList);

    void CreateTexture(CommandList* cmdList, ETextureType type, const std::wstring& name, const std::wstring& path);

    [[nodiscard]] Texture* GetTexture(const std::wstring& name);

    [[nodiscard]] inline size_t GetTexturesCount() const noexcept { return mTextures.size(); }
};