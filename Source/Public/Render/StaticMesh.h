#pragma once
#include "Mesh.h"
#include "GeometryCommon.h"

class StaticMesh : public Mesh
{
    GeoRenderData* mGeoRenderData = nullptr;

public:
    StaticMesh() = default;
    StaticMesh(std::string name, int ID);

    [[nodiscard]] inline GeoRenderData* GetGeoRenderData() const noexcept { return mGeoRenderData; }
    void SetGeoRenderData(GeoRenderData* rd);
};