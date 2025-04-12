#include "MeshGeometry.h"
#include "Buffer.h"
#include "CommandList.h"

MeshGeometry::MeshGeometry(const std::wstring& name, CommandList* cmdList, const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices) : mName(name)
{
    std::wstring vbName = L"VertexBuffer " + mName;
    mVertexBuffer = std::make_unique<Buffer<Vertex>>(vbName, cmdList, vertices);

    std::wstring ibName = L"IndexBuffer " + mName;
    mIndexBuffer = std::make_unique<Buffer<std::uint16_t>>(ibName, cmdList, indices);
}

MeshGeometry::~MeshGeometry() = default;

D3D12_VERTEX_BUFFER_VIEW MeshGeometry::VertexBufferView() const
{
    D3D12_VERTEX_BUFFER_VIEW vbv{};
    vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
    vbv.StrideInBytes = VertexByteStride;
    vbv.SizeInBytes = VertexBufferByteSize;

    return vbv;
}

D3D12_INDEX_BUFFER_VIEW MeshGeometry::IndexBufferView() const
{
    D3D12_INDEX_BUFFER_VIEW ibv{};
    ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
    ibv.Format = IndexFormat;
    ibv.SizeInBytes = IndexBufferByteSize;

    return ibv;
}

void MeshGeometry::DisposeUploaders()
{
    VertexBufferUploader = nullptr;
    IndexBufferUploader = nullptr;
}
