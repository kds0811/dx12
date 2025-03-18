#include "GeometryManager.h"

GeometryManager::GeometryManager(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
	:
	pDevice(device),
	pCommandList(cmdList)
{
    mPrimitiveGeometries = mPrimitiveGeometryBuilder.BuildShapeGeometry(pDevice, pCommandList);
}
