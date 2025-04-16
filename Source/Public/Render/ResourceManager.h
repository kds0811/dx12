#pragma once
#include <memory>
#include <string>

class DescriptorHeapManager;
class GeometryManager;
class TextureManager;
class MaterialManager;
class Material;

class ResourceManager
{
    std::unique_ptr<DescriptorHeapManager> mDescriptorHeapManager;
    std::unique_ptr<GeometryManager> mGeometryManager;
    std::unique_ptr<TextureManager> mTextureManager;
    std::unique_ptr<MaterialManager> mMaterialManager;

public:
    ResourceManager();
    ~ResourceManager();

    const Material* GetMaterial(const std::string& name) const;

private:
    void BuildResources();
    void AddTexturesToStandartMaterial();
};