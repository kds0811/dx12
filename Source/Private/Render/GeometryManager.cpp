#include "GeometryManager.h"
#include "Logger.h"

GeometryManager::GeometryManager() {}

void GeometryManager::CreateBaseGeometries(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    mPrimitiveGeometries = mPrimitiveGeometryBuilder.BuildShapeGeometry(device, cmdList);
}

void GeometryManager::CreatePrimitiveGeoRenderData() 
{
    CreateRenderDataInstance("box");
    CreateRenderDataInstance("sphere");
    CreateRenderDataInstance("cylinder");
    CreateRenderDataInstance("grid50x50");
    CreateRenderDataInstance("grid10x10");
    CreateRenderDataInstance("grid10x10g");

}

void GeometryManager::CreateRenderDataInstance(std::string&& name)
{
    auto drawArgs = mPrimitiveGeometryBuilder.GetDrawArgs();

    if (!drawArgs.contains(name))
    {
        Log::LogWarning("render data " + name + "is not created");
    }

    mRenderDataInstances[name] = std::make_unique<GeoRenderData>(
        name, mPrimitiveGeometries.get(), D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST, drawArgs[name].IndexCount, drawArgs[name].StartIndexLocation, drawArgs[name].BaseVertexLocation);
}
