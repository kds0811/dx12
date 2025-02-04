#include "TextureCreator.h"
#include "GraphicError.h"
#include "DDSTextureLoader.h"

std::unordered_map<std::string, std::unique_ptr<Texture>> TextureCreator::CreateTextures(
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    std::unordered_map<std::string, std::unique_ptr<Texture>> result{};

    auto woodCrateTex = std::make_unique<Texture>();
    woodCrateTex->Name = "woodCrateTex";
    woodCrateTex->Filename = L"../Source/Textures/WoodCrate01.dds";
    DirectX::CreateDDSTextureFromFile12(
        device, cmdList, woodCrateTex->Filename.c_str(), woodCrateTex->Resource, woodCrateTex->UploadHeap) >> Kds::App::Check;

    result[woodCrateTex->Name] = std::move(woodCrateTex);

    return result;
}
