#pragma once
#include "D3D12Utils.h"
#include <string>

class RenderData
{
    std::string mName;
    MeshGeometry* mGeo = nullptr;
    D3D12_PRIMITIVE_TOPOLOGY mPrimitiveType = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    UINT mIndexCount = 0;
    UINT mStartIndexLocation = 0;
    UINT mBaseVertexLocation = 0;

public:
    RenderData(std::string name, MeshGeometry* geo, D3D12_PRIMITIVE_TOPOLOGY type, UINT indexCount, UINT startIndexLocation, UINT baseVertexLocation)
        :
        mName(std::move(name)),
        mGeo(geo),
        mPrimitiveType(type),
        mIndexCount(indexCount),
        mStartIndexLocation(startIndexLocation),
        mBaseVertexLocation(baseVertexLocation)
    {
    }

    [[nodiscard]] inline std::string GetName()  {return mName;}
    [[nodiscard]] inline MeshGeometry* GetGeo() const noexcept { return mGeo; }
    [[nodiscard]] inline D3D12_PRIMITIVE_TOPOLOGY GetPrimitiveType() noexcept { return mPrimitiveType; }
    [[nodiscard]] inline UINT GetIndexCount() noexcept { return mIndexCount; }
    [[nodiscard]] inline UINT GetStartIndexLocation() noexcept { return mStartIndexLocation; }
    [[nodiscard]] inline UINT GetBaseVertexLocation() noexcept { return mBaseVertexLocation; }
};