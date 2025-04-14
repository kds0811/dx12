#include "GeometryManager.h"
#include "Logger.h"
#include <cassert>

GeometryManager::GeometryManager() {}

void GeometryManager::CreateBaseGeometries(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    mPrimitiveGeometries = mPrimitiveGeometryBuilder.BuildShapeGeometry(device, cmdList);
}

void GeometryManager::CreatePrimitiveStaticMeshes()
{
    CreateStaticMesh("box");
    CreateStaticMesh("sphere");
    CreateStaticMesh("cylinder");
    CreateStaticMesh("grid50x50");
    CreateStaticMesh("grid10x10");
    CreateStaticMesh("grid10x10g");
}

const StaticMesh* GeometryManager::GetStaticMesh(const std::string& name) const
{
    if (!mStaticMeshes.contains(name))
    {
        LOG_WARNING("render data instance ", name, " is not created");
        assert(0);
        return;
    }

    return mStaticMeshes.at(name).get();
}

StaticMesh* GeometryManager::GetStaticMesh(const std::string& name)
{
    if (!mStaticMeshes.contains(name))
    {
        LOG_WARNING("render data instance ", name, " is not created");
        assert(0);
        return;
    }

    return mStaticMeshes.at(name).get();
}

void GeometryManager::CreateStaticMesh(const std::string& name)
{
    const auto& drawArgs = mPrimitiveGeometries->GetDrawArgs();

    if (!drawArgs.contains(name))
    {
        LOG_WARNING("render data ", name, "is not created");
        assert(0);
        return;
    }

    const auto& namedDrawArgs = drawArgs.at(name);

    GeoRenderData geoRenderData = {
        name, mPrimitiveGeometries.get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, namedDrawArgs.IndexCount, namedDrawArgs.StartIndexLocation, namedDrawArgs.BaseVertexLocation};
    
    mStaticMeshes[name] = std::make_unique<StaticMesh>(name, mStaticMeshID, geoRenderData);
    LOG_MESSAGE(" Static Mesh ", name, "ID: ", mStaticMeshID, " Is created");
    mStaticMeshID++;
}
