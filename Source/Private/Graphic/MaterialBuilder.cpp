#include "MaterialBuilder.h"
#include "DirectXColors.h"

std::unordered_map<EMaterialType, std::unique_ptr<Material>> MaterialBuilder::CreateMaterials()
{
    using namespace DirectX;

    std::unordered_map<EMaterialType, std::unique_ptr<Material>> result{};

    auto grass = std::make_unique<Material>();
    grass->Type = EMaterialType::GRASS;
    grass->MatCBIndex = 0;
    grass->DiffuseAlbedo = XMFLOAT4(0.3f, 0.6f, 0.3f, 1.0f);
    grass->FresnelR0 = XMFLOAT3(0.01f, 0.01f, 0.01f);
    grass->Roughness = 0.125f;

    auto water = std::make_unique<Material>();
    water->Type = EMaterialType::WATER;
    water->MatCBIndex = 1;
    water->DiffuseAlbedo = XMFLOAT4(0.0f, 0.2f, 0.6f, 1.0f);
    water->FresnelR0 = XMFLOAT3(0.1f, 0.1f, 0.1f);
    water->Roughness = 0.0f;

    auto bricks0 = std::make_unique<Material>();
    bricks0->Type = EMaterialType::BRICKS;
    bricks0->MatCBIndex = 2;
    bricks0->DiffuseSrvHeapIndex = 0;
    bricks0->DiffuseAlbedo = XMFLOAT4(Colors::ForestGreen);
    bricks0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
    bricks0->Roughness = 0.1f;

    auto stone0 = std::make_unique<Material>();
    stone0->Type = EMaterialType::STONE;
    stone0->MatCBIndex = 3;
    stone0->DiffuseSrvHeapIndex = 1;
    stone0->DiffuseAlbedo = XMFLOAT4(Colors::LightSteelBlue);
    stone0->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05f);
    stone0->Roughness = 0.3f;

    auto tile0 = std::make_unique<Material>();
    tile0->Type = EMaterialType::TILE;
    tile0->MatCBIndex = 4;
    tile0->DiffuseSrvHeapIndex = 2;
    tile0->DiffuseAlbedo = XMFLOAT4(Colors::LightGray);
    tile0->FresnelR0 = XMFLOAT3(0.02f, 0.02f, 0.02f);
    tile0->Roughness = 0.2f;

    auto skullMat = std::make_unique<Material>();
    skullMat->Type = EMaterialType::SKULLMAT;
    skullMat->MatCBIndex = 5;
    skullMat->DiffuseSrvHeapIndex = 3;
    skullMat->DiffuseAlbedo = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    skullMat->FresnelR0 = XMFLOAT3(0.05f, 0.05f, 0.05);
    skullMat->Roughness = 0.3f;


    result[EMaterialType::GRASS] = std::move(grass);
    result[EMaterialType::WATER] = std::move(water);
    result[EMaterialType::BRICKS] = std::move(bricks0);
    result[EMaterialType::STONE] = std::move(stone0);
    result[EMaterialType::TILE] = std::move(tile0);
    result[EMaterialType::SKULLMAT] = std::move(skullMat);
    

    return result;
}