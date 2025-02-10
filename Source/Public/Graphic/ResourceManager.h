#pragma once
#include "D3D12Utils.h"
#include "ShapeGeometryBuilder.h"
#include "MaterialBuilder.h"
#include "TextureCreator.h"


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
    ShapeGeometryBuilder mShapeGeometryBuilder;
    MaterialBuilder mMaterialBuilder;
    TextureCreator mTextureCreator;
    

    // Storages
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;
    std::unordered_map<EMaterialType, std::unique_ptr<Material>> mMaterials;
    std::unordered_map<EMaterialType, std::unique_ptr<Texture>> mTextures;

public:
    ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue);
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GetGeometries() { return mGeometries; }
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& GetMaterials() { return mMaterials; }
    std::unordered_map<EMaterialType, std::unique_ptr<Texture>>& GetTextures() { return mTextures; }

private:
    void CreateStandartShapeGeometry();
    void FlushCommandQueue();
    void AddTexturePtrOnMaterial();
};