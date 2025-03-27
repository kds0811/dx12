#include "MaterialManager.h"
#include "DirectXColors.h"
#include <vector>
#include "Logger.h"
#include <cassert>

MaterialManager::MaterialManager()
{
    CreateBaseMaterials();
}

void MaterialManager::CreateBaseMaterials()
{
    using namespace DirectX;

    mMaterials.reserve(32);

    CreateMaterial("wood_crate1", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("tile", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.02f, 0.02f, 0.02f), 0.2f);
    CreateMaterial("grass", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("bricks", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
    CreateMaterial("bricks2", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f);
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

void MaterialManager::CreateMaterial(const std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness)
{
    if (mMaterials.contains(name))
    {
        LOG_WARNING("the material named: ", name, " has already been created");
        assert(0);
        return;
    }

    mMaterials[name] = std::make_unique<Material>(name, type, color, fresnel, roughness);
    ++mMaterialCount;
}

void MaterialManager::SetMaterialBaseColor(const std::string& materialName, Texture* baseColor)
{
    if (!baseColor)
    {
        LOG_WARNING("baseColor Texture pointer is nullptr");
        assert(0);
        return;
    }

    if (!mMaterials.contains(materialName))
    {
        LOG_WARNING("the material named: ", materialName, " name has not been created");
        assert(0);
        return;
    }

    mMaterials.at(materialName)->SetBaseColorTexture(baseColor);
    LOG_MESSAGE("the material named: ", materialName, " has the base color texture changed to a texture named:", baseColor->GetName());
}

void MaterialManager::SetMaterialNormalMap(const std::string& materialName,  Texture* normalMap)
{
    if (!normalMap)
    {
        LOG_WARNING(" normalMap Texture pointer is nullptr");
        assert(0);
        return;
    }

    if (!mMaterials.contains(materialName))
    {
        LOG_WARNING("the material named: ", materialName, "  has not been created");
        assert(0);
        return;
    }
    
    mMaterials.at(materialName)->SetNormalMapTexture(normalMap);
    LOG_MESSAGE("the material named: ", materialName, " has the normal map texture changed to a texture named:", normalMap->GetName());
}

const Material* MaterialManager::GetMaterial(const std::string& materialName) const
{
    if (!mMaterials.contains(materialName))
    {
        LOG_WARNING("the material named: ", materialName, " has not been created");
        assert(0);
        return nullptr;
    }
    
    return mMaterials.at(materialName).get();
}
