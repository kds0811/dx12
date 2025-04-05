#include "Renderer.h"
#include "FrameResourceManager.h"
#include "SwapChain.h"


Renderer::Renderer(HWND windowHandle)
{
    mFrameResourceManager = std::make_unique<FrameResourceManager>();
    mSwapChain = std::make_unique<SwapChain>(windowHandle);
}

Renderer::~Renderer() {}
