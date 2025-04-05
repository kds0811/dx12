#pragma once
#include <memory>

class FrameResourceManager;
class SwapChain;

class Renderer
{
    std::unique_ptr<FrameResourceManager> mFrameResourceManager;
    std::unique_ptr<SwapChain> mSwapChain;


public:
    Renderer(HWND windowHandle);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(const Renderer&& rhs) = delete;
    Renderer& operator=(const Renderer&& rhs) = delete;
};