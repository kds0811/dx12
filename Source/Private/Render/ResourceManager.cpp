#include "ResourceManager.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "Material.h"
#include "GeometryManager.h"
#include "CommandManager.h"
#include "Device.h"
#include "PsoManager.h"
#include "CommandList.h"
#include "DescriptorHeapManager.h"


ResourceManager::ResourceManager()
{
    mDescriptorHeapManager = std::make_unique<DescriptorHeapManager>(Device::GetDevice());
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
        return;
    }

    mTextureManager->CreateBaseTextures(commandList);
    mGeometryManager->CreateBaseGeometries(device, commandList->GetCommandList());
    mGeometryManager->CreatePrimitiveStaticMeshes();

    CommandManager::ReturnAndExecuteCommandList(commandList);
    CommandManager::FlushCommandQueue();
}


void ResourceManager::AddTexturesToStandartMaterial()
{
    // add base color textures ptr to material
    mMaterialManager->SetMaterialBaseColor("wood_crate1",    mTextureManager->GetTexture(L"wood_crate1"));
    mMaterialManager->SetMaterialBaseColor("tile",           mTextureManager->GetTexture(L"tile"));
    mMaterialManager->SetMaterialBaseColor("grass",          mTextureManager->GetTexture(L"grass"));
    mMaterialManager->SetMaterialBaseColor("bricks",         mTextureManager->GetTexture(L"bricks"));
    mMaterialManager->SetMaterialBaseColor("bricks2",        mTextureManager->GetTexture(L"bricks2"));
    mMaterialManager->SetMaterialBaseColor("default",        mTextureManager->GetTexture(L"default"));
    mMaterialManager->SetMaterialBaseColor("stone",          mTextureManager->GetTexture(L"stone"));
    mMaterialManager->SetMaterialBaseColor("grassx",         mTextureManager->GetTexture(L"grassx"));
    mMaterialManager->SetMaterialBaseColor("grassy",         mTextureManager->GetTexture(L"grassy"));
    mMaterialManager->SetMaterialBaseColor("firebal",        mTextureManager->GetTexture(L"firebal"));
    mMaterialManager->SetMaterialBaseColor("coldfire",       mTextureManager->GetTexture(L"coldfire"));
    mMaterialManager->SetMaterialBaseColor("metall",         mTextureManager->GetTexture(L"metall"));
    mMaterialManager->SetMaterialBaseColor("droneBaseColor", mTextureManager->GetTexture(L"droneBaseColor"));
    mMaterialManager->SetMaterialBaseColor("treeArray2",     mTextureManager->GetTexture(L"treeArray2"));
    mMaterialManager->SetMaterialBaseColor("wireFence",      mTextureManager->GetTexture(L"wireFence"));
    mMaterialManager->SetMaterialBaseColor("water",          mTextureManager->GetTexture(L"water"));
    mMaterialManager->SetMaterialBaseColor("grasscube1024",  mTextureManager->GetTexture(L"grasscube1024"));

    // Add normal maps textures ptr
    mMaterialManager->SetMaterialNormalMap("default", mTextureManager->GetTexture(L"default_nmap"));
    mMaterialManager->SetMaterialNormalMap("bricks",  mTextureManager->GetTexture(L"bricks_nmap"));
    mMaterialManager->SetMaterialNormalMap("bricks2", mTextureManager->GetTexture(L"bricks2_nmap"));
    mMaterialManager->SetMaterialNormalMap("tile",    mTextureManager->GetTexture(L"tile_nmap"));
}
