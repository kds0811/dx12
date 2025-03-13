#pragma once
#include "D3D12Utils.h"
#include <memory>
#include <unordered_map>




struct MaterialInfo
{
    EMaterialType Type = EMaterialType::UNKNOWN;
    DirectX::XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT3 Fresnel = {0.01f, 0.01f, 0.01f};
    float Roughness = .25f;
};



class MaterialBuilder
{
    int mCurrentMatCBIndex = 0;
    int mCurrentDiffuseSrvHeapIndex = 0;
    int mCurrentNormalSrvHeapIndex = 0;

public:
    std::unordered_map<EMaterialType, std::unique_ptr<Material>> CreateMaterials();

private:
    std::unique_ptr<Material> MakeMaterial(const MaterialInfo& matInfo);
};