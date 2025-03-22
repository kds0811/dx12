#include "TextureManager.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"
#include <filesystem>

TextureManager::TextureManager() 
{
    mTextures.reserve(32);
}

void TextureManager::CreateBaseTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{

    // Base color textures
    CreateTexture(device, cmdList, ETextureType::BaseColor, "wood_crate1", L"../Source/Textures/WoodCrate01.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "tile", L"../Source/Textures/tile.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "grass", L"../Source/Textures/grass.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "bricks", L"../Source/Textures/bricks.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "bricks2", L"../Source/Textures/bricks2.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "default", L"../Source/Textures/default.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "stone", L"../Source/Textures/stone.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "water", L"../Source/Textures/water.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "grassx", L"../Source/Textures/grassx.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "grassy", L"../Source/Textures/grassy.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "firebal", L"../Source/Textures/firebal.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "coldfire", L"../Source/Textures/coldfire.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "metall", L"../Source/Textures/metall.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "droneBaseColor", L"../Source/Textures/droneBaseColor.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "wireFence", L"../Source/Textures/WireFence.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "treeArray2", L"../Source/Textures/treeArray2.dds");

    // Normal maps
    CreateTexture(device, cmdList, ETextureType::NormalMap, "default_nmap", L"../Source/Textures/default_nmap.dds");
    CreateTexture(device, cmdList, ETextureType::NormalMap, "bricks_nmap", L"../Source/Textures/bricks_nmap.dds");
    CreateTexture(device, cmdList, ETextureType::NormalMap, "bricks2_nmap", L"../Source/Textures/bricks2_nmap.dds");
    CreateTexture(device, cmdList, ETextureType::NormalMap, "tile_nmap", L"../Source/Textures/tile_nmap.dds");

    // Skybox
    CreateTexture(device, cmdList, ETextureType::BaseColor, "grasscube1024", L"../Source/Textures/grasscube1024.dds");

    std::string message = "base textures have been created";
    Log::LogMessage(message);
}

void TextureManager::CreateTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, ETextureType type, const std::string& name, const std::wstring& path)
{
    if (!device)
    {
        Log::LogWarning("Invalid device pointer.");
        assert(0);
        return;
    }

    if (!cmdList)
    {
        Log::LogWarning("Invalid  command list pointer.");
        assert(0);
        return;
    }

    if (mTextures.contains(name))
    {
        Log::LogWarning("texture " + name + " has already been created");
        assert(0);
        return;
    }

    if (!std::filesystem::exists(path))
    {
        Log::LogWarning("File not found: " + std::string(path.begin(), path.end()));
        assert(0);
        return;
    }

    mTextures[name] = std::make_unique<Texture>(type, name, path, device, cmdList);
}
