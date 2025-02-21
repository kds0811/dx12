#pragma once
#include "MathHelper.h"
#include "D3D12Utils.h"

// Lightweight structure stores parameters to draw a shape.
struct RenderItem
{
    RenderItem() = default;

    DirectX::XMFLOAT4X4 TexTransform = MathHelper::Identity4x4();

    Material* Mat = nullptr;
    MeshGeometry* Geo = nullptr;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;

    // Index into GPU constant buffer corresponding to the ObjectCB for this render item.
    int ObjCBIndex = -1;
};