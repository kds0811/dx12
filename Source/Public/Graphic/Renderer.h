#pragma once
#include <memory>
#include <windef.h>

class FrameResourceManager;
class SwapChain;
class DepthStencil;

class Renderer
{
    std::unique_ptr<FrameResourceManager> mFrameResourceManager;
    std::unique_ptr<SwapChain> mSwapChain;
    std::unique_ptr<DepthStencil> mDepthStencil;

public:
    Renderer(HWND windowHandle);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(const Renderer&& rhs) = delete;
    Renderer& operator=(const Renderer&& rhs) = delete;

private:
    void InitDepthStencil();
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView();
};