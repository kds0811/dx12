#include "GeometryManager.h"

GeometryManager::GeometryManager()
{}

void GeometryManager::CreateBaseGeometries(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    mPrimitiveGeometries = mPrimitiveGeometryBuilder.BuildShapeGeometry(device, cmdList);
}
