#pragma once
#include "Mesh.h"
#include "RenderData.h"

class StaticMesh : public Mesh
{
    RenderData* pRenderData = nullptr;

public:
    StaticMesh() = default;
    StaticMesh(std::string name, int ID);

    [[nodiscard]] inline RenderData* GetRenderData() const noexcept { return pRenderData; }

};