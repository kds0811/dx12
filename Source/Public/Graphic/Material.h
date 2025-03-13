#pragma once
#include <cstdint>
#include <string>
#include "GraphicGlobals.h"
#include <DirectXMath.h>
#include "MathHelper.h"
#include <memory>

class Texture;

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

class Material
{
    std::string mName{};
    EMaterialType mType = EMaterialType::Opaque;

    Texture* mBaseColorTex = nullptr;
    Texture* mNormalMapTex = nullptr;

    int mMatCBIndex = -1;
    int mNumFramesDirty = GG::gNumFrameResources;
    DirectX::XMFLOAT4X4 mMatTransform = MathHelper::Identity4x4();

    DirectX::XMFLOAT4 mBaseColorFactor = {1.0f, 1.0f, 1.0f, 1.0f};
    DirectX::XMFLOAT3 mFresnel = {0.01f, 0.01f, 0.01f};
    DirectX::XMFLOAT3 mNormalMapFactor = {1.0f, 1.0f, 1.0f};
    float mRoughness = .25f;
   

public:
    Material(std::string name, EMaterialType type);
    Material(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness);
    Material(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness, Texture* baseColorTex);
    Material(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness, Texture* baseColorTex, Texture* normalMapTex);

    void SetBaseColorFactor(DirectX::XMFLOAT4 colorFactor);
    void SetNormalMapFactor(DirectX::XMFLOAT3 factor);
    void SetFresnel(DirectX::XMFLOAT3 fresnel);
    void SetRoughness(float roughness);
    void SetMaterialTransformation(DirectX::XMFLOAT4X4 trans);
    void SetBaseColorTexture(Texture* tex);
};