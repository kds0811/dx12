#pragma once
#include "Mesh.h"
#include "RenderData.h"


class StaticMesh : public Mesh
{
    GeoRenderData* mGeoRenderData = nullptr;

public:
    StaticMesh() = default;
    StaticMesh(std::string name, int ID);

    [[nodiscard]] inline GeoRenderData* GetGeoRenderData() const noexcept { return mGeoRenderData; }
    inline void SetGeoRenderData(GeoRenderData* geoRenderData) noexcept { mGeoRenderData = geoRenderData; }
};