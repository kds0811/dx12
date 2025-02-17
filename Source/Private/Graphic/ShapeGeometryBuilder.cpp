#include "ShapeGeometryBuilder.h"
#include <algorithm>
#include <string>
#include "Transform.h"

using namespace DirectX;

std::unique_ptr<MeshGeometry> ShapeGeometryBuilder::BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    // Add geometries
    AddGeometry(mGeometryGenerator.CreateBox(1.f, 1.0f, 1.0f, 3), EPrimitiveType::BOX);
    AddGeometry(mGeometryGenerator.CreateGrid(50.0f, 50.0f, 50, 50), EPrimitiveType::GRID);
    AddGeometry(mGeometryGenerator.CreateSphere(1.f, 20, 20), EPrimitiveType::SPHERE);
    AddGeometry(mGeometryGenerator.CreateCylinder(2.0f, 1.f, 3.0f, 20, 20), EPrimitiveType::CYLINDER);

    // Mounts
    AddGeometry(mGeometryGenerator.CreateGrid(160.0f, 160.0f, 160, 160), EPrimitiveType::LAND);

    // skull
    AddGeometry(mGeometryLoader.LoadGeometryFromTXTFile("..//Source//Models//skull.txt"), EPrimitiveType::MESH);

    // assimp load model
    AddGeometry(mAssimpLoader.LoadGeometryFromFile("..//Source//Models//ironpod.fbx"), EPrimitiveType::MODEL);

    // assimp load model
    AddGeometry(mAssimpLoader.LoadGeometryFromFile("..//Source//Models//UpgradedKleinerToilet.fbx"), EPrimitiveType::MODEL1);

    // assimp load model
    AddGeometry(mAssimpLoader.LoadGeometryFromFile("..//Source//Models//spider.fbx"), EPrimitiveType::MODELSPIDERFBX);
    AddGeometry(mAssimpLoader.LoadGeometryFromFile("..//Source//Models//spider.obj"), EPrimitiveType::MODELSPIDEROBJ);

    // HUESITOS
    AddGeometry(mAssimpLoader.LoadGeometryFromFile("..//Source//Models//huesitos.fbx"), EPrimitiveType::HUESITOS);

    // SPINER
    AddGeometry(mAssimpLoader.LoadGeometryFromFile("..//Source//Models//spinner.fbx"), EPrimitiveType::SPINER);

    // DRONE
    AddGeometry(mAssimpLoader.LoadGeometryFromFile("..//Source//Models//drone_simple.fbx"), EPrimitiveType::DRONE);

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
    AddGeometry(mGeometryGenerator.CreateGrid(145.0f, 145.0f, 145, 145), EPrimitiveType::WAVES);

    CalculateOffsets();

    auto vertices = CreateVertexBuffer();
    auto indices = CreateIndexBuffer();

    auto result = CreateMeshGeometry(device, cmdList, vertices, indices, "waterGeo", true);

    // Clear geometries for next use
    mGeometries.clear();

    return result;
}

std::unique_ptr<MeshGeometry> ShapeGeometryBuilder::BuildThreeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList)
{
    return std::unique_ptr<MeshGeometry>();
}

void ShapeGeometryBuilder::AddGeometry(const GeometryGenerator::MeshData& mesh, EPrimitiveType type)
{
    GeometryData data;
    data.mesh = mesh;
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
            v.TexC = vertex.TexC;
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

void ShapeGeometryBuilder::ModifyHeightLandVertices(std::vector<Vertex>& vertices)
{
    for (auto& geom : mGeometries)
    {
        if (geom.type == EPrimitiveType::LAND)
        {
            // update vecrtices
            for (size_t i = geom.vertexOffset; i < geom.vertexOffset + geom.mesh.Vertices.size(); ++i)
            {
                vertices[i].Pos.y = GetHillsHeight(vertices[i].Pos.x, vertices[i].Pos.z);
            }
            // update normals
            const int rows = 160;
            const int cols = 160;
            for (int i = 0; i < rows - 1; ++i)
            {
                for (int j = 0; j < cols - 1; ++j)
                {
                    int index = geom.vertexOffset + i * cols + j;

                    Vector v1 = (Vector(vertices[index + cols].Pos) - Vector(vertices[index].Pos)).Normalize();
                    Vector v2 = (Vector(vertices[index + 1].Pos) - Vector(vertices[index].Pos)).Normalize();
                    auto normal = v2.Cross(v1);
                    vertices[index].Normal = DirectX::XMFLOAT3(normal.GetX(), normal.GetY(), normal.GetZ());
                }
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
