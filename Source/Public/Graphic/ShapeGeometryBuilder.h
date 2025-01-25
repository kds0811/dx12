#pragma once
#include "GeometryGenerator.h"
#include "D3D12Utils.h"
#include <memory>
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "GraphicError.h"
#include <vector>

class ShapeGeometryBuilder
{
    struct GeometryData
    {
        GeometryGenerator::MeshData mesh;
        DirectX::XMFLOAT4 color;
        std::string name;
        UINT vertexOffset;
        UINT indexOffset;
        SubmeshGeometry submesh;
    };

    std::vector<GeometryData> mGeometries;
    GeometryGenerator mGeometryGenerator;
    UINT mVertexBufferSize = 0;
    UINT mIndexBufferSize = 0;

public:
    ShapeGeometryBuilder() = default;
    std::unique_ptr<MeshGeometry> BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
    void AddGeometry(const GeometryGenerator::MeshData& mesh, const DirectX::XMFLOAT4& color, const std::string& name);
    void CalculateOffsets();
    std::vector<Vertex> CreateVertexBuffer();
    std::vector<std::uint16_t> CreateIndexBuffer();
    std::unique_ptr<MeshGeometry> CreateMeshGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices);
};