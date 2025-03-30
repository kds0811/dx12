#include "ResourceManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "Material.h"
#include "GeometryManager.h"
#include "CommandManager.h"
#include "Device.h"
#include "PsoManager.h"
#include "CommandList.h"

ResourceManager::ResourceManager()
{
    mTextureManager = std::make_unique<TextureManager>();
    mGeometryManager = std::make_unique<GeometryManager>();
    mMaterialManager = std::make_unique<MaterialManager>();

    BuildResources();

    AddTexturesToStandartMaterial();
}

ResourceManager::~ResourceManager() = default;

const Material* ResourceManager::GetMaterial(const std::string& name) const
{
    return mMaterialManager->GetMaterial(name);
}

void ResourceManager::BuildResources()
{
    auto commandList = CommandManager::GetFreeCommandListAndResetIt(PsoManager::GetStandartPso()); 
    auto device = Device::GetDevice();

    assert(commandList && device);
    if (!commandList || !device)
    {
        LOG_ERROR("command list or device pointers is nullptr");
    }

    mTextureManager->CreateBaseTextures(device, commandList->GetCommandList());
    mGeometryManager->CreateBaseGeometries(device, commandList->GetCommandList());
    mGeometryManager->CreatePrimitiveStaticMeshes();

    CommandManager::ReturnAndExecuteCommandList(commandList);
    CommandManager::FlushCommandQueue();
}


void ResourceManager::AddTexturesToStandartMaterial()
{
    // add base color textures ptr to material
    mMaterialManager->SetMaterialBaseColor("wood_crate1", mTextureManager->GetTexture("wood_crate1"));
    mMaterialManager->SetMaterialBaseColor("tile", mTextureManager->GetTexture("tile"));
    mMaterialManager->SetMaterialBaseColor("grass", mTextureManager->GetTexture("grass"));
    mMaterialManager->SetMaterialBaseColor("bricks", mTextureManager->GetTexture("bricks"));
    mMaterialManager->SetMaterialBaseColor("bricks2", mTextureManager->GetTexture("bricks2"));
    mMaterialManager->SetMaterialBaseColor("default", mTextureManager->GetTexture("default"));
    mMaterialManager->SetMaterialBaseColor("stone", mTextureManager->GetTexture("stone"));
    mMaterialManager->SetMaterialBaseColor("grassx", mTextureManager->GetTexture("grassx"));
    mMaterialManager->SetMaterialBaseColor("grassy", mTextureManager->GetTexture("grassy"));
    mMaterialManager->SetMaterialBaseColor("firebal", mTextureManager->GetTexture("firebal"));
    mMaterialManager->SetMaterialBaseColor("coldfire", mTextureManager->GetTexture("coldfire"));
    mMaterialManager->SetMaterialBaseColor("metall", mTextureManager->GetTexture("metall"));
    mMaterialManager->SetMaterialBaseColor("droneBaseColor", mTextureManager->GetTexture("droneBaseColor"));
    mMaterialManager->SetMaterialBaseColor("treeArray2", mTextureManager->GetTexture("treeArray2"));
    mMaterialManager->SetMaterialBaseColor("wireFence", mTextureManager->GetTexture("wireFence"));
    mMaterialManager->SetMaterialBaseColor("water", mTextureManager->GetTexture("water"));
    mMaterialManager->SetMaterialBaseColor("grasscube1024", mTextureManager->GetTexture("grasscube1024"));

    // Add normal maps textures ptr
    mMaterialManager->SetMaterialNormalMap("default", mTextureManager->GetTexture("default_nmap"));
    mMaterialManager->SetMaterialNormalMap("bricks", mTextureManager->GetTexture("bricks_nmap"));
    mMaterialManager->SetMaterialNormalMap("bricks2", mTextureManager->GetTexture("bricks2_nmap"));
    mMaterialManager->SetMaterialNormalMap("tile", mTextureManager->GetTexture("tile_nmap"));
}
