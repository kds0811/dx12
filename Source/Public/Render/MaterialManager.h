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
    static inline size_t mMaterialCount = 0;


public:
    MaterialManager();

    void CreateMaterial(const std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness);

    void SetMaterialBaseColor(const std::string& materialName, Texture* baseColor);
    void SetMaterialNormalMap(const std::string& materialName, Texture* normalMap);

    const Material* GetMaterial(const std::string& materialName) const;

    static inline size_t GetMaterialCount() noexcept { return mMaterialCount; }

private:
    void CreateBaseMaterials();
};