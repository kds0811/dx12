#pragma once
#include <cstdint>
#include <string>
#include <DirectXMath.h>
#include "MathHelper.h"
#include <memory>
#include "Settings.h"

class Texture;

enum class EMaterialType : std::uint8_t
{
    Default,
    Opaque,
    Transparent,
    AlphaTested,
    Skybox,
    Mirror,
    Reflected,
    Shadow,
};

class Material
{
    std::string mName{};
    EMaterialType mType = EMaterialType::Default;

    Texture* mBaseColor = nullptr;
    Texture* mNormalMap = nullptr;
    Texture* mRoughnesslMap = nullptr;
    Texture* mMetallicMap = nullptr;

    int mMatCBIndex = -1;
    int mNumFramesDirty = Settings::mNumFrameResources;
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
    void SetNormalMapTexture(Texture* tex);

    inline EMaterialType GetMaterialType() noexcept { return mType; }
    inline Texture* GetBaseColor() noexcept { return mBaseColor; }
    inline Texture* GetNormalMap() noexcept { return mNormalMap; }
    inline Texture* GetRoughnesslMap() noexcept { return mRoughnesslMap; }
    inline Texture* GetMetalliclMap() noexcept { return mMetallicMap; }
    inline int GetMaterialCBIndex() noexcept { return mMatCBIndex; }
};