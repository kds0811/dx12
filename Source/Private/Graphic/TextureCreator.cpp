#include "TextureCreator.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"

std::unordered_map<EMaterialType, std::unique_ptr<Texture>> TextureCreator::CreateTextures(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    assert(device);
    assert(cmdList);

    std::unordered_map<EMaterialType, std::unique_ptr<Texture>> result;

    std::vector<TextureInfo> texInfos;

    // add textures
    texInfos.push_back(TextureInfo{.Type = EMaterialType::WOODCRATE, .FilePath = L"../Source/Textures/WoodCrate01.dds"});
    texInfos.push_back(TextureInfo{.Type = EMaterialType::TILE, .FilePath = L"../Source/Textures/tile.dds"});
    texInfos.push_back(TextureInfo{.Type = EMaterialType::GRASS, .FilePath = L"../Source/Textures/grass.dds"});
    texInfos.push_back(TextureInfo{.Type = EMaterialType::BRICKS, .FilePath = L"../Source/Textures/bricks.dds"});
    texInfos.push_back(TextureInfo{.Type = EMaterialType::UNKNOWN, .FilePath = L"../Source/Textures/default_nmap.dds"});
    texInfos.push_back(TextureInfo{.Type = EMaterialType::STONE, .FilePath = L"../Source/Textures/stone.dds"});
    texInfos.push_back(TextureInfo{.Type = EMaterialType::WATER, .FilePath = L"../Source/Textures/water1.dds"});


    for (const auto& info : texInfos)
    {
        result[info.Type] = CreateTexture(info, device, cmdList);
    }

    return result;
}

std::unique_ptr<Texture> TextureCreator::CreateTexture(const TextureInfo& info, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    auto result = std::make_unique<Texture>();
    result->Type = info.Type;
    result->Filename = info.FilePath;

    DirectX::CreateDDSTextureFromFile12(device, cmdList, result->Filename.c_str(), result->Resource, result->UploadHeap) >>
        Kds::App::Check;

    return result;
}
