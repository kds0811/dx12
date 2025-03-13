#include "MaterialBuilder.h"
#include "DirectXColors.h"
#include <vector>

std::unordered_map<std::string, std::unique_ptr<Material>> MaterialBuilder::CreateMaterials(std::unordered_map<std::string, std::unique_ptr<Texture>>& textures)
{
    using namespace DirectX;

    std::unordered_map<std::string, std::unique_ptr<Material>> result{};

    result["wood_crate1"] =
        std::make_unique<Material>("wood_crate1", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["wood_crate1"]);

    result["tile"] = std::make_unique<Material>("tile", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.02f, 0.02f, 0.02f), 0.2f, textures["tile"].get());

    result["grass"] = std::make_unique<Material>("grass", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["grass"].get());

    result["bricks"] = std::make_unique<Material>("bricks", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["bricks"].get());

    result["default"] = std::make_unique<Material>(
        "default", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f, textures["default"].get(), result["default_nmap"].get());

    result["stone"] =
        std::make_unique<Material>("stone", EMaterialType::Transparent, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f, textures["stone"].get());

    result["grassx"] = std::make_unique<Material>("grassx", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["grassx"].get());

    result["grassy"] = std::make_unique<Material>("grassy", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["grassy"].get());

    result["firebal"] =
        std::make_unique<Material>("firebal", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["firebal"].get());

    result["coldfire"] =
        std::make_unique<Material>("coldfire", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["coldfire"].get());

    result["metall"] = std::make_unique<Material>("metall", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.05f, 0.05f, 0.05f), 0.3f, textures["metall"].get());

    result["droneBaseColor"] = std::make_unique<Material>(
        "droneBaseColor", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["droneBaseColor"].get());

    result["treeArray2"] =
        std::make_unique<Material>("treeArray2", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["treeArray2"].get());

    // Alpha Tested
    result["wireFence"] =
        std::make_unique<Material>("wireFence", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.01f, 0.01f, 0.01f), 0.5f, textures["wireFence"].get());

    // Transparent
    result["water"] = std::make_unique<Material>("water", EMaterialType::Opaque, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.6f), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.0f, textures["water"].get());

    // sky box
    result["grasscube1024"] =
        std::make_unique<Material>("grasscube1024", EMaterialType::Skybox, XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f), XMFLOAT3(0.1f, 0.1f, 0.1f), 1.0f, textures["grasscube1024"].get());

    // Mirror
    result["mirror"] = std::make_unique<Material>("mirror", EMaterialType::Mirror, XMFLOAT4(1.0f, 1.0f, 1.0f, 0.3f), XMFLOAT3(0.1f, 0.1f, 0.1f), 0.5f);

    // Planar Shadow Material
    result["shadow"] = std::make_unique<Material>("shadow", EMaterialType::Shadow, XMFLOAT4(0.0f, 0.0f, 0.0f, 0.5f), XMFLOAT3(0.001f, 0.001f, 0.001f), 0.0f);

    return result;
}

// std::unique_ptr<Material> MaterialBuilder::MakeMaterial(const MaterialInfo& matInfo)
//{
//     std::unique_ptr<Material> result = std::make_unique<Material>();
//     result->Type = matInfo.Type;
//     result->DiffuseAlbedo = matInfo.DiffuseAlbedo;
//     result->FresnelR0 = matInfo.Fresnel;
//     result->Roughness = matInfo.Roughness;
//     result->MatCBIndex = mCurrentMatCBIndex;
//     return result;
// }
