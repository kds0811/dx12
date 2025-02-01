#pragma once
#include "D3D12Utils.h"
#include "ShapeGeometryBuilder.h"
#include "MaterialBuilder.h"

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

    // Builders
    ShapeGeometryBuilder mShapeGeometryBuilder;
    MaterialBuilder mMaterialBuilder;

    // Storage standart geometry and buffers
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;

    // Storage materials
    std::unordered_map<EMaterialType, std::unique_ptr<Material>> mMaterials;

public:
    ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue);
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>>& GetGeometries() { return mGeometries; }
    std::unordered_map<EMaterialType, std::unique_ptr<Material>>& GetMaterials() { return mMaterials; }

private:
    void CreateStandartShapeGeometry();
    void FlushCommandQueue();
};