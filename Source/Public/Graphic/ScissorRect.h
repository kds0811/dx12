#pragma once
#include "GraphicCommonHeaders.h"

class CommandList;

class ScissorRect
{
private:
    D3D12_RECT mRect;

public:
    ScissorRect(UINT left, UINT top, UINT right, UINT bottom);

    void Set(UINT left, UINT top, UINT right, UINT bottom);
    void Set(const D3D12_RECT& rect);

    [[nodiscard]] const D3D12_RECT& GetScissorRect() const noexcept { return mRect; }
    [[nodiscard]] D3D12_RECT& GetScissorRect() noexcept { return mRect; }

    void SetToPipeline(CommandList* commandList) const;
};