#include "TextureManager.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"
#include <filesystem>
#include "Texture.h"
#include "CommandList.h"

TextureManager::TextureManager()
{
    mTextures.reserve(32);
}

TextureManager::~TextureManager() = default;

void TextureManager::CreateBaseTextures(CommandList* cmdList)
{
    CreateTexture(cmdList, ETextureType::BaseColor, L"wood_crate1", L"../Source/Textures/WoodCrate01.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"tile", L"../Source/Textures/tile.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"grass", L"../Source/Textures/grass.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"bricks", L"../Source/Textures/bricks.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"bricks2", L"../Source/Textures/bricks2.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"default", L"../Source/Textures/default.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"stone", L"../Source/Textures/stone.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"water", L"../Source/Textures/water.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"grassx", L"../Source/Textures/grassx.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"grassy", L"../Source/Textures/grassy.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"firebal", L"../Source/Textures/firebal.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"coldfire", L"../Source/Textures/coldfire.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"metall", L"../Source/Textures/metall.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"droneBaseColor", L"../Source/Textures/droneBaseColor.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"wireFence", L"../Source/Textures/WireFence.dds");
    CreateTexture(cmdList, ETextureType::BaseColor, L"treeArray2", L"../Source/Textures/treeArray2.dds");

    CreateTexture(cmdList, ETextureType::NormalMap, L"default_nmap", L"../Source/Textures/default_nmap.dds");
    CreateTexture(cmdList, ETextureType::NormalMap, L"bricks_nmap", L"../Source/Textures/bricks_nmap.dds");
    CreateTexture(cmdList, ETextureType::NormalMap, L"bricks2_nmap", L"../Source/Textures/bricks2_nmap.dds");
    CreateTexture(cmdList, ETextureType::NormalMap, L"tile_nmap", L"../Source/Textures/tile_nmap.dds");

    CreateTexture(cmdList, ETextureType::CubeMap, L"grasscube1024", L"../Source/Textures/grasscube1024.dds");
}

void TextureManager::CreateTexture(CommandList* cmdList, ETextureType type, const std::wstring& name, const std::wstring& path)
{
    assert(cmdList);

    if (mTextures.contains(name))
    {
        LOG_WARNING("texture ", name, " has already been created");
        assert(0);
        return;
    }

    if (!std::filesystem::exists(path))
    {
        LOG_WARNING("File not found: ", path);
        assert(0);
        return;
    }

    mTextures[name] = std::make_unique<Texture>(type, name, path, cmdList);
}

Texture* TextureManager::GetTexture(const std::wstring& name)
{
    if (!mTextures.contains(name))
    {
        LOG_WARNING("texture ", name, " is not in the container");
        assert(0);
        return nullptr;
    }
    return mTextures.at(name).get();
}
