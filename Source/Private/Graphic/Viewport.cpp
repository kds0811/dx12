#include "Viewport.h"
#include "CommandList.h"

Viewport::Viewport(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
    Set(topLeftX, topLeftY, width, height, minDepth, maxDepth);
}

void Viewport::Set(float topLeftX, float topLeftY, float width, float height, float minDepth, float maxDepth)
{
    mViewport.TopLeftX = topLeftX;
    mViewport.TopLeftY = topLeftY;
    mViewport.Width = width;
    mViewport.Height = height;
    mViewport.MinDepth = minDepth;
    mViewport.MaxDepth = maxDepth;
}

void Viewport::Set(const D3D12_VIEWPORT& viewport)
{
    mViewport = viewport;
}

void Viewport::SetToPipeline(CommandList* commandList) const
{
    assert(commandList);
    if (commandList)
    {
        commandList->SetViewports(1, &mViewport);
    }

}