#pragma once
#include "D3D12Utils.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "Material.h"
#include "GeometryManager.h"
#include <memory>



class ResourceManager
{

    // Managers
    std::unique_ptr<GeometryManager> mGeometryManager;
    std::unique_ptr<TextureManager> mTextureManager;
    std::unique_ptr<MaterialManager> mMaterialManager;


public:
    ResourceManager();

    const Material* GetMaterial(const std::string& name) const;

private:
    void BuildResources();
    void AddTexturesToStandartMaterial();
};