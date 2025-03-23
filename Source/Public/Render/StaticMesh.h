#pragma once
#include "Mesh.h"
#include "RenderData.h"


class StaticMesh : public Mesh
{
    GeoRenderData mGeoRenderData;

public:
    StaticMesh(std::string name, int ID, GeoRenderData geo);

    [[nodiscard]] inline GeoRenderData& GetGeoRenderData()  noexcept { return mGeoRenderData; }
    inline void SetGeoRenderData(GeoRenderData geoRenderData) noexcept { mGeoRenderData = geoRenderData; }
};