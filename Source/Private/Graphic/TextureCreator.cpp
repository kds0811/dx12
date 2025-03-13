#include "TextureCreator.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"

std::unordered_map<std::string, std::unique_ptr<Texture>> TextureCreator::CreateTextures(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    assert(device);
    assert(cmdList);

    std::unordered_map<std::string, std::unique_ptr<Texture>> result;

    // base color tex
    result["wood_crate1"] = std::make_unique<Texture>(ETextureType::BaseColor, "woodCrate01", L"../Source/Textures/WoodCrate01.dds", device, cmdList);
    result["tile"]  = std::make_unique<Texture>(ETextureType::BaseColor, "tile", L"../Source/Textures/tile.dds", device, cmdList);
    result["grass"] = std::make_unique<Texture>(ETextureType::BaseColor, "grass", L"../Source/Textures/grass.dds", device, cmdList);
    result["bricks"] = std::make_unique<Texture>(ETextureType::BaseColor, "bricks", L"../Source/Textures/bricks.dds", device, cmdList);
    result["default"] = std::make_unique<Texture>(ETextureType::BaseColor, "default", L"../Source/Textures/default.dds", device, cmdList);
    result["stone"] = std::make_unique<Texture>(ETextureType::BaseColor, "stone", L"../Source/Textures/stone.dds", device, cmdList);
    result["water"] = std::make_unique<Texture>(ETextureType::BaseColor, "water", L"../Source/Textures/water.dds", device, cmdList);
    result["grassx"] = std::make_unique<Texture>(ETextureType::BaseColor, "grassx", L"../Source/Textures/grassx.dds", device, cmdList);
    result["grassy"] = std::make_unique<Texture>(ETextureType::BaseColor, "grassy", L"../Source/Textures/grassy.dds", device, cmdList);
    result["firebal"] = std::make_unique<Texture>(ETextureType::BaseColor, "firebal", L"../Source/Textures/firebal.dds", device, cmdList);
    result["coldfire"] = std::make_unique<Texture>(ETextureType::BaseColor, "coldfire", L"../Source/Textures/coldfire.dds", device, cmdList);
    result["metall"] = std::make_unique<Texture>(ETextureType::BaseColor, "metall", L"../Source/Textures/metall.dds", device, cmdList);
    result["droneBaseColor"] = std::make_unique<Texture>(ETextureType::BaseColor, "droneBaseColor", L"../Source/Textures/droneBaseColor.dds", device, cmdList);
    result["wireFence"] = std::make_unique<Texture>(ETextureType::BaseColor, "wireFence", L"../Source/Textures/WireFence.dds", device, cmdList);
    result["treeArray2"] = std::make_unique<Texture>(ETextureType::BaseColor, "treeArray2", L"../Source/Textures/treeArray2.dds", device, cmdList);

    // normal maps
    result["default_nmap"] = std::make_unique<Texture>(ETextureType::NormalMap, "default_nmap", L"../Source/Textures/default_nmap.dds", device, cmdList);

    // sky box
    result["grasscube1024"] = std::make_unique<Texture>(ETextureType::CubeMap, "grasscube1024", L"../Source/Textures/grasscube1024.dds", device, cmdList);

    // add textures
    //texInfos.push_back(TextureInfo{.Type = EMaterialType::WOODCRATE, .FilePath = L"../Source/Textures/WoodCrate01.dds"});
    //texInfos.push_back(TextureInfo{.Type = EMaterialType::TILE, .FilePath = L"../Source/Textures/tile.dds"});
   /* texInfos.push_back(TextureInfo{.Type = EMaterialType::GRASS, .FilePath = L"../Source/Textures/grass.dds"});
    texInfos.push_back(TextureInfo{.Type = EMaterialType::BRICKS, .FilePath = L"../Source/Textures/bricks.dds"});*/
   /* texInfos.push_back(TextureInfo{.Type = EMaterialType::UNKNOWN, .FilePath = L"../Source/Textures/default.dds"});*/
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::STONE, .FilePath = L"../Source/Textures/stone.dds"});
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::WATER, .FilePath = L"../Source/Textures/water.dds"});
   /* texInfos.push_back(TextureInfo{.Type = EMaterialType::SKULLMAT, .FilePath = L"../Source/Textures/default_nmap.dds"});*/
  //  texInfos.push_back(TextureInfo{.Type = EMaterialType::GRASSX, .FilePath = L"../Source/Textures/grassx.dds"});
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::GRASSY, .FilePath = L"../Source/Textures/grassy.dds"});
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::FIREBALL, .FilePath = L"../Source/Textures/fireball.dds"});
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::COLDFIRE, .FilePath = L"../Source/Textures/coldfire.dds"});
  //  texInfos.push_back(TextureInfo{.Type = EMaterialType::METALL, .FilePath = L"../Source/Textures/metall.dds"});
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::DRONEBASECOLOR, .FilePath = L"../Source/Textures/droneBaseColor.dds"});
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::WIREFENCE, .FilePath = L"../Source/Textures/WireFence.dds"});
  //  texInfos.push_back(TextureInfo{.Type = EMaterialType::TREES, .FilePath = L"../Source/Textures/treeArray2.dds"});
   // texInfos.push_back(TextureInfo{.Type = EMaterialType::SKYBOX, .FilePath = L"../Source/Textures/grasscube1024.dds"});

    return result;
}

