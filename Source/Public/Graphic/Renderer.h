#pragma once
#include <memory>
#include <windef.h>

struct D3D12_CPU_DESCRIPTOR_HANDLE;
class FrameResourceManager;
class SwapChain;
class DepthStencil;
class CommandList;
class Viewport;
class ScissorRect;
class PsoManager;


class Renderer
{
    std::unique_ptr<FrameResourceManager> mFrameResourceManager;
    std::unique_ptr<SwapChain> mSwapChain;
    std::unique_ptr<DepthStencil> mDepthStencil;
    std::unique_ptr<Viewport> mViewport;
    std::unique_ptr<ScissorRect> mScissorRect;
    std::unique_ptr<PsoManager> mPsoManager;

public:
    Renderer(HWND windowHandle);
    ~Renderer();
    Renderer(const Renderer&) = delete;
    Renderer& operator=(const Renderer&) = delete;
    Renderer(const Renderer&& rhs) = delete;
    Renderer& operator=(const Renderer&& rhs) = delete;

    void StartDrawFrame();

private:
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilView();
};