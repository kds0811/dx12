#include "Material.h"
#include "Texture.h"

Material::Material(std::string name, EMaterialType type) 
	:
	mName(name),
	mType(type) 
{}

void Material::SetBaseColor(DirectX::XMFLOAT4 color)
{
    mBaseColor = color;
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
