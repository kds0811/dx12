#pragma once
#include "D3D12Utils.h"
#include "ShapeGeometryBuilder.h"

class ResourceManager
{
    // pointer to Graphic DirectX fields
    ID3D12Device8* mDevice = nullptr;
    ID3D12CommandQueue* mCommandQueue = nullptr;

    // own DirectX fields
    Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mCommandAllocator = nullptr;
    Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList6> mCommandList = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Fence> mFence = nullptr;
    UINT64 mCurrentFenceValue = 0;

    // standart Geonetry Builder
    ShapeGeometryBuilder mShapeGeometryBuilder;

    //Store standart geometry and buffers
    std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> mGeometries;

public:
    ResourceManager(ID3D12Device8* device, ID3D12CommandQueue* commandQueue);


};