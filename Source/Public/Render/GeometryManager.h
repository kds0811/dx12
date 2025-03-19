#pragma once
#include "PrimitiveGeometryBuilder.h"
#include "type_traits"
#include "iterator"
#include <unordered_map>
#include "GeoRenderData.h"
#include "GeometryCommon.h"


class GeometryManager
{
    PrimitiveGeometryBuilder mPrimitiveGeometryBuilder;

    std::unordered_map<std::string, std::unique_ptr<GeoRenderData>> mRenderDataInstances;


    // storage Geometries
    std::unique_ptr<MeshGeometry> mPrimitiveGeometries = nullptr;
    std::unordered_map<std::string, SubmeshGeometry> mDrawArgs;

public:
    GeometryManager();
    void CreateBaseGeometries(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

    [[nodiscard]] inline const MeshGeometry* GetPrimitiveGeometry() const noexcept { return mPrimitiveGeometries.get(); }

private:
    void CreatePrimitiveGeoRenderData();


};