#pragma once
#include "Vector.h"
#include "DirectXMath.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include <directx/d3dx12.h>
#include "GraphicError.h"
#include <string>
#include "Material.h"
#include "Texture.h"
#include "GeometryCommon.h"
#include <array>
#include <bitset>

struct GeoRenderData
{
    std::string Name{};
    MeshGeometry* Geo = nullptr;
    D3D12_PRIMITIVE_TOPOLOGY PrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    UINT IndexCount = 0;
    UINT StartIndexLocation = 0;
    UINT BaseVertexLocation = 0;
};

struct MatRenderData
{
    static constexpr size_t TextureArraySize = 4;
    static constexpr size_t BaseColorTexIndex = 0;
    static constexpr size_t NormalMapTexIndex = 1;
    static constexpr size_t RoughnessMapTexIndex = 2;
    static constexpr size_t MetallicMapTexIndex = 3;

    EMaterialType Type = EMaterialType::Default;
    std::array<int, TextureArraySize> SrvIndexes{};
    std::array<bool, TextureArraySize> TexStatus = {false, false, false, false};
    int MatCBIndex = -1;
};


struct RenderData
{
    GeoRenderData GeoRenderData;
    MatRenderData MatRenderData;
    int ObjCBIndex = -1;
};