#pragma once
#include "D3D12Utils.h"

struct TextureInfo
{
    EMaterialType Type;
    std::wstring FilePath;
};

class TextureCreator
{

public:
    std::unordered_map<EMaterialType, std::unique_ptr<Texture>> CreateTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
    std::unique_ptr<Texture> CreateTexture(const TextureInfo& info, ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
};