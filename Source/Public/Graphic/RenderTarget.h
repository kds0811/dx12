#pragma once
#include "GraphicCommonHeaders.h"
#include "GpuResource.h"

class RenderTarget : public GpuResource
{
public:
    RenderTarget(std::wstring name, UINT width, UINT height, DXGI_FORMAT format, D3D12_CLEAR_VALUE clearValue, D3D12_RESOURCE_DIMENSION resourceDimension);

    RenderTarget(const RenderTarget& rhs) = delete;
    RenderTarget& operator=(const RenderTarget& rhs) = delete;
    ~RenderTarget() = default;

    [[nodiscard]] inline DescriptorHandle& GetSrvDescriptorHandle() noexcept { return mSrvDescriptorHandle; }
    [[nodiscard]] inline DescriptorHandle& RtvDescriptorHandle() noexcept { return mRtvDescriptorHandle; }

    void OnResize(UINT newWidth, UINT newHeight);

private:
    void BuildResource();
    void BuildDescriptors();

private:
    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    D3D12_CLEAR_VALUE mClearValue{};
    D3D12_RESOURCE_DIMENSION mResourceDimension;
    DescriptorHandle mSrvDescriptorHandle{};
    DescriptorHandle mRtvDescriptorHandle{};
    
};
