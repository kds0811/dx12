#include "ShapeGeometryBuilder.h"
#include <algorithm>
#include <string>

using namespace DirectX;

std::unique_ptr<MeshGeometry> ShapeGeometryBuilder::BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // Add geometries
    AddGeometry(mGeometryGenerator.CreateBox(1.5f, 5.0f, 1.5f, 3), XMFLOAT4(DirectX::Colors::DarkGreen), ePrimitiveType::BOX);
    AddGeometry(mGeometryGenerator.CreateGrid(50.0f, 50.0f, 50, 50), XMFLOAT4(DirectX::Colors::ForestGreen), ePrimitiveType::GRID);
    AddGeometry(mGeometryGenerator.CreateSphere(0.5f, 20, 20), XMFLOAT4(DirectX::Colors::Crimson), ePrimitiveType::SPHERE);
    AddGeometry(
        mGeometryGenerator.CreateCylinder(0.5f, 0.3f, 3.0f, 20, 20), XMFLOAT4(DirectX::Colors::SteelBlue), ePrimitiveType::CYLINDER);

    //Mounts
    AddGeometry(mGeometryGenerator.CreateGrid(160.0f, 160.0f, 160, 160), XMFLOAT4(DirectX::Colors::SteelBlue), ePrimitiveType::LAND);

    //skull
    AddGeometry(mGeometryLoader.LoadGeometryFromTXTFile("..//Source//Models//skull.txt"), XMFLOAT4(DirectX::Colors::Gray), ePrimitiveType::MESH);

    CalculateOffsets();

    auto vertices = CreateVertexBuffer();
    auto indices = CreateIndexBuffer();

   // modify height land vertices
    ModifyHeightLandVertices(vertices);

    auto result = CreateMeshGeometry(device, cmdList, vertices, indices, "shapeGeo", false);

    // Clear geometries for next use
    mGeometries.clear();

    return std::move(result);
}

std::unique_ptr<MeshGeometry> ShapeGeometryBuilder::BuildWavesGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // waves
    AddGeometry(mGeometryGenerator.CreateGrid(145.0f, 145.0f, 145, 145), XMFLOAT4(DirectX::Colors::Blue), ePrimitiveType::WAVES);

    CalculateOffsets();

    auto vertices = CreateVertexBuffer();
    auto indices = CreateIndexBuffer();

    auto result = CreateMeshGeometry(device, cmdList, vertices, indices, "waterGeo", true);

    // Clear geometries for next use
    mGeometries.clear();

    return result;
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
            v.Normal = vertex.Normal;
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
    const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices, std::string meshName, bool isWavesMesh)
{
    const UINT vbByteSize = (UINT)vertices.size() * sizeof(Vertex);
    const UINT ibByteSize = (UINT)indices.size() * sizeof(std::uint16_t);

    auto geo = std::make_unique<MeshGeometry>();
    geo->Name = meshName;

    if (!isWavesMesh)
    {
        D3DCreateBlob(vbByteSize, &geo->VertexBufferCPU) >> Kds::App::Check;
        CopyMemory(geo->VertexBufferCPU->GetBufferPointer(), vertices.data(), vbByteSize);
        geo->VertexBufferGPU = D3D12Utils::CreateDefaultBuffer(device, cmdList, vertices.data(), vbByteSize, geo->VertexBufferUploader);
    }
    else
    {
        // Set dynamically for Waves
        geo->VertexBufferCPU = nullptr;
        geo->VertexBufferGPU = nullptr;
    }

    D3DCreateBlob(ibByteSize, &geo->IndexBufferCPU) >> Kds::App::Check;
    CopyMemory(geo->IndexBufferCPU->GetBufferPointer(), indices.data(), ibByteSize);

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

void ShapeGeometryBuilder::ModifyHeightLandVertices(std::vector<Vertex>& vertices)
{
    for (auto& geom : mGeometries)
    {
        if (geom.type == ePrimitiveType::LAND)
        {
            for (size_t i = geom.vertexOffset; i < geom.vertexOffset + geom.mesh.Vertices.size(); ++i)
            {
                vertices[i].Pos.y = GetHillsHeight(vertices[i].Pos.x, vertices[i].Pos.z);
            }
        }
    }
}

float ShapeGeometryBuilder::GetHillsHeight(float x, float z) const
{
    const float gridSize = 160.0f;
    const float halfSize = gridSize * 0.5f;
    const float borderSize = gridSize * 0.05f;  

    float dx = abs(x);
    float dz = abs(z);

    if (dx > halfSize - borderSize || dz > halfSize - borderSize)
    {
        return 0.0f;
    }

    float borderFalloff = 1.0f;
    if (dx > halfSize - borderSize * 2)
    {
        borderFalloff *= (halfSize - borderSize - dx) / borderSize;
    }
    if (dz > halfSize - borderSize * 2)
    {
        borderFalloff *= (halfSize - borderSize - dz) / borderSize;
    }

    float height = 0.3f * (z * sinf(0.1f * x) + x * cosf(0.1f * z));
    return std::max<float>(-5.0f, height * borderFalloff);
}

void ShapeGeometryBuilder::ModifyWaveMeshGeometry(std::unique_ptr<MeshGeometry>& geom) {}


