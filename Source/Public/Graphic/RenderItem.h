#pragma once

#include "MathHelper.h"
#include "D3D12Utils.h"



// Lightweight structure stores parameters to draw a shape.
struct RenderItem
{
    RenderItem() = default;

    MeshGeometry* Geo = nullptr;

    // Primitive topology.
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

    // DrawIndexedInstanced parameters.
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    int BaseVertexLocation = 0;
};