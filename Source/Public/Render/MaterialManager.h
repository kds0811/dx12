#pragma once
#include "D3D12Utils.h"
#include <memory>
#include <unordered_map>
#include <string>
#include "Material.h"
#include "Texture.h"

class MaterialManager
{
    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;

public:
    MaterialManager();

    void CreateMaterial(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness);

    void SetMaterialBaseColor(std::string materialName, Texture* baseColor);
    void SetMaterialNormalMap(std::string materialName, Texture* normalMap);

private:
    void CreateBaseMaterials();
};