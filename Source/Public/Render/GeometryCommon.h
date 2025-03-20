#pragma once
#include "Vector.h"
#include "DirectXMath.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include "GraphicError.h"

struct SubmeshGeometry
{
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    INT BaseVertexLocation = 0;

    // max and min vertices for create Bounding box
    Vector VertexMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};
    Vector VertexMin{FLT_MAX, FLT_MAX, FLT_MAX};
};

struct MeshGeometry
{
    // Give it a name so we can look it up by name.
    std::string Name;

    // System memory copies.  Use Blobs because the vertex/index format can be generic.
    // It is up to the client to cast appropriately.
    Microsoft::WRL::ComPtr<ID3DBlob> VertexBufferCPU = nullptr;
    Microsoft::WRL::ComPtr<ID3DBlob> IndexBufferCPU = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

    // Data about the buffers.
    UINT VertexByteStride = 0;
    UINT VertexBufferByteSize = 0;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
    UINT IndexBufferByteSize = 0;

    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv{};
        vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const
    {
        D3D12_INDEX_BUFFER_VIEW ibv{};
        ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = IndexFormat;
        ibv.SizeInBytes = IndexBufferByteSize;

        return ibv;
    }

    // We can free this memory after we finish upload to the GPU.
    void DisposeUploaders()
    {
        VertexBufferUploader = nullptr;
        IndexBufferUploader = nullptr;
    }
};


class GeoRenderData
{
    std::string mName;
    MeshGeometry* mGeo = nullptr;
    D3D12_PRIMITIVE_TOPOLOGY mPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    UINT mIndexCount = 0;
    UINT mStartIndexLocation = 0;
    UINT mBaseVertexLocation = 0;

public:
    GeoRenderData(std::string name, MeshGeometry* geo, D3D12_PRIMITIVE_TOPOLOGY type, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation)
        : mName(std::move(name)), mGeo(geo), mPrimitiveType(type), mIndexCount(indexCount), mStartIndexLocation(startIndexLocation), mBaseVertexLocation(baseVertexLocation)
    {
    }

    [[nodiscard]] inline std::string GetName() { return mName; }
    [[nodiscard]] inline MeshGeometry* GetGeo() const noexcept { return mGeo; }
    [[nodiscard]] inline D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveType() noexcept { return mPrimitiveType; }
    [[nodiscard]] inline UINT GetIndexCount() noexcept { return mIndexCount; }
    [[nodiscard]] inline UINT GetStartIndexLocation() noexcept { return mStartIndexLocation; }
    [[nodiscard]] inline UINT GetBaseVertexLocation() noexcept { return mBaseVertexLocation; }
};
