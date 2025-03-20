#include "Material.h"
#include "Texture.h"

Material::Material(std::string name, EMaterialType type) 
	:
	mName(name),
	mType(type) 
{}

Material::Material(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness) 
    :
    mName(name),
    mType(type),
    mBaseColorFactor(color),
    mFresnel(fresnel),
    mRoughness(roughness)
{
}

Material::Material(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness, Texture* baseColorTex)
    :
    mName(name),
    mType(type),
    mBaseColorFactor(color),
    mFresnel(fresnel),
    mRoughness(roughness),
    mBaseColorTex(baseColorTex)
{
}

Material::Material(std::string name, EMaterialType type, DirectX::XMFLOAT4 color, DirectX::XMFLOAT3 fresnel, float roughness, Texture* baseColorTex, Texture* normalMapTex)
    :
    mName(name),
    mType(type),
    mBaseColorFactor(color),
    mFresnel(fresnel),
    mRoughness(roughness),
    mBaseColorTex(baseColorTex),
    mNormalMapTex(normalMapTex)
{
}

void Material::SetBaseColorFactor(DirectX::XMFLOAT4 colorFactor)
{
    mBaseColorFactor = colorFactor;
}

void Material::SetNormalMapFactor(DirectX::XMFLOAT3 factor) 
{
    mNormalMapFactor = factor;
}

void Material::SetFresnel(DirectX::XMFLOAT3 fresnel) 
{
    mFresnel = fresnel;
}

void Material::SetRoughness(float roughness)
{
    mRoughness = roughness;
}

void Material::SetMaterialTransformation(DirectX::XMFLOAT4X4 trans)
{
    mMatTransform = trans;
}

void Material::SetBaseColorTexture(Texture* tex)
{
    mBaseColorTex = tex;
}

void Material::SetNormalMapTexture(Texture* tex)
{
    mNormalMapTex = tex;
}
