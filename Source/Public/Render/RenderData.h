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

//public:
//    GeoRenderData(std::string name, MeshGeometry* geo, D3D12_PRIMITIVE_TOPOLOGY type, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation)
//        : mName(std::move(name)), mGeo(geo), mPrimitiveType(type), mIndexCount(indexCount), mStartIndexLocation(startIndexLocation), mBaseVertexLocation(baseVertexLocation)
//    {
//    }
//
//    [[nodiscard]] inline std::string GetName() { return mName; }
//    [[nodiscard]] inline MeshGeometry* GetGeo() const noexcept { return mGeo; }
//    [[nodiscard]] inline D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveType() const noexcept { return mPrimitiveType; }
//    [[nodiscard]] inline UINT GetIndexCount() const noexcept { return mIndexCount; }
//    [[nodiscard]] inline UINT GetStartIndexLocation() const noexcept { return mStartIndexLocation; }
//    [[nodiscard]] inline UINT GetBaseVertexLocation() const noexcept { return mBaseVertexLocation; }
};

struct MatRenderData
{
    EMaterialType Type = EMaterialType::Default;
    std::array<int, 4> SrvIndexes{};
    std::array<bool, 4> TexStatus = {false, false, false, false};
    int MatCBIndex = -1;
};


struct RenderData
{
    GeoRenderData GeoRenderData;
    MatRenderData MatRenderData;
    int ObjCBIndex = -1;
};