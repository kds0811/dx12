#include "ShapeGeometryBuilder.h"


using namespace DirectX;

std::unique_ptr<MeshGeometry> ShapeGeometryBuilder::BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // Add geometries
    AddGeometry(mGeometryGenerator.CreateBox(1.5f, 5.0f, 1.5f, 3), XMFLOAT4(DirectX::Colors::DarkGreen), "box");
    AddGeometry(mGeometryGenerator.CreateGrid(100.0f, 100.0f, 50, 50), XMFLOAT4(DirectX::Colors::ForestGreen), "grid");
    AddGeometry(mGeometryGenerator.CreateSphere(0.5f, 20, 20), XMFLOAT4(DirectX::Colors::Crimson), "sphere");
    AddGeometry(mGeometryGenerator.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20), XMFLOAT4(DirectX::Colors::SteelBlue), "cylinder");

    CalculateOffsets();

    auto vertices = CreateVertexBuffer();
    auto indices = CreateIndexBuffer();

    auto result = CreateMeshGeometry(device, cmdList, vertices, indices);

    // Clear geometries for next use
    mGeometries.clear();

    return std::move(result);
}

void ShapeGeometryBuilder::AddGeometry(const GeometryGenerator::MeshData& mesh, const XMFLOAT4& color, const std::string& name)
{
    GeometryData data;
    data.mesh = mesh;
    data.color = color;
    data.name = name;
    mGeometries.push_back(data);
}

void ShapeGeometryBuilder::CalculateOffsets()
{
    UINT currentVertexOffset = 0;
    UINT currentIndexOffset = 0;

    for (auto& geometry : mGeometries)
    {
        geometry.vertexOffset = currentVertexOffset;
        geometry.indexOffset = currentIndexOffset;

        geometry.submesh.IndexCount = (UINT)geometry.mesh.Indices32.size();
        geometry.submesh.StartIndexLocation = geometry.indexOffset;
        geometry.submesh.BaseVertexLocation = geometry.vertexOffset;

        currentVertexOffset += (UINT)geometry.mesh.Vertices.size();
        currentIndexOffset += (UINT)geometry.mesh.Indices32.size();
    }
    mVertexBufferSize = currentVertexOffset;
    mIndexBufferSize = currentIndexOffset;

}

std::vector<Vertex> ShapeGeometryBuilder::CreateVertexBuffer()
{
    std::vector<Vertex> vertices;
    vertices.reserve(mVertexBufferSize);
    for (const auto& geometry : mGeometries)
    {
        for (const auto& vertex : geometry.mesh.Vertices)
        {
            Vertex v;
            v.Pos = vertex.Position;
            v.Color = geometry.color;
            vertices.push_back(v);
        }
    }
    return vertices;
}

std::vector<std::uint16_t> ShapeGeometryBuilder::CreateIndexBuffer()
{
    std::vector<std::uint16_t> indices;
    indices.reserve(mIndexBufferSize);
    for (auto& geometry : mGeometries)
    {
        auto geometryIndices = geometry.mesh.GetIndices16();
        indices.insert(indices.end(), std::begin(geometryIndices), std::end(geometryIndices));
    }
    return indices;
}

std::unique_ptr<MeshGeometry> ShapeGeometryBuilder::CreateMeshGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList,
    const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices)
{
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = "shapeGeo";

    D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU) >> Kds::App::Check;
    CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);

    D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU) >> Kds::App::Check;
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

    geo->VertexBufferGPU = D3D12Utils::CreateDefaultBuffer(device, cmdList, vertices.data(), vbByteSize, geo->VertexBufferUploader);

    geo->IndexBufferGPU = D3D12Utils::CreateDefaultBuffer(device, cmdList, indices.data(), ibByteSize, geo->IndexBufferUploader);

    geo->VertexByteStride = sizeof(Vertex);
    geo->VertexBufferByteSize = vbByteSize;
    geo->IndexFormat = DXGI_FORMAT_R16_UINT;
    geo->IndexBufferByteSize = ibByteSize;

    // Add DrawArgs
    for (const auto& geometry : mGeometries)
    {
        geo->DrawArgs[geometry.name] = geometry.submesh;
    }

    return geo;
}

