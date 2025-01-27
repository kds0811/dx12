#include "ShapeGeometryBuilder.h"

using namespace DirectX;

std::unique_ptr<MeshGeometry> ShapeGeometryBuilder::BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // Add geometries
    AddGeometry(mGeometryGenerator.CreateBox(1.5f, 5.0f, 1.5f, 3), XMFLOAT4(DirectX::Colors::DarkGreen), ePrimitiveType::BOX);
    AddGeometry(mGeometryGenerator.CreateGrid(50.0f, 50.0f, 50, 50), XMFLOAT4(DirectX::Colors::ForestGreen), ePrimitiveType::GRID);
    AddGeometry(mGeometryGenerator.CreateSphere(0.5f, 20, 20), XMFLOAT4(DirectX::Colors::Crimson), ePrimitiveType::SPHERE);
    AddGeometry(
        mGeometryGenerator.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20), XMFLOAT4(DirectX::Colors::SteelBlue), ePrimitiveType::CYLINDER);
    AddGeometry(mGeometryGenerator.CreateGrid(160.0f, 160.0f, 50, 50), XMFLOAT4(DirectX::Colors::SteelBlue), ePrimitiveType::LAND);

    CalculateOffsets();

    auto vertices = CreateVertexBuffer();
    auto indices = CreateIndexBuffer();

    auto result = CreateMeshGeometry(device, cmdList, vertices, indices);

    // Clear geometries for next use
    mGeometries.clear();

    return std::move(result);
}

void ShapeGeometryBuilder::AddGeometry(const GeometryGenerator::MeshData& mesh, const XMFLOAT4& color, ePrimitiveType type)
{
    GeometryData data;
    data.mesh = mesh;
    data.color = color;
    data.type = type;
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
        geo->DrawArgs[geometry.type] = geometry.submesh;
    }

    return geo;
}

std::vector<GeometryGenerator::Vertex> ShapeGeometryBuilder::CreateLandGeometry(GeometryGenerator::MeshData& meshData)
{

    std::vector<GeometryGenerator::Vertex> vertices(meshData.Vertices.size());
    for (size_t i = 0; i < meshData.Vertices.size(); ++i)
    {
        auto& p = meshData.Vertices[i].Position;
        vertices[i].Pos = p;
        vertices[i].Pos.y = GetHillsHeight(p.x, p.z);

        // Color the vertex based on its height.
        if (vertices[i].Pos.y < -10.0f)
        {
            // Sandy beach color.
            vertices[i].Color = XMFLOAT4(1.0f, 0.96f, 0.62f, 1.0f);
        }
        else if (vertices[i].Pos.y < 5.0f)
        {
            // Light yellow-green.
            vertices[i].Color = XMFLOAT4(0.48f, 0.77f, 0.46f, 1.0f);
        }
        else if (vertices[i].Pos.y < 12.0f)
        {
            // Dark yellow-green.
            vertices[i].Color = XMFLOAT4(0.1f, 0.48f, 0.19f, 1.0f);
        }
        else if (vertices[i].Pos.y < 20.0f)
        {
            // Dark brown.
            vertices[i].Color = XMFLOAT4(0.45f, 0.39f, 0.34f, 1.0f);
        }
        else
        {
            // White snow.
            vertices[i].Color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
        }
    }
    return vertices;
}

float ShapeGeometryBuilder::GetHillsHeight(float x, float z) const
{
    return 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
}
