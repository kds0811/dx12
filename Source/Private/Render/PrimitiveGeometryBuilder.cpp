#include "ShapeGeometryBuilder.h"
#include <algorithm>
#include <string>
#include "Transform.h"
#include <random>
#include "Vector.h"

using namespace DirectX;

std::unique_ptr<MeshGeometry> PrimitiveGeometryBuilder::BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // Add geometries
    AddGeometry(mGeometryGenerator.CreateBox(1.f, 1.0f, 1.0f, 3), EPrimitiveType::BOX);
    AddGeometry(mGeometryGenerator.CreateGrid(50.0f, 50.0f, 50, 50), EPrimitiveType::GRID);
    AddGeometry(mGeometryGenerator.CreateSphere(1.f, 20, 20), EPrimitiveType::SPHERE);
    AddGeometry(mGeometryGenerator.CreateCylinder(2.0f, 1.f, 3.0f, 20, 20), EPrimitiveType::CYLINDER);
    AddGeometry(mGeometryGenerator.CreateGeosphere(1.0f, 0), EPrimitiveType::GEOSPHERE);

    CalculateOffsets();

    auto vertices = CreateVertexBuffer();
    auto indices = CreateIndexBuffer();

    auto result = CreateMeshGeometry(device, cmdList, vertices, indices, "primitiveShapeGeo");

    // Clear geometries for next use
    mGeometries.clear();

    return std::move(result);
}

void PrimitiveGeometryBuilder::AddGeometry(const GeometryGenerator::MeshData& mesh, EPrimitiveType type)
{
    GeometryData data;
    data.meshData = mesh;
    data.type = type;
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
    ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices, std::string meshName)
{
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(T);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = meshName;

    D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU) >> Kds::App::Check;
    CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
    geo->VertexBufferGPU = D3D12Utils::CreateDefaultBuffer(device, cmdList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

    D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU) >> Kds::App::Check;
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);
    geo->IndexBufferGPU = D3D12Utils::CreateDefaultBuffer(device, cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(T);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R16_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    // Add DrawArgs
    for (const auto& geometry : mGeometries)
    {
        geo->DrawArgs[geometry.type] = geometry.submesh;
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
