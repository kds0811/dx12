#pragma once
#include "PrimitiveGeometryBuilder.h"
#include "type_traits"
#include "iterator"
#include <unordered_map>
#include "GeometryCommon.h"
#include "RenderData.h"


class GeometryManager
{
    PrimitiveGeometryBuilder mPrimitiveGeometryBuilder;

    std::unordered_map<std::string, std::unique_ptr<GeoRenderData>> mRenderDataInstances;

    // storage Geometries
    std::unique_ptr<MeshGeometry> mPrimitiveGeometries = nullptr;

public:
    GeometryManager();
    void CreateBaseGeometries(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void CreatePrimitiveGeoRenderData();
    const GeoRenderData* GetRenderDataInstancePtr(const std::string& name) const;

private:
    void CreateRenderDataInstance(const std::string& name);
};