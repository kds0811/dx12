#pragma once
#include "PrimitiveGeometryBuilder.h"
#include "type_traits"
#include "iterator"

// class for creating and storing all kinds of geometry
class GeometryManager
{
    using iterator = std::vector<std::unique_ptr<MeshGeometry>>::iterator;
    using const_iterator = std::vector<std::unique_ptr<MeshGeometry>>::const_iterator;

    ID3D12Device* pDevice = nullptr;
    ID3D12GraphicsCommandList* pCommandList = nullptr;

    PrimitiveGeometryBuilder mPrimitiveGeometryBuilder;

    // storage
    std::unique_ptr<MeshGeometry> mPrimitiveGeometries = nullptr;

public:
    GeometryManager(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    [[nodiscard]] inline const MeshGeometry* GetPrimitiveGeometry() const noexcept { return mPrimitiveGeometries.get(); }
};