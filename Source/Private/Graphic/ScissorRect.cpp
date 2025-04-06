#include "ScissorRect.h"

ScissorRect::ScissorRect(UINT left, UINT top, UINT right, UINT bottom)
{
    Set(left, top, right, bottom);
}

void ScissorRect::Set(UINT left, UINT top, UINT right, UINT bottom)
{
    mRect.left = static_cast<long>(left);
    mRect.top = static_cast<long>(top);
    mRect.right = static_cast<long>(right);
    mRect.bottom = static_cast<long>(bottom);
}

void ScissorRect::Set(const D3D12_RECT& rect)
{
    mRect = rect;
}

void ScissorRect::Apply(ID3D12GraphicsCommandList* commandList) const
{
    assert(commandList);
    if (commandList)
    {
        commandList->RSSetScissorRects(1, &mRect);
    }
}