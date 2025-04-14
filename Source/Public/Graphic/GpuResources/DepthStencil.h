#pragma once
#include "GraphicCommonHeaders.h"
#include "GpuResource.h"

class DepthStencil : public GpuResource
{
    UINT mWidth = 0;
    UINT mHeight = 0;
    DXGI_FORMAT mFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
    DescriptorHandle mDsvDescriptorHandle{};

public:
    DepthStencil(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_STATES state);

    DepthStencil(const DepthStencil& rhs) = delete;
    DepthStencil& operator=(const DepthStencil& rhs) = delete;
    DepthStencil(const DepthStencil&& rhs) noexcept = delete;
    DepthStencil& operator=(const DepthStencil&& rhs) noexcept = delete;
    ~DepthStencil();

    [[nodiscard]] inline DescriptorHandle& GetDsvDescriptorHandle() noexcept { return mDsvDescriptorHandle; }
    [[nodiscard]] bool IsInitialized() const noexcept { return ResourceIsInitialized() && !mDsvDescriptorHandle.IsNull(); }
    void OnResize(UINT newWidth, UINT newHeight);

private:
    void Initialize(const std::wstring& name, UINT width, UINT height, DXGI_FORMAT format, D3D12_RESOURCE_STATES state);
    void BuildResource(const std::wstring& name, D3D12_RESOURCE_STATES state);
    void BuildDescriptors();

    void Destroy();
    void DestroyDescriptorHandles();
};