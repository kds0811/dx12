#pragma once
#include "D3D12Utils.h"
#include "Texture.h"


class TextureCreator
{

public:
    std::unordered_map<std::string, std::unique_ptr<Texture>> CreateTextures(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

};