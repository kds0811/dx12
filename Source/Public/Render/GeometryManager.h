#pragma once
#include "PrimitiveGeometryBuilder.h"
#include "type_traits"
#include "iterator"

// class for creating and storing all kinds of geometry
class GeometryManager
{
    ID3D12Device* pDevice = nullptr;
    ID3D12GraphicsCommandList* pCommandList = nullptr;

    PrimitiveGeometryBuilder mPrimitiveGeometryBuilder;

    // storage
    std::unique_ptr<MeshGeometry> mPrimitiveGeometries = nullptr;

public:
    GeometryManager(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    [[nodiscard]] inline const MeshGeometry* GetPrimitiveGeometry() const noexcept { return mPrimitiveGeometries.get(); }
};