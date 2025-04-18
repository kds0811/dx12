#include "MeshGeometry.h"
#include "Buffer.h"
#include "CommandList.h"

MeshGeometry::MeshGeometry(const std::wstring& name, CommandList* cmdList, const std::vector<Vertex>& vertices, const std::vector<std::uint16_t>& indices) : mName(name)
{
    std::wstring vbName = L"VertexBuffer " + mName;
    mVertexBuffer = std::make_unique<Buffer<Vertex>>(vbName, cmdList, vertices);
    mVertexByteStride = sizeof(Vertex);

    std::wstring ibName = L"IndexBuffer " + mName;
    mIndexBuffer = std::make_unique<Buffer<std::uint16_t>>(ibName, cmdList, indices);
    mIndexFormat = DXGI_FORMAT_R16_UINT;
}

MeshGeometry::~MeshGeometry() = default;

void MeshGeometry::AddSubmeshDrawArgs(const std::string& name, SubmeshGeometry drawArgs)
{
    if (mDrawArgs.contains(name))
    {
        LOG_ERROR("SubMeshGeometry ", name, " allredy on container");
        return;
    }
    mDrawArgs[name] = drawArgs;
}

D3D12_VERTEX_BUFFER_VIEW MeshGeometry::GetVertexBufferView() const
{
    assert(mVertexBuffer);
    if (!mVertexBuffer)
    {
        LOG_ERROR("mVertexBuffer is nullptr");
        return D3D12_VERTEX_BUFFER_VIEW{};
    }

    D3D12_VERTEX_BUFFER_VIEW vbv{};
    vbv.BufferLocation = mVertexBuffer->GetResourceGpuVirtualAddress();
    vbv.StrideInBytes = mVertexByteStride;
    vbv.SizeInBytes = mVertexBuffer->GetBufferSize();

    return vbv;
}

D3D12_INDEX_BUFFER_VIEW MeshGeometry::GetIndexBufferView() const
{
    assert(mIndexBuffer);
    if (!mIndexBuffer)
    {
        LOG_ERROR("mIndexBuffer is nullptr");
        return D3D12_INDEX_BUFFER_VIEW{};
    }

    D3D12_INDEX_BUFFER_VIEW ibv{};
    ibv.BufferLocation = mIndexBuffer->GetResourceGpuVirtualAddress();
    ibv.Format = mIndexFormat;
    ibv.SizeInBytes = mIndexBuffer->GetBufferSize();

    return ibv;
}

