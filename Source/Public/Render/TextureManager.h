#pragma once
#include "D3D12Utils.h"
#include "Texture.h"


class TextureManager
{
    ID3D12Device* pDevice = nullptr;
    ID3D12GraphicsCommandList* pCommandList = nullptr;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

public:
    TextureManager(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    void CreateBaseTextures();



};