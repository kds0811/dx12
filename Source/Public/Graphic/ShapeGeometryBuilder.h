#pragma once
#include "GeometryGenerator.h"
#include "D3D12Utils.h"
#include <memory>
#include "DirectXMath.h"
#include "DirectXColors.h"
#include "GraphicError.h"
#include <vector>
#include "GeometryLoader.h"
#include "FrameResource.h"
#include "AssimpLoader.h"
#include <variant>

struct TreeSpriteVertex
{
    DirectX::XMFLOAT3 Pos;
    DirectX::XMFLOAT2 Size;
};

struct ThreeSpriteMeshData
{
    std::vector<TreeSpriteVertex> Vertices;
    std::vector<uint32_t> Indices32;
};

struct GeometryData
{
    std::variant<GeometryGenerator::MeshData, ThreeSpriteMeshData> meshData;
    EPrimitiveType type;
    UINT vertexOffset = 0;
    UINT indexOffset = 0;
    SubmeshGeometry submesh;
};

class ShapeGeometryBuilder
{

    std::vector<GeometryData> mGeometries;

    GeometryGenerator mGeometryGenerator;
    GeometryLoader mGeometryLoader;
    AssimpLoader mAssimpLoader;

    UINT mVertexBufferSize = 0;
    UINT mIndexBufferSize = 0;

public:
    ShapeGeometryBuilder() = default;
    std::unique_ptr<MeshGeometry> BuildShapeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    std::unique_ptr<MeshGeometry> BuildWavesGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);
    std::unique_ptr<MeshGeometry> BuildThreeGeometry(ID3D12Device* device, ID3D12GraphicsCommandList* cmdList);

private:
    void AddGeometry(const GeometryGenerator::MeshData& mesh, EPrimitiveType type);
    void AddGeometry(const ThreeSpriteMeshData& mesh, EPrimitiveType type);

    void CalculateOffsets();
    std::vector<Vertex> CreateVertexBuffer();
    std::vector<std::uint16_t> CreateIndexBuffer();
    void ModifyHeightLandVertices(std::vector<Vertex>& vertices);
    float GetHillsHeight(float x, float z) const;

    ThreeSpriteMeshData GenerateTreePoints(float gridSize, int treeCount);

    template <typename T>
    std::unique_ptr<MeshGeometry> CreateMeshGeometry(
        ID3D12Device* device, ID3D12GraphicsCommandList* cmdList, const std::vector<T>& vertices, const std::vector<std::uint16_t>& indices, std::string meshName, bool isWavesMesh)
    {
        const UINT vbByteSize = (UINT)vertices.size() * sizeof(T);
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
};

template <typename T>
void CalculateMinMaxVertices(DirectX::XMFLOAT3& vertexMinOut, DirectX::XMFLOAT3& vertexMaxOut, const std::vector<T>& vertices)
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