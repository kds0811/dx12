#pragma once
#include "PrimitiveGeometryBuilder.h"
#include "type_traits"
#include "iterator"
#include <unordered_map>
#include "RenderData.h"
#include "StaticMesh.h"


class GeometryManager
{
    PrimitiveGeometryBuilder mPrimitiveGeometryBuilder;

    // storage Geometries
    std::unique_ptr<MeshGeometry> mPrimitiveGeometries = nullptr;

    //  Primitive geometry static meshes
    std::unordered_map<std::string, std::unique_ptr<StaticMesh>> mStaticMeshes;
    int mStaticMeshID = 0;

public:
    GeometryManager();
    void CreateBaseGeometries(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    void CreatePrimitiveStaticMeshes();

    const StaticMesh* GetStaticMesh(const std::string& name) const;
    StaticMesh* GetStaticMesh(const std::string& name);

private:
    void CreateStaticMesh(const std::string& name);
};