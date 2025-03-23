#include "GeometryManager.h"
#include "Logger.h"
#include <cassert>

GeometryManager::GeometryManager() {}

void GeometryManager::CreateBaseGeometries(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    mPrimitiveGeometries = mPrimitiveGeometryBuilder.BuildShapeGeometry(device, cmdList);
    
}

void GeometryManager::CreatePrimitiveGeoRenderData() 
{
    mRenderDataInstances.reserve(16);
    CreateRenderDataInstance("box");
    CreateRenderDataInstance("sphere");
    CreateRenderDataInstance("cylinder");
    CreateRenderDataInstance("grid50x50");
    CreateRenderDataInstance("grid10x10");
    CreateRenderDataInstance("grid10x10g");
}

const GeoRenderData* GeometryManager::GetRenderDataInstancePtr(const std::string& name) const
{
    if (!mRenderDataInstances.contains(name))
    {
        LOG_WARNING("render data instance ", name, " is not created");
        assert(0);
        return;
    }

    return mRenderDataInstances.at(name).get();
}

void GeometryManager::CreateRenderDataInstance(const std::string& name)
{
    const auto& drawArgs = mPrimitiveGeometryBuilder.GetDrawArgs();

    if (!drawArgs.contains(name))
    {
        LOG_WARNING("render data ", name, "is not created");
        assert(0);
        return;
    }

   const auto& namedDrawArgs = drawArgs.at(name);

    mRenderDataInstances[name] = std::make_unique<GeoRenderData>(name, mPrimitiveGeometries.get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST,
        namedDrawArgs.IndexCount, namedDrawArgs.StartIndexLocation, namedDrawArgs.BaseVertexLocation);
}
