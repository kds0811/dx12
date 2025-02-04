#include "TextureCreator.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"

std::unordered_map<std::string, std::unique_ptr<Texture>> TextureCreator::CreateTextures(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    assert(device);
    assert(cmdList);

    std::unordered_map<std::string, std::unique_ptr<Texture>> result;

    std::vector<TextureInfo> texInfos;

    // add textures
    texInfos.push_back(TextureInfo{.Name = "woodCrateTex", .FilePath = L"../Source/Textures/WoodCrate01.dds"});


    for (const auto& info : texInfos)
    {
        result[info.Name] = CreateTexture(info, device, cmdList);
    }

    return result;
}

std::unique_ptr<Texture> TextureCreator::CreateTexture(const TextureInfo& info, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    auto result = std::make_unique<Texture>();
    result->Name = info.Name;
    result->Filename = info.FilePath;

    DirectX::CreateDDSTextureFromFile12(device, cmdList, result->Filename.c_str(), result->Resource, result->UploadHeap) >>
        Kds::App::Check;

    return result;
}
