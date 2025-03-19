#include "MaterialManager.h"
#include "DirectXColors.h"
#include <vector>
#include "Logger.h"

MaterialManager::MaterialManager()
{
    CreateBaseMaterials();
}

void MaterialManager::CreateBaseMaterials()
{
    using namespace DirectX;

    CreateMaterial("wood_crate1", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("tile", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.02f, 0.02f, 0.02f), 0.2f);
    CreateMaterial("grass", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("bricks", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("default", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f);
    CreateMaterial("stone", EMaterialType::Transparent, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f);
    CreateMaterial("grassx", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("grassy", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("firebal", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("coldfire", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("metall", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f);
    CreateMaterial("droneBaseColor", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("treeArray2", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("wireFence", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("water", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.0f);
    CreateMaterial("grasscube1024", EMaterialType::Skybox, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.1f, 0.1f, 0.1f), 1.0f);
    CreateMaterial("mirror", EMaterialType::Mirror, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.5f);
    CreateMaterial("shadow", EMaterialType::Shadow, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f), XMFLOAT3(0.001f, 0.001f, 0.001f), 0.0f);

}

void MaterialManager::CreateMaterial(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness) 
{
    if (mMaterials.contains(name))
    {
        std::string message = "material " + name + " has already been created";
        Log::LogWarning(message);
        return;
    }

    mMaterials[name] = std::make_unique<Material>(name, type, color, fresnel, roughness);
}

void MaterialManager::SetMaterialBaseColor(std::string materialName, Texture* baseColor)
{
    if (!baseColor)
    {
        std::string message = " baseColor Texture pointer is nullptr";
        Log::LogWarning(message);
    }

    if (!mMaterials.contains(materialName))
    {
        std::string message = "material with" + materialName  + " name has not been created";
        Log::LogWarning(message);
    }

    mMaterials[materialName]->SetBaseColorTexture(baseColor);
}

void MaterialManager::SetMaterialNormalMap(std::string materialName, Texture* normalMap)
{
    if (!normalMap)
    {
        std::string message = "normalMap Texture pointer is nullptr";
        Log::LogWarning(message);
    }

    if (!mMaterials.contains(materialName))
    {
        std::string message = "material with" + materialName + " name has not been created";
        Log::LogWarning(message);
    }

    mMaterials[materialName]->SetNormalMapTexture(normalMap);
}
