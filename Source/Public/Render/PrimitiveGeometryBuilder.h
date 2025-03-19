#pragma once
#include "GeometryGenerator.h"
#include "D3D12Utils.h"
#include <memory>
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "GraphicError.h"
#include <vector>
#include "FrameResource.h"
#include "GeometryCommon.h"

struct GeometryData
{
    GeometryGenerator::MeshData meshData;
    std::string name;
    UINT vertexOffset = 0;
    UINT indexOffset = 0;
    SubmeshGeometry submesh;
};

class PrimitiveGeometryBuilder
{
    std::vector<GeometryData> mGeometries;
    GeometryGenerator mGeometryGenerator;
    std::unordered_map<std::string, SubmeshGeometry> mDrawArgs;

    UINT mVertexBufferSize = 0;
    UINT mIndexBufferSize = 0;

public:
    PrimitiveGeometryBuilder() = default;
    std::unique_ptr<MeshGeometry> BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    std::unordered_map<std::string, SubmeshGeometry> GetDrawArgs() { return mDrawArgs; }

private:
    void AddGeometry(const GeometryGenerator::MeshData& mesh, std::string name);

    void CalculateOffsets();
    std::vector<Vertex> CreateVertexBuffer();
    std::vector<std::uint16_t> CreateIndexBuffer();

    std::unique_ptr<MeshGeometry> CreateMeshGeometry(
        ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices, std::string meshName);

    void CalculateMinMaxVertices(DirectX::XMFLOAT3& vertexMinOut, DirectX::XMFLOAT3& vertexMaxOut, const std::vector<Vertex>& vertices);
};

