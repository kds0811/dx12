#pragma once
#include "GraphicCommonHeaders.h"

class Viewport
{
private:
    D3D12_VIEWPORT mViewport{};

public:
    Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);

    void Set(float topLeftX, float topLeftY, float width, float height, float minDepth = 0.0f, float maxDepth = 1.0f);
    void Set(const D3D12_VIEWPORT& viewport);

    [[nodiscard]] const D3D12_VIEWPORT& GetViewport() const noexcept { return mViewport; }
    [[nodiscard]] D3D12_VIEWPORT& GetViewport() noexcept { return mViewport; }

    void Apply(ID3D12GraphicsCommandList* commandList) const;
};