#include "PrimitiveGeometryBuilder.h"
#include <algorithm>
#include <string>
#include "Transform.h"
#include <random>
#include "Vector.h"

using namespace DirectX;

PrimitiveGeometryBuilder::PrimitiveGeometryBuilder() {}

PrimitiveGeometryBuilder::~PrimitiveGeometryBuilder() = default;

std::unique_ptr<MeshGeometry> PrimitiveGeometryBuilder::BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // Add geometries
    AddGeometry(mGeometryGenerator.CreateBox(1.f, 1.0f, 1.0f, 3), "box");
    AddGeometry(mGeometryGenerator.CreateSphere(1.f, 20, 20), "sphere");
    AddGeometry(mGeometryGenerator.CreateCylinder(1.0f, 1.f, 1.0f, 20, 20), "cylinder");
    AddGeometry(mGeometryGenerator.CreateGrid(50.0f, 50.0f, 50, 50), "grid50x50");
    AddGeometry(mGeometryGenerator.CreateGrid(10.0f, 10.0f, 5, 5), "grid10x10");
    AddGeometry(mGeometryGenerator.CreateGrid(10.0f, 10.0f, 2, 2), "grid10x10g");

    CalculateOffsets();

    auto vertices = CreateVertexBuffer();
    auto indices = CreateIndexBuffer();

    auto result = CreateMeshGeometry(L"primitiveShapeGeo", cmdList, vertices, indices);

    // Clear geometries for next use
    mGeometries.clear();

    return std::move(result);
}

void PrimitiveGeometryBuilder::AddGeometry(const GeometryGenerator::MeshData& mesh, std::string name)
{
    GeometryData data;
    data.meshData = mesh;
    data.name = name;
    mGeometries.push_back(data);
}

void PrimitiveGeometryBuilder::CalculateOffsets()
{
    UINT currentVertexOffset = 0;
    UINT currentIndexOffset = 0;

    for (auto& geometry : mGeometries)
    {
        geometry.vertexOffset = currentVertexOffset;
        geometry.indexOffset = currentIndexOffset;

        const auto& meshData = geometry.meshData;

        geometry.submesh.IndexCount = (UINT)meshData.Indices32.size();
        geometry.submesh.StartIndexLocation = geometry.indexOffset;
        geometry.submesh.BaseVertexLocation = geometry.vertexOffset;
        currentVertexOffset += (UINT)meshData.Vertices.size();
        currentIndexOffset += (UINT)meshData.Indices32.size();
    }
    mVertexBufferSize = currentVertexOffset;
    mIndexBufferSize = currentIndexOffset;
}

std::vector<Vertex> PrimitiveGeometryBuilder::CreateVertexBuffer()
{
    std::vector<Vertex> vertices;
    vertices.reserve(mVertexBufferSize);

    for (auto& geometry : mGeometries)
    {
        // Calculate minx max vertices for create bound volume
        DirectX::XMVECTOR vMax = geometry.submesh.VertexMax.ToSIMD();
        DirectX::XMVECTOR vMin = geometry.submesh.VertexMin.ToSIMD();

        auto& meshData = geometry.meshData;
        for (auto& vertex : meshData.Vertices)
        {
            Vertex v;
            v.Pos = vertex.Position;
            v.Normal = vertex.Normal;
            v.TexC = vertex.TexC;
            vertices.push_back(v);

            auto vPosVec = DirectX::XMLoadFloat3(&v.Pos);
            vMax = DirectX::XMVectorMax(vMax, vPosVec);
            vMin = DirectX::XMVectorMin(vMin, vPosVec);
        }

        geometry.submesh.VertexMax = Vector{vMax};
        geometry.submesh.VertexMin = Vector{vMin};
    }

    return vertices;
}

std::vector<std::uint16_t> PrimitiveGeometryBuilder::CreateIndexBuffer()
{
    std::vector<std::uint16_t> indices;
    indices.reserve(mIndexBufferSize);
    for (auto& geometry : mGeometries)
    {
        const auto& meshData = geometry.meshData;
        auto geometryIndices = meshData.Indices32;
        indices.insert(indices.end(), std::begin(geometryIndices), std::end(geometryIndices));
    }
    return indices;
}

std::unique_ptr<MeshGeometry> PrimitiveGeometryBuilder::CreateMeshGeometry(
    std::wstring meshName, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices)
{
    auto geo = std::make_unique<MeshGeometry>(meshName, cmdList, vertices, indices);
    
    for (const auto& geometry : mGeometries)
    {
        geo->AddSubmeshDrawArgs(geometry.name, geometry.submesh);
    }

    return geo;
}

void PrimitiveGeometryBuilder::CalculateMinMaxVertices(DirectX::XMFLOAT3& vertexMinOut, DirectX::XMFLOAT3& vertexMaxOut, const std::vector<Vertex>& vertices)
{
    DirectX::XMVECTOR vMax = DirectX::XMLoadFloat3(&vertexMaxOut);
    DirectX::XMVECTOR vMin = DirectX::XMLoadFloat3(&vertexMinOut);

    for (const auto& v : vertices)
    {
        auto vPosVec = DirectX::XMLoadFloat3(&v.Pos);
        vMax = DirectX::XMVectorMax(vMax, vPosVec);
        vMin = DirectX::XMVectorMin(vMin, vPosVec);
    }

    DirectX::XMStoreFloat3(&vertexMaxOut, vMax);
    DirectX::XMStoreFloat3(&vertexMinOut, vMin);
}
