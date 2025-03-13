#pragma once
#include <cstdint>
#include <string>
#include "GraphicGlobals.h"
#include <DirectXMath.h>
#include "MathHelper.h"

enum class EMaterialType : std::uint8_t
{
    Opaque,
    Transparent,
    AlphaTested,
    Skybox,
    Mirror,
    Reflected,
    Shadow
};

struct Texture;

struct Material
{
    std::string Name;
    EMaterialType Type;


    int MatCBIndex = -1;

    Texture* Tex = nullptr;

    int NumFramesDirty = GG::gNumFrameResources;

    DirectX::XMFLOAT4 DiffuseAlbedo = {1.0f, 1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT3 FresnelR0 = {0.01f, 0.01f, 0.01f};
    float Roughness = .25f;

    DirectX::XMFLOAT4X4 MatTransform = MathHelper::Identity4x4();
};