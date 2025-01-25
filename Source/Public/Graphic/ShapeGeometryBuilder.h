#pragma once
#include "GeometryGenerator.h"
#include "D3D12Utils.h"
#include <memory>

class ShapeGeometryBuilder
{
public:
    ShapeGeometryBuilder() = default;

    static std::unique_ptr<MeshGeometry> BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

};