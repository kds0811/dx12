#pragma once
#include "D3D12Utils.h"
#include "MaterialManager.h"
#include "TextureManager.h"
#include "Material.h"
#include "GeometryManager.h"
#include <memory>


class ResourceManager
{
    // pointer to Graphic DirectX fields
    ID3D12Device8* pDevice = nullptr;
    ID3D12CommandQueue* pCommandQueue = nullptr;

    // own DirectX fields
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator = nullptr;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> mCommandList = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence = nullptr;
    UINT64 mCurrentFenceValue = 0;

    // Managers
    std::unique_ptr<GeometryManager> mGeometryManager;
    std::unique_ptr<TextureManager> mTextureManager;
    std::unique_ptr<MaterialManager> mMaterialManager;


public:
    ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue);

    const GeoRenderData* GetRenderDataInstance(const std::string& name) const;
    const Material* GetMaterial(const std::string& name) const;



private:
    void BuildResources();
    void CreateCommandList();
    void FlushCommandQueue();
    void AddTexturesToStandartMaterial();
};