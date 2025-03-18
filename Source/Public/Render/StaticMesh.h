#pragma once
#include "Mesh.h"
#include "RenderData.h"

class StaticMesh : public Mesh
{
    RenderData* mRenderData = nullptr;

public:
    StaticMesh() = default;
    StaticMesh(std::string name, int ID);

    [[nodiscard]] inline RenderData* GetRenderData() const noexcept { return mRenderData; }
    void SetRenderData(RenderData* rd);
};