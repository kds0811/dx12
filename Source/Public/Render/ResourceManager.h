#pragma once
#include "D3D12Utils.h"
#include "MaterialBuilder.h"
#include "TextureManager.h"
#include "Material.h"
#include "GeometryManager.h"
#include <memory>


class ResourceManager
{
    // pointer to Graphic DirectX fields
    ID3D12Device8* pDevice = nullptr;
    ID3D12CommandQueue* pCommandQueue = nullptr;

    // own DirectX members
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator = nullptr;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> mCommandList = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence = nullptr;
    UINT64 mCurrentFenceValue = 0;

    // Builders
    std::unique_ptr<GeometryManager> mGeometryManager;
    std::unique_ptr<TextureManager> mTexturemanager;
    MaterialBuilder mMaterialBuilder;

    // Storages

    std::unordered_map<std::string, std::unique_ptr<Material>> mMaterials;
    std::unordered_map<std::string, std::unique_ptr<Texture>> mTextures;

public:
    ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue);

    std::unordered_map<std::string, std::unique_ptr<Material>>& GetMaterials() { return mMaterials; }
    std::unordered_map<std::string, std::unique_ptr<Texture>>& GetTextures() { return mTextures; }

private:
    void CreateStandartShapeGeometry();
    void BuildResources();
    void CreateCommandList();
    void FlushCommandQueue();
};