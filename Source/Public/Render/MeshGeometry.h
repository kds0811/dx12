#pragma once
#include "GraphicCommonHeaders.h"
#include "Vector.h"

struct SubmeshGeometry
{
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    INT BaseVertexLocation = 0;

    // max and min vertices for create Bounding box
    Vector VertexMax{-FLT_MAX, -FLT_MAX, -FLT_MAX};
    Vector VertexMin{FLT_MAX, FLT_MAX, FLT_MAX};
};


template <typename T>
class Buffer;

class CommandList;

class MeshGeometry
{
    std::wstring mName;

    std::unique_ptr<Buffer<Vertex>> mVertexBuffer;
    std::unique_ptr<Buffer<std::uint16_t>> mIndexBuffer;

    UINT mVertexByteStride = 0;
    DXGI_FORMAT mIndexFormat = DXGI_FORMAT_R16_UINT;

public:
    MeshGeometry(const std::wstring& name, CommandList* cmdList, const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices);
    ~MeshGeometry();

    D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() const;
    D3D12_INDEX_BUFFER_VIEW GetIndexBufferView() const;
};