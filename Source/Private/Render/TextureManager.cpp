#include "TextureManager.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"
#include "Logger.h"

TextureManager::TextureManager() {}

void TextureManager::CreateBaseTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // Base color textures
    CreateTexture(device, cmdList, ETextureType::BaseColor, "wood_crate1", "../Source/Textures/WoodCrate01.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "tile", "../Source/Textures/tile.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "grass", "../Source/Textures/grass.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "bricks", "../Source/Textures/bricks.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "default", "../Source/Textures/default.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "stone", "../Source/Textures/stone.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "water", "../Source/Textures/water.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "grassx", "../Source/Textures/grassx.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "grassy", "../Source/Textures/grassy.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "firebal", "../Source/Textures/firebal.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "coldfire", "../Source/Textures/coldfire.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "metall", "../Source/Textures/metall.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "droneBaseColor", "../Source/Textures/droneBaseColor.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "wireFence", "../Source/Textures/WireFence.dds");
    CreateTexture(device, cmdList, ETextureType::BaseColor, "treeArray2", "../Source/Textures/treeArray2.dds");

    // Normal maps
    CreateTexture(device, cmdList, ETextureType::NormalMap, "default_nmap", "../Source/Textures/default_nmap.dds");

    // Skybox
    CreateTexture(device, cmdList, ETextureType::CubeMap, "grasscube1024", "../Source/Textures/grasscube1024.dds");

    std::string message = "base textures have been created";
    Log::LogMessage(message);
}

void TextureManager::CreateTexture(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, ETextureType type, std::string name, std::string path)
{
    if (mTextures.contains(name))
    {
        std::string message = "texture " + name + " has already been created";
        Log::LogWarning(message);
        return;
    }
    mTextures.end();
    mTextures[name] = std::make_unique<Texture>(type, name, path, device, cmdList);
}

Texture* TextureManager::GetTexture(std::string name)
{
    if (!mTextures.contains(name))
    {
        std::string message = "texture " + name + " is not in the container";
        Log::LogWarning(message);
        return nullptr;
    }
   
    return mTextures[name].get();
}

