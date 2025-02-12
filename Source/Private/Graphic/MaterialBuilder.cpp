#include "MaterialBuilder.h"
#include "DirectXColors.h"
#include <vector>

std::unordered_map<EMaterialType, std::unique_ptr<Material>> MaterialBuilder::CreateMaterials()
{
    using namespace DirectX;

    std::vector<MaterialInfo> materialsInfo{};

    MaterialInfo grassInfo{.Type = EMaterialType::GRASS,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.01f, 0.01f, 0.01f),
        .Roughness = 0.5f};
    materialsInfo.push_back(grassInfo);

    MaterialInfo grassxInfo{.Type = EMaterialType::GRASSX,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.01f, 0.01f, 0.01f),
        .Roughness = 0.5f};
    materialsInfo.push_back(grassxInfo);

        MaterialInfo grassyInfo{.Type = EMaterialType::GRASSY,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.01f, 0.01f, 0.01f),
        .Roughness = 0.5f};
    materialsInfo.push_back(grassyInfo);

    MaterialInfo waterInfo{.Type = EMaterialType::WATER,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f),
        .Fresnel = XMFLOAT3(0.1f, 0.1f, 0.1f),
        .Roughness = 0.0f};
    materialsInfo.push_back(waterInfo);

    MaterialInfo bricksInfo{.Type = EMaterialType::BRICKS,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.02f, 0.02f, 0.02f),
        .Roughness = 0.1f};
    materialsInfo.push_back(bricksInfo);

    MaterialInfo stoneInfo{.Type = EMaterialType::STONE,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05f),
        .Roughness = 0.3f};
    materialsInfo.push_back(stoneInfo);

        MaterialInfo tileInfo{.Type = EMaterialType::TILE,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.02f, 0.02f, 0.02f),
        .Roughness = 0.2f};
    materialsInfo.push_back(tileInfo);

         MaterialInfo skullInfo{.Type = EMaterialType::SKULLMAT,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05),
        .Roughness = 0.3f};
    materialsInfo.push_back(skullInfo);


     MaterialInfo fireball{.Type = EMaterialType::FIREBALL,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05),
        .Roughness = 0.3f};
    materialsInfo.push_back(fireball);

    MaterialInfo coldfire{.Type = EMaterialType::COLDFIRE,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05),
        .Roughness = 0.3f};
    materialsInfo.push_back(coldfire);

    MaterialInfo metall{.Type = EMaterialType::METALL,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05),
        .Roughness = 0.3f};
    materialsInfo.push_back(metall);

    MaterialInfo droneBaseColor{.Type = EMaterialType::DRONEBASECOLOR,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05),
        .Roughness = 0.3f};
    materialsInfo.push_back(droneBaseColor);

     MaterialInfo wireFence{.Type = EMaterialType::WIREFENCE,
        .DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
        .Fresnel = XMFLOAT3(0.05f, 0.05f, 0.05),
        .Roughness = 0.3f};
    materialsInfo.push_back(wireFence);


    std::unordered_map<EMaterialType, std::unique_ptr<Material>> result{};

    for (const auto& matInfo : materialsInfo)
    {

        if (result.contains(matInfo.Type))
        {
            assert(false); // material type allready on container
            continue;
        }
        result[matInfo.Type] = MakeMaterial(matInfo);

        ++mCurrentMatCBIndex;
    }

    return result;
}

std::unique_ptr<Material> MaterialBuilder::MakeMaterial(const MaterialInfo& matInfo)
{
    std::unique_ptr<Material> result = std::make_unique<Material>();
    result->Type = matInfo.Type;
    result->DiffuseAlbedo = matInfo.DiffuseAlbedo;
    result->FresnelR0 = matInfo.Fresnel;
    result->Roughness = matInfo.Roughness;
    result->MatCBIndex = mCurrentMatCBIndex;
    return result;
}
