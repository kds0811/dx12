#include "TextureManager.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"


TextureManager::TextureManager(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
    :
    pDevice(device),
    pCommandList(cmdList)
{
    assert(pDevice);
    assert(pCommandList);

    CreateBaseTextures();
}

void TextureManager::CreateBaseTextures()
{

    // base color tex
    mTextures["wood_crate1"] = std::make_unique<Texture>(ETextureType::BaseColor, "woodCrate01", L"../Source/Textures/WoodCrate01.dds", pDevice, pCommandList);
    mTextures["tile"] = std::make_unique<Texture>(ETextureType::BaseColor, "tile", L"../Source/Textures/tile.dds", pDevice, pCommandList);
    mTextures["grass"] = std::make_unique<Texture>(ETextureType::BaseColor, "grass", L"../Source/Textures/grass.dds", pDevice, pCommandList);
    mTextures["bricks"] = std::make_unique<Texture>(ETextureType::BaseColor, "bricks", L"../Source/Textures/bricks.dds", pDevice, pCommandList);
    mTextures["default"] = std::make_unique<Texture>(ETextureType::BaseColor, "default", L"../Source/Textures/default.dds", pDevice, pCommandList);
    mTextures["stone"] = std::make_unique<Texture>(ETextureType::BaseColor, "stone", L"../Source/Textures/stone.dds", pDevice, pCommandList);
    mTextures["water"] = std::make_unique<Texture>(ETextureType::BaseColor, "water", L"../Source/Textures/water.dds", pDevice, pCommandList);
    mTextures["grassx"] = std::make_unique<Texture>(ETextureType::BaseColor, "grassx", L"../Source/Textures/grassx.dds", pDevice, pCommandList);
    mTextures["grassy"] = std::make_unique<Texture>(ETextureType::BaseColor, "grassy", L"../Source/Textures/grassy.dds", pDevice, pCommandList);
    mTextures["firebal"] = std::make_unique<Texture>(ETextureType::BaseColor, "firebal", L"../Source/Textures/firebal.dds", pDevice, pCommandList);
    mTextures["coldfire"] = std::make_unique<Texture>(ETextureType::BaseColor, "coldfire", L"../Source/Textures/coldfire.dds", pDevice, pCommandList);
    mTextures["metall"] = std::make_unique<Texture>(ETextureType::BaseColor, "metall", L"../Source/Textures/metall.dds", pDevice, pCommandList);
    mTextures["droneBaseColor"] = std::make_unique<Texture>(ETextureType::BaseColor, "droneBaseColor", L"../Source/Textures/droneBaseColor.dds", pDevice, pCommandList);
    mTextures["wireFence"] = std::make_unique<Texture>(ETextureType::BaseColor, "wireFence", L"../Source/Textures/WireFence.dds", pDevice, pCommandList);
    mTextures["treeArray2"] = std::make_unique<Texture>(ETextureType::BaseColor, "treeArray2", L"../Source/Textures/treeArray2.dds", pDevice, pCommandList);

    // normal maps
    mTextures["default_nmap"] = std::make_unique<Texture>(ETextureType::NormalMap, "default_nmap", L"../Source/Textures/default_nmap.dds", pDevice, pCommandList);

    // sky box
    mTextures["grasscube1024"] = std::make_unique<Texture>(ETextureType::CubeMap, "grasscube1024", L"../Source/Textures/grasscube1024.dds", pDevice, pCommandList);
}

