#pragma once
#include "GeometryGenerator.h"
#include "D3D12Utils.h"
#include <memory>
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "GraphicError.h"
#include <vector>
#include "GeometryLoader.h"

class ShapeGeometryBuilder
{
    struct GeometryData
    {
        GeometryGenerator::MeshData mesh;
        DirectX::XMFLOAT4 color;
        ePrimitiveType type;
        UINT vertexOffset;
        UINT indexOffset;
        SubmeshGeometry submesh;
    };

    std::vector<GeometryData> mGeometries;
    GeometryGenerator mGeometryGenerator;
    GeometryLoader mGeometryLoader;

    UINT mVertexBufferSize = 0;
    UINT mIndexBufferSize = 0;

public:
    ShapeGeometryBuilder() = default;
    std::unique_ptr<MeshGeometry> BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    std::unique_ptr<MeshGeometry> BuildWavesGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
    void AddGeometry(const GeometryGenerator::MeshData& mesh, const DirectX::XMFLOAT4& color, ePrimitiveType type);
    void CalculateOffsets();
    std::vector<Vertex> CreateVertexBuffer();
    std::vector<std::uint16_t> CreateIndexBuffer();
    std::unique_ptr<MeshGeometry> CreateMeshGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
        const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices);
    void ModifyHeightLandVertices(std::vector<Vertex>& vertices);
    float GetHillsHeight(float x, float z) const;


};